#!/usr/bin/env node
/*
 * joy 绑定 API 契约工具（P0）
 *
 * 只做三件事：解析 joy.h、维护 api/joy.idl、产出 api/coverage.md。
 * P0 阶段不生成任何会被调用的胶水代码，现有各模块的 bindings 目录一行不动。
 *
 * 用法（任意目录，零 npm 依赖，只用 Node 内置 fs/path）：
 *   node gen_bindings.js            从 joy.idl 重生成 coverage.md（幂等）
 *   node gen_bindings.js --scan     从 joy.h 更新 joy.idl（保留人工字段）并重生成报告
 *   node gen_bindings.js --check    只校验不写文件，供 CI 做「生成物无 diff」门禁
 *
 * 三段结构：parse（解析 joy.h）→ classify（分级 / 推断所有权）→ emit（IDL / 报告）。
 */
'use strict';

const fs = require('fs');
const path = require('path');

const HERE = __dirname;
const JOY_REPO = path.resolve(HERE, '..');
const JOY_H = path.join(JOY_REPO, 'joy.h');
const JMATH_H = path.join(JOY_REPO, 'jmath.h');
const IDL = path.join(HERE, 'joy.idl');
const REPORT = path.join(HERE, 'coverage.md');
const THUNKS = path.join(HERE, 'thunks_generated.c');
const { buildThunks } = require('./emit_thunks');

/* 有意排除：条件编译下不存在、返回缓冲约定依赖 SDL、插件系统停用、测试符号。
   与 modules/cs/gen_csclib.js 的 SKIP_* 保持一致。 */
const SKIP_PREFIXES = ['utils_', 'jspine_', 'modules_', 'module_instance_'];
const SKIP_NAMES = new Set(['jecs_test']);
const DESTROY_SUFFIXES = ['destroy', 'free', 'release', 'delete'];

const SCALARS = new Set([
  'void', 'bool', 'char', 'float', 'double',
  'int', 'unsigned', 'unsigned int', 'long', 'unsigned long',
  'short', 'unsigned short',
  'int8_t', 'uint8_t', 'int16_t', 'uint16_t',
  'int32_t', 'uint32_t', 'int64_t', 'uint64_t',
  'size_t', 'ssize_t', 'intptr_t', 'uintptr_t',
  'fp_t', 'ft_t',
]);

/* ------------------------------------------------------------------ parse */

/* 块注释只清字符、保留换行，否则 collectDecls 记下的 joy.h 行号会偏移。 */
function stripComments(src) {
  return src
    .replace(/\/\*[\s\S]*?\*\//g, m => m.replace(/[^\n]/g, ''))
    .replace(/\/\/[^\n]*/g, '');
}

/* 值类型分散在 joy.h 与 jmath.h（vec2_t / vec3_t / vec2f_t 在 jmath.h）。 */
function collectStructs() {
  const out = new Set();
  for (const file of [JOY_H, JMATH_H]) {
    if (!fs.existsSync(file)) continue;
    const src = stripComments(fs.readFileSync(file, 'utf8'));
    const re = /typedef\s+struct\s*\w*\s*\{([^}]*)\}\s*([A-Za-z_]\w*)\s*;/g;
    let m;
    while ((m = re.exec(src)) !== null) {
      if (m[2].endsWith('_t')) out.add(m[2]);
    }
  }
  return out;
}

/* joy.h 里有 81 个跨行的 JOY_API 声明，必须收集到分号为止。 */
function collectDecls(src) {
  const lines = stripComments(src).split(/\r?\n/);
  const out = [];
  let buf = '';
  let active = false;
  let startLine = 0;
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    if (line.trimStart().startsWith('#')) continue;
    if (!active) {
      const idx = line.indexOf('JOY_API');
      if (idx < 0) continue;
      active = true;
      startLine = i + 1;
      buf = line.slice(idx + 'JOY_API'.length);
    } else {
      buf += ' ' + line;
    }
    const semi = buf.indexOf(';');
    if (semi >= 0) {
      const decl = buf.slice(0, semi).replace(/\s+/g, ' ').trim();
      if (decl) out.push({ decl, line: startLine });
      buf = '';
      active = false;
    }
  }
  return out;
}

function normType(t) {
  return t.replace(/\bconst\b/g, ' ')
    .replace(/\s+/g, ' ')
    .trim()
    .replace(/\s*\*\s*/g, '*');
}

function parseParams(raw) {
  const t = raw.trim();
  if (!t || t === 'void') return { params: [], reason: null };
  if (t.includes('...')) return { params: null, reason: '可变参数' };
  if (t.includes('(*')) return { params: null, reason: '函数指针 / 回调' };
  const out = [];
  for (const piece of t.split(',')) {
    const s = piece.trim();
    if (s.includes('[')) return { params: null, reason: '数组参数' };
    const m = s.match(/^(.*?)([A-Za-z_]\w*)\s*$/);
    if (!m) return { params: null, reason: '参数无法解析' };
    out.push({ name: m[2], ctype: normType(m[1]), isConst: /\bconst\b/.test(m[1]) });
  }
  return { params: out, reason: null };
}

function parseDecl(raw, line) {
  const m = raw.match(/^(.+?)\s+([A-Za-z_]\w*)\s*\((.*)\)$/);
  if (!m) return null;
  const parsed = parseParams(m[3]);
  return { name: m[2], ret: normType(m[1]), params: parsed.params, reason: parsed.reason, line };
}

/* -------------------------------------------------------------- classify */

function isOpaque(t) {
  return t === 'void*' || (t.endsWith('_p') && !t.includes('*'));
}

/* create_window / destroy_window 这类通用动词开头的名字，用第二段做域名。 */
const GENERIC_FIRST = new Set([
  'create', 'destroy', 'find', 'get', 'set', 'new', 'free', 'init',
  'make', 'load', 'save', 'add', 'remove', 'is', 'has', 'to', 'from',
]);

function domainOf(name) {
  const parts = name.split('_');
  if (parts.length > 1 && GENERIC_FIRST.has(parts[0])) return parts[1];
  return parts[0];
}

/* 返回句柄的 create → 找配对的 destroy/free/release/delete。 */
function inferOwns(name, ret, declNames) {
  if (!isOpaque(ret)) return null;
  const domain = domainOf(name);
  for (const suffix of DESTROY_SUFFIXES) {
    const exact = name.replace(/_create[\s\S]*$/, '_' + suffix);
    if (declNames.has(exact)) return exact;
    const byDomain = domain + '_' + suffix;
    if (declNames.has(byDomain)) return byDomain;
  }
  return null;
}

/* 分级按风险排序：X(有意排除) L3(手写) L2(需静态断言) L1(句柄) L0(直接)。 */
function classify(name, ret, params, reason, structs) {
  if (SKIP_NAMES.has(name) || SKIP_PREFIXES.some(p => name.startsWith(p))) {
    return { level: 'X', reason: '有意排除（条件编译 / 插件系统 / 测试符号）' };
  }
  if (params === null) return { level: 'L3', reason };
  if (structs.has(ret)) return { level: 'L3', reason: '结构体按值返回' };

  let hasStructParam = false;
  let hasHandle = isOpaque(ret);
  for (const p of params) {
    const t = p.ctype;
    const base = t.replace(/\*$/, '');
    if (t.includes('*')) {
      if (t === 'char*') {
        if (!p.isConst) return { level: 'L3', reason: '非 const char* 输出缓冲' };
      } else if (isOpaque(t)) {
        hasHandle = true;
      } else if (structs.has(base)) {
        return { level: 'L3', reason: '结构体指针参数' };
      } else {
        return { level: 'L3', reason: '非不透明指针 / 输出参数：' + t };
      }
    } else if (structs.has(t)) {
      hasStructParam = true;
    } else if (isOpaque(t)) {
      hasHandle = true;
    } else if (!SCALARS.has(t)) {
      return { level: 'L3', reason: '未知类型需人工标注：' + t };
    }
  }
  if (hasStructParam) return { level: 'L2', reason: '结构体按值参数（可展开成标量）' };
  if (hasHandle) return { level: 'L1', reason: '不透明指针 → 句柄 / 所有权' };
  return { level: 'L0', reason: '纯标量 + const char*' };
}

function scanJoyHeader() {
  const src = fs.readFileSync(JOY_H, 'utf8');
  const structs = collectStructs();
  const parsed = [];
  for (const d of collectDecls(src)) {
    const p = parseDecl(d.decl, d.line);
    if (p) parsed.push(p);
  }
  const declNames = new Set(parsed.map(p => p.name));
  const fresh = parsed.map(p => {
    const c = classify(p.name, p.ret, p.params, p.reason, structs);
    return {
      name: p.name,
      domain: domainOf(p.name),
      level: c.level,
      reason: c.reason,
      status: 'off',
      owns: inferOwns(p.name, p.ret, declNames),
      ret: p.ret,
      params: p.params,
      source: 'joy.h:' + p.line,
      note: '',
    };
  });
  return { fresh, structs, declNames };
}

/* --scan 只覆盖自动字段；owns / status / note 是人工字段，永不覆盖。
   joy.h 里消失的条目保留并标 orphan，避免丢失人工标注。 */
function mergeIdl(fresh) {
  let old = [];
  if (fs.existsSync(IDL)) old = JSON.parse(fs.readFileSync(IDL, 'utf8'));
  const byName = new Map(old.map(e => [e.name, e]));
  const result = fresh.map(f => {
    const o = byName.get(f.name);
    if (!o) return f;
    return {
      ...f,
      owns: o.owns !== undefined ? o.owns : f.owns,
      status: o.status || 'off',
      note: o.note || '',
    };
  });
  const names = new Set(fresh.map(f => f.name));
  for (const o of old) {
    if (!names.has(o.name)) result.push({ ...o, orphan: true });
  }
  return result;
}

/* ------------------------------------------------------------------ emit */

function idlText(entries) {
  return '[\n' + entries.map(e => '  ' + JSON.stringify(e)).join(',\n') + '\n]\n';
}

function jmathMeta() {
  if (!fs.existsSync(JMATH_H)) return null;
  const src = stripComments(fs.readFileSync(JMATH_H, 'utf8'));
  return {
    lines: src.split(/\r?\n/).length,
    inlineFns: (src.match(/\bJOY_INLINE\b/g) || []).length,
    structs: (src.match(/typedef\s+struct\b/g) || []).length,
  };
}

const LEVEL_DESC = {
  L0: '纯标量 + const char*，可直接生成',
  L1: '不透明指针 → 句柄，需所有权测试',
  L2: '结构体按值 → 展开成标量，需静态断言',
  L3: '复杂签名 → 必须手写 thunk，不自动生成',
  X: '有意排除，不进绑定层',
};

function buildReport(entries, meta) {
  const counts = { L0: 0, L1: 0, L2: 0, L3: 0, X: 0 };
  const domains = new Map();
  for (const e of entries) {
    counts[e.level] = (counts[e.level] || 0) + 1;
    if (!domains.has(e.domain)) domains.set(e.domain, { L0: 0, L1: 0, L2: 0, L3: 0, X: 0 });
    domains.get(e.domain)[e.level]++;
  }
  const on = entries.filter(e => e.status === 'on').length;
  const orphan = entries.filter(e => e.orphan);
  const needsOwns = entries.filter(e => e.level !== 'X' && isOpaque(e.ret) && !e.owns);
  const l3 = entries.filter(e => e.level === 'L3');
  const excluded = entries.filter(e => e.level === 'X');
  const jm = jmathMeta();

  const L = [];
  L.push('# joy 绑定 API 覆盖率报告');
  L.push('');
  L.push('> 自动生成，勿手改：由 `api/gen_bindings.js` 从 `joy.h` + `api/joy.idl` 产出。');
  L.push('> **P0 阶段**：本文件与 `joy.idl` 只描述契约，不生成任何会被调用的胶水代码；');
  L.push('> 现有 `modules/*/bindings/` 未被改动，构建行为不变。');
  L.push('');
  L.push('## 总览');
  L.push('');
  L.push('| 项 | 数量 |');
  L.push('|---|---|');
  L.push('| `joy.h` 中 `JOY_API` 声明 | ' + meta.fresh.length + ' |');
  L.push('| `joy.h` 中值类型 `typedef struct` | ' + meta.structs.size + ' |');
  L.push('| IDL 条目 | ' + entries.length + ' |');
  L.push('| 默认开放（`status: on`） | ' + on + ' |');
  L.push('| 待人工确认 `owns` | ' + needsOwns.length + ' |');
  L.push('| orphan（`joy.h` 中已消失） | ' + orphan.length + ' |');
  L.push('');
  L.push('| 级别 | 含义 | 数量 |');
  L.push('|---|---|---|');
  for (const k of ['L0', 'L1', 'L2', 'L3', 'X']) {
    L.push('| ' + k + ' | ' + LEVEL_DESC[k] + ' | ' + counts[k] + ' |');
  }
  L.push('');
  L.push('默认全部 `status: off`。「能力全覆盖」指每条 API 都有 IDL 条目，');
  L.push('不等于已对脚本开放；验证过一条才把 `status` 改成 `on`。');
  L.push('');
  L.push('## 按域分布');
  L.push('');
  L.push('| domain | L0 | L1 | L2 | L3 | X | 合计 |');
  L.push('|---|---|---|---|---|---|---|');
  for (const [d, c] of [...domains.entries()].sort()) {
    const total = c.L0 + c.L1 + c.L2 + c.L3 + c.X;
    L.push('| ' + d + ' | ' + c.L0 + ' | ' + c.L1 + ' | ' + c.L2 + ' | ' + c.L3 + ' | ' + c.X + ' | ' + total + ' |');
  }
  L.push('');
  L.push('## 待办：返回句柄但缺 `owns`');
  L.push('');
  L.push('这些函数返回 `*_p` / `void*`，却没推断出配对的析构函数。');
  L.push('`owns` 是**唯一无法自动发现**的语义，必须人工核对后填进 IDL。');
  L.push('');
  if (!needsOwns.length) {
    L.push('（无）');
  } else {
    L.push('| 函数 | 返回 | 位置 |');
    L.push('|---|---|---|');
    for (const e of needsOwns) L.push('| `' + e.name + '` | `' + e.ret + '` | ' + e.source + ' |');
  }
  L.push('');
  L.push('## L3 清单（必须手写 thunk）');
  L.push('');
  L.push('| 函数 | 原因 | 位置 |');
  L.push('|---|---|---|');
  for (const e of l3) L.push('| `' + e.name + '` | ' + e.reason + ' | ' + e.source + ' |');
  L.push('');
  L.push('## 有意排除（X）');
  L.push('');
  L.push('| 函数 | 位置 |');
  L.push('|---|---|');
  for (const e of excluded) L.push('| `' + e.name + '` | ' + e.source + ' |');
  L.push('');
  if (orphan.length) {
    L.push('## orphan（joy.h 中已消失，人工标注保留）');
    L.push('');
    for (const e of orphan) L.push('- `' + e.name + '`');
    L.push('');
  }
  L.push('## `jmath.h`');
  L.push('');
  if (jm) {
    L.push('`jmath.h` 是头文件内的 `JOY_INLINE` 值类型数学库，没有 `JOY_API` 声明，');
    L.push('因此不在上面的解析范围内，P1 需要单独的解析与「记录」映射。');
    L.push('');
    L.push('| 项 | 数量（粗统计） |');
    L.push('|---|---|');
    L.push('| 行数 | ' + jm.lines + ' |');
    L.push('| `JOY_INLINE` 函数 | ' + jm.inlineFns + ' |');
    L.push('| `typedef struct` 值类型 | ' + jm.structs + ' |');
  } else {
    L.push('（未找到 `jmath.h`）');
  }
  L.push('');
  L.push('## 三种常见操作');
  L.push('');
  L.push('见 `api/README.md`。CI 用 `node api/gen_bindings.js --check` 保证本文件与 IDL 同步。');
  L.push('');
  return L.join('\n');
}

function loadIdl() {
  if (!fs.existsSync(IDL)) {
    console.error('未找到 ' + IDL + '，先执行 node gen_bindings.js --scan');
    process.exit(1);
  }
  return JSON.parse(fs.readFileSync(IDL, 'utf8'));
}

/* 校验：IDL 字段完整、owns 指向存在、on 不能是 L3/X、joy.h 与 IDL 同步、报告最新。 */
function check(entries) {
  const errors = [];
  const names = new Set();
  for (const e of entries) {
    if (names.has(e.name)) errors.push('重复 name: ' + e.name);
    names.add(e.name);
    if (!e.level || !LEVEL_DESC[e.level]) errors.push('缺 level: ' + e.name);
    if (e.status === 'on' && e.level !== 'L0' && e.level !== 'L1') {
      errors.push('P1 只支持 L0/L1 自动 thunk，不能 status:on: ' + e.name + '(' + e.level + ')');
    }
  }
  const meta = scanJoyHeader();
  for (const e of entries) {
    if (e.owns && !meta.declNames.has(e.owns)) {
      errors.push('owns 指向 joy.h 中不存在的函数: ' + e.name + ' -> ' + e.owns);
    }
  }
  const auto = e => JSON.stringify([e.name, e.domain, e.level, e.reason, e.ret, e.params, e.source]);
  for (const f of meta.fresh) {
    const e = entries.find(x => x.name === f.name);
    if (!e) { errors.push('joy.h 新增 API 未进 IDL，请跑 --scan: ' + f.name); continue; }
    if (auto(e) !== auto(f)) errors.push('joy.h 签名已变更，请跑 --scan: ' + f.name);
  }
  /* Windows checkout（core.autocrlf）会把入库的 LF 转成 CRLF，比较前统一换行。 */
  const normEol = s => s.replace(/\r\n/g, '\n');
  const expected = buildReport(entries, meta);
  const onDisk = fs.existsSync(REPORT) ? fs.readFileSync(REPORT, 'utf8') : '';
  if (normEol(onDisk) !== normEol(expected)) {
    errors.push('coverage.md 已过期，请跑 node gen_bindings.js');
  }
  let expectedThunks = null;
  try {
    expectedThunks = buildThunks(entries);
  } catch (err) {
    errors.push('thunk 生成失败: ' + err.message);
  }
  if (expectedThunks !== null) {
    const thunksOnDisk = fs.existsSync(THUNKS) ? fs.readFileSync(THUNKS, 'utf8') : '';
    if (normEol(thunksOnDisk) !== normEol(expectedThunks)) {
      errors.push('thunks_generated.c 已过期，请跑 node gen_bindings.js --emit');
    }
  }
  return errors;
}

/* ------------------------------------------------------------------- main */

function main() {
  const args = process.argv.slice(2);
  try {
    if (args.includes('--check')) {
      const errors = check(loadIdl());
      if (errors.length) {
        for (const e of errors.slice(0, 30)) console.error('  - ' + e);
        if (errors.length > 30) console.error('  ... 共 ' + errors.length + ' 项');
        console.error('check FAILED (' + errors.length + ')');
        return 1;
      }
      console.log('check OK: ' + loadIdl().length + ' IDL entries, report + thunks up to date');
      return 0;
    }
    let entries;
    let meta;
    if (args.includes('--scan')) {
      meta = scanJoyHeader();
      entries = mergeIdl(meta.fresh);
      fs.writeFileSync(IDL, idlText(entries), 'utf8');
      console.log('scan: ' + meta.fresh.length + ' JOY_API decls, ' + entries.length + ' IDL entries');
    } else {
      entries = loadIdl();
      meta = scanJoyHeader();
    }
    fs.writeFileSync(REPORT, buildReport(entries, meta), 'utf8');
    fs.writeFileSync(THUNKS, buildThunks(entries), 'utf8');
    console.log('emit: open=' + entries.filter(e => e.status === 'on').length
      + ', report + thunks_generated.c written');
    return 0;
  } catch (err) {
    console.error('gen_bindings: ' + err.message);
    return 1;
  }
}

process.exit(main());
