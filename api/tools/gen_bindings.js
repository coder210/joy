#!/usr/bin/env node
/*
 * joy 绑定 API 契约工具（P0）
 *
 * 只做三件事：解析 include/joy/ 分域头、维护 api/idl/joy.idl、产出 api/generated/coverage.md。
 * P0 阶段不生成任何会被调用的胶水代码，现有各模块的 bindings 目录一行不动。
 *
 * 用法（任意目录，零 npm 依赖，只用 Node 内置 fs/path）：
 *   node api/tools/gen_bindings.js            从 joy.idl 重生成 coverage.md（幂等）
 *   node api/tools/gen_bindings.js --scan     从 joy.h 更新 joy.idl（保留人工字段）并重生成报告
 *   node api/tools/gen_bindings.js --check    只校验不写文件，供 CI 做「生成物无 diff」门禁
 *
 * 三段结构：parse（解析 joy.h）→ classify（分级 / 推断所有权）→ emit（IDL / 报告）。
 */
'use strict';

const fs = require('fs');
const path = require('path');

const HERE = __dirname;                              /* api/tools */
const JOY_REPO = path.resolve(HERE, '..', '..');    /* joy 仓库根 */
const JOY_API = path.join(JOY_REPO, 'api');
const JOY_H = path.join(JOY_REPO, 'include/joy/joy.h');
const JMATH_H = path.join(JOY_REPO, 'include/joy/jmath.h');
/* 第二个契约来源：jmath_wrap.js 生成的数学包装声明。主头只留一行 include，
 * 生成代码不再混进 joy.h；两者都是 --scan / --check 的输入。
 * api/ 目录约定：tools/ 人工生成器、idl/ 唯一真相源、generated/ 全部入库产物。 */
const JMATH_WRAPPERS_H = path.join(JOY_API, 'generated', 'jmath_wrappers.h');
const IDL = path.join(JOY_API, 'idl', 'joy.idl');
const REPORT = path.join(JOY_API, 'generated', 'coverage.md');
const THUNKS = path.join(JOY_API, 'generated', 'thunks_generated.c');
/* C++ 后端强签名 vtable（当前纵向切片试点），同样是入库生成物。 */
const VTABLE_H = path.join(JOY_API, 'generated', 'joy_api_vtable.h');
const VTABLE_C = path.join(JOY_API, 'generated', 'joy_api_vtable.c');
/* cs 后端薄适配也是入库生成物：由 joy.idl 的 status:on 条目驱动。 */
const CS_C = path.join(JOY_REPO, 'modules/cs/bindings/api/api_generated.c');
const CS_CS = path.join(JOY_REPO, 'modules/cs/scripts/Api.generated.cs');
const { buildThunks, canThunk, ownsIssue, THUNK_MAX_ARGS } = require('./emit_thunks');
const { buildVtable, canVtable } = require('./emit_vtable');
const { buildCsGlue, canEmitCs } = require('./emit_cs');
const { isOpaque, namespacedDomain, isNamespaced } = require('./naming');
/* jmath 包装生成器作为模块引入：--check 直接汇总它的错误，不再起子进程。 */
const { check: checkJmathWrappers } = require('./jmath_wrap');

/* 存量豁免名单：P0 冻结的 1156 条旧名字。命名门禁只检查不在这张表里的
 * 新增 API；全量迁移完成后删除该文件，届时所有名字都必须自带域。 */
const LEGACY_NAMES = path.join(JOY_API, 'idl', 'legacy_names.txt');
/* P1 分批改名：旧名 → 新名。--scan 时让新条目继承旧条目的 status/owns/note，
 * 且旧名不再变成 orphan，否则已开放的绑定会被静默关掉。每批填一次。 */
const RENAME_MAP = path.join(JOY_API, 'idl', 'rename_map.json');

/* 有意排除：条件编译下不存在、返回缓冲约定依赖 SDL、插件系统停用、测试符号。
 * SKIP_PREFIXES 管存量名字，SKIP_DOMAINS 管三段式新名字。 */
/* jspine 是有意排除（JOY_USE_SPINE 条件编译）；迁移后新名沿用同一排除，
 * 否则 19 条会从 X 掉回 L0/L1 混进 thunk 生成。 */
const SKIP_PREFIXES = ['joy_core_utils_', 'joy_r2d_spine_'];
/* 旧名 jecs_test 已随 P1 迁成 joy_scene_ecs_test，源码里不再存在，无需保留防护。 */
const SKIP_NAMES = new Set(['joy_scene_ecs_test']);
const SKIP_DOMAINS = new Set(['script']);
const DESTROY_SUFFIXES = ['destroy', 'free', 'release', 'delete'];

const SCALARS = new Set([
  'void', 'bool', 'char', 'float', 'double',
  'int', 'unsigned', 'unsigned int', 'long', 'unsigned long',
  'short', 'unsigned short',
  'int8_t', 'uint8_t', 'int16_t', 'uint16_t',
  'int32_t', 'uint32_t', 'int64_t', 'uint64_t',
  'size_t', 'ssize_t', 'intptr_t', 'uintptr_t',
  'joy_fp_t', 'joy_ft_t',
]);

/* ------------------------------------------------------------------ parse */

/* 块注释只清字符、保留换行，否则 collectDecls 记下的 joy.h 行号会偏移。 */
function stripComments(src) {
  return src
    .replace(/\/\*[\s\S]*?\*\//g, m => m.replace(/[^\n]/g, ''))
    .replace(/\/\/[^\n]*/g, '');
}

/* 值类型布局由 structs.js 统一解析：gen / thunks / cs 三个生成器共用，
   L2 试点还靠它把结构体参数展开成标量序列。 */
const { collectStructs, canExpand, expandParams } = require('./structs');

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

/* isOpaque 统一取自 naming.js，避免三个生成器对「句柄」的判定各说各话。 */

/* P1 前的旧名（形如 create_window）用第二段做域名；P1 后一律 joy_<域>_<动作>。 */
const GENERIC_FIRST = new Set([
  'create', 'destroy', 'find', 'get', 'set', 'new', 'free', 'init',
  'make', 'load', 'save', 'add', 'remove', 'is', 'has', 'to', 'from',
]);

function domainOf(name) {
  /* P0：三段式新名字取白名单域；存量名字仍走旧分词，结果一字不变。
   * 全量迁移完成后 GENERIC_FIRST 可退休（那时每个名字都自带域）。 */
  const ns = namespacedDomain(name);
  if (ns) return ns;
  const parts = name.split('_');
  if (parts.length > 1 && GENERIC_FIRST.has(parts[0])) return parts[1];
  return parts[0];
}

/* 返回句柄的 create → 找配对的 destroy/free/release/delete。
 * 两道保险，与 emit_thunks.js 的 ownsIssue() 硬校验保持一致：
 * 1. 只有名字里确实含 `_create` 才做精确替换；否则 replace 是恒等变换，
 *    get_* / find_* / wrap_* 会把 owns 填成自己。
 * 2. 候选 dtor 必须恰好一个参数、类型与 ret 严格相等；否则 byDomain 兜底
 *    会把 joy_r3d_destroy(joy_r3d_p) 套到返回 void* 的 GPU 对象上。 */
function inferOwns(name, ret, declByName) {
  if (!isOpaque(ret)) return null;
  const domain = domainOf(name);
  const valid = candidate => {
    const f = declByName.get(candidate);
    return !!f && !!f.params && f.params.length === 1 && f.params[0].ctype === ret;
  };
  for (const suffix of DESTROY_SUFFIXES) {
    if (/_create/.test(name)) {
      const exact = name.replace(/_create[\s\S]*$/, '_' + suffix);
      if (valid(exact)) return exact;
    }
    const byDomain = domain + '_' + suffix;
    if (valid(byDomain)) return byDomain;
  }
  return null;
}

/* 分级按风险排序：X(有意排除) L3(手写) L2(需静态断言) L1(句柄) L0(直接)。 */
function classify(name, ret, params, reason, structs) {
  const ns = namespacedDomain(name);
  if (SKIP_NAMES.has(name) || SKIP_PREFIXES.some(p => name.startsWith(p))
      || (ns && SKIP_DOMAINS.has(ns))) {
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

/* 契约来源按顺序列出。source 记真实文件名，IDL 里就能区分「joy.h 原生声明」
 * 与「工具生成的 jmath 派生包装」；顺序也决定同名时的优先级（当前无重名）。 */
const DECL_SOURCES = [
  { label: 'include/joy/defs.h', file: path.join(JOY_REPO, 'include/joy/defs.h') },
  { label: 'include/joy/api.h', file: path.join(JOY_REPO, 'include/joy/api.h') },
  { label: 'api/generated/jmath_wrappers.h', file: JMATH_WRAPPERS_H },
];

function scanJoyHeader() {
  const structs = collectStructs();
  const parsed = [];
  for (const s of DECL_SOURCES) {
    const src = fs.readFileSync(s.file, 'utf8');
    for (const d of collectDecls(src)) {
      const p = parseDecl(d.decl, d.line);
      if (p) parsed.push({ p, label: s.label });
    }
  }
  /* 用 Map 而不是名字集合：inferOwns 要核对候选 dtor 的参数类型。 */
  const declByName = new Map(parsed.map(x => [x.p.name, x.p]));
  const fresh = parsed.map(({ p, label }) => {
    const c = classify(p.name, p.ret, p.params, p.reason, structs);
    return {
      name: p.name,
      domain: domainOf(p.name),
      level: c.level,
      reason: c.reason,
      status: 'off',
      owns: inferOwns(p.name, p.ret, declByName),
      ret: p.ret,
      params: p.params,
      source: label + ':' + p.line,
      note: '',
    };
  });
  return { fresh, structs, declByName };
}

/* --scan 只覆盖自动字段；owns / status / note 是人工字段，永不覆盖。
   joy.h 里消失的条目保留并标 orphan，避免丢失人工标注。 */
function loadRenames() {
  if (!fs.existsSync(RENAME_MAP)) return new Map();
  return new Map(Object.entries(JSON.parse(fs.readFileSync(RENAME_MAP, 'utf8'))));
}

function mergeIdl(fresh) {
  let old = [];
  if (fs.existsSync(IDL)) old = JSON.parse(fs.readFileSync(IDL, 'utf8'));
  const renames = loadRenames();
  const renamedFrom = new Map([...renames].map(([from, to]) => [to, from]));
  const byName = new Map(old.map(e => [e.name, e]));
  const result = fresh.map(f => {
    /* 本批改名的新条目从旧名继承人工字段，并把 owns 里指向旧名的也换掉。 */
    /* rename_map 可以跨批累加：旧名条目已被上一批消费掉时回退到按本名匹配，
     * 否则第二次 --scan 会把已开放的 status 又清成 off。 */
    const o = byName.get(renamedFrom.get(f.name) || f.name) || byName.get(f.name);
    if (!o) return f;
    const owns = o.owns !== undefined ? o.owns : f.owns;
    return {
      ...f,
      owns: owns && renames.has(owns) ? renames.get(owns) : owns,
      /* borrowed 是人工字段：--scan 不许覆盖，随改名继承（未标记时为 undefined，
       * JSON.stringify 会省略，不产生整文件 diff）。 */
      borrowed: o.borrowed !== undefined ? o.borrowed : f.borrowed,
      status: o.status || 'off',
      note: o.note || '',
    };
  });
  const names = new Set(fresh.map(f => f.name));
  for (const o of old) {
    /* 已被改名消费的旧条目不再进 orphan。 */
    if (!names.has(o.name) && !renames.has(o.name)) result.push({ ...o, orphan: true });
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
  L2: '结构体按值 → P2-A 试点按字段展开成标量序列',
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
  const byName = new Map(entries.map(e => [e.name, e]));
  const on = entries.filter(e => e.status === 'on').length;
  const orphan = entries.filter(e => e.orphan);
  /* borrowed 是已裁决的借用句柄，不算「待人工确认 owns」。 */
  const needsOwns = entries.filter(e => e.level !== 'X' && isOpaque(e.ret)
    && !e.owns && !e.borrowed);
  const borrowedCount = entries.filter(e => e.borrowed).length;
  const ownsIssues = entries
    .filter(e => e.level !== 'X' && isOpaque(e.ret) && ownsIssue(e, byName))
    .map(e => ({ e, why: ownsIssue(e, byName) }));
  const l3 = entries.filter(e => e.level === 'L3');
  const excluded = entries.filter(e => e.level === 'X');
  const jm = jmathMeta();

  const L = [];
  L.push('# joy 绑定 API 覆盖率报告');
  L.push('');
  L.push('> 自动生成，勿手改：由 `api/tools/gen_bindings.js` 从 `include/joy/{defs,api}.h` + `api/idl/joy.idl` 产出。');
  L.push('> 生成物（共享 thunk 与 cs 薄适配）覆盖 `status: on` 的 L0/L1，');
  L.push('> 以及按字段展开成标量序列的 L2 条目；');
  L.push('> 注册表同时携带参数 / 返回类型，供 lua / js / cs 三端 vm.c 共用；');
  L.push('> `modules/*/bindings/legacy/` 旧手写胶水已删除，IDL 驱动层是唯一入口。');
  L.push('');
  L.push('## 总览');
  L.push('');
  L.push('| 项 | 数量 |');
  L.push('|---|---|');
  L.push('| `joy.h` 中 `JOY_API` 声明 | ' + meta.fresh.length + ' |');
  L.push('| `joy.h` 中值类型 `typedef struct` | ' + meta.structs.size + ' |');
  L.push('| IDL 条目 | ' + entries.length + ' |');
  L.push('| 已开放（`status: on`） | ' + on + ' |');
  const borrowedOpen = entries.filter(e => e.borrowed && e.status === 'on').length;
  L.push('| 借用句柄（`borrowed`，不注册析构） | ' + borrowedCount
    + '（已开放 ' + borrowedOpen + '） |');
  L.push('| 待人工确认 `owns` | ' + needsOwns.length + ' |');
  L.push('| 所有权待裁决（类型不匹配 / 缺 `owns`） | ' + ownsIssues.length + ' |');
  L.push('| orphan（`joy.h` 中已消失） | ' + orphan.length + ' |');
  L.push('');
  L.push('| 级别 | 含义 | 数量 |');
  L.push('|---|---|---|');
  for (const k of ['L0', 'L1', 'L2', 'L3', 'X']) {
    L.push('| ' + k + ' | ' + LEVEL_DESC[k] + ' | ' + counts[k] + ' |');
  }
  L.push('');
  L.push('开放由 `joy.idl` 的 `status` 控制。`--enable` 只放行通过类型、所有权与');
  L.push('缓冲校验的条目，被跳过的原因会在命令输出和下面各节列出；**开放 ≠ 业务已调用**。');
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
    L.push('| 函数 | 返回 | 位置 | 人工裁决（`note`） |');
    L.push('|---|---|---|---|');
    for (const e of needsOwns) {
      L.push('| `' + e.name + '` | `' + e.ret + '` | ' + e.source + ' | ' + (e.note || '—') + ' |');
    }
  }
  L.push('');
  L.push('## 待办：所有权待裁决（`owns` 不可信）');
  L.push('');
  L.push('返回句柄但没有可通过硬校验的 `owns`：借用指针、dtor 参数不匹配，');
  L.push('或压根没有合规的单参数 dtor。`note` 列是逐条人工核对后的所有权裁决。');
  L.push('批量开放会跳过它们；`--check` 也禁止把这类条目设成 `on`，');
  L.push('否则 dtor 会把错误类型的指针交给析构函数，运行时才崩。');
  L.push('');
  if (!ownsIssues.length) {
    L.push('（无）');
  } else {
    L.push('| 函数 | 返回 | `owns` | 问题 | 人工裁决（`note`） |');
    L.push('|---|---|---|---|---|');
    for (const x of ownsIssues) {
      L.push('| `' + x.e.name + '` | `' + x.e.ret + '` | '
        + (x.e.owns ? '`' + x.e.owns + '`' : '（无）') + ' | ' + x.why + ' | '
        + (x.e.note || '—') + ' |');
    }
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
  L.push('见 `api/README.md`。CI 用 `node api/tools/gen_bindings.js --check` 保证本文件与 IDL 同步。');
  L.push('');
  return L.join('\n');
}

function loadIdl() {
  if (!fs.existsSync(IDL)) {
    console.error('未找到 ' + IDL + '，先执行 node api/tools/gen_bindings.js --scan');
    process.exit(1);
  }
  return JSON.parse(fs.readFileSync(IDL, 'utf8'));
}

/* 校验：IDL 字段完整、owns 指向存在、on 不能是 L3/X、joy.h 与 IDL 同步、报告最新。 */
function check(entries) {
  const errors = [];
  const names = new Set();
  const meta = scanJoyHeader();
  for (const e of entries) {
    if (names.has(e.name)) errors.push('重复 name: ' + e.name);
    names.add(e.name);
    if (!e.level || !LEVEL_DESC[e.level]) errors.push('缺 level: ' + e.name);
    if (e.borrowed) {
      if (!isOpaque(e.ret)) errors.push('borrowed 只能用于返回句柄: ' + e.name);
      if (e.owns) errors.push('borrowed 与 owns 互斥: ' + e.name);
    }
    if (e.status === 'on' && e.level !== 'L0' && e.level !== 'L1'
        && !(e.level === 'L2' && canExpand(e, meta.structs))) {
      errors.push('只支持 L0/L1 及可展开的 L2 自动 thunk，不能 status:on: '
        + e.name + '(' + e.level + ')');
    }
    /* lapi.c 的 LUAX_MAX_ARGS 是运行期硬上限；手工把 IDL 改成 status:on
     * 也要在门禁里拦住，不能只靠 --enable 时的 canThunk()。 */
    if (e.status === 'on') {
      const argc = expandParams(e, meta.structs).length;
      if (argc > THUNK_MAX_ARGS) {
        errors.push('展开后参数 ' + argc + ' 个，超过 LUAX_MAX_ARGS='
          + THUNK_MAX_ARGS + '，不能 status:on: ' + e.name);
      }
    }
  }
  for (const e of entries) {
    if (e.owns && !meta.declByName.has(e.owns)) {
      errors.push('owns 指向 joy.h 中不存在的函数: ' + e.name + ' -> ' + e.owns);
    }
  }
  const byName = new Map(entries.map(e => [e.name, e]));
  for (const e of entries) {
    if (e.status !== 'on') continue;
    const issue = ownsIssue(e, byName);
    if (issue) errors.push('所有权不可信: ' + e.name + ' -> ' + issue);
  }
  const auto = e => JSON.stringify([e.name, e.domain, e.level, e.reason, e.ret, e.params, e.source]);
  for (const f of meta.fresh) {
    const e = entries.find(x => x.name === f.name);
    if (!e) { errors.push('joy.h 新增 API 未进 IDL，请跑 --scan: ' + f.name); continue; }
    if (auto(e) !== auto(f)) errors.push('joy.h 签名已变更，请跑 --scan: ' + f.name);
  }
  /* P0 命名门禁：只卡新增，不卡存量。存量 1156 条冻结在 legacy_names.txt；
   * 迁移产生的新名符合 joy_<域>_<动作> 即放行，旧名由 compat.h 兜住。
   * 全量迁移完成后删掉豁免文件，门禁自动变成全量强制。 */
  const legacy = new Set(
    fs.existsSync(LEGACY_NAMES)
      ? fs.readFileSync(LEGACY_NAMES, 'utf8').split(/\r?\n/).filter(Boolean)
      : []
  );
  for (const f of meta.fresh) {
    if (legacy.has(f.name)) continue;
    if (!isNamespaced(f.name)) {
      errors.push('新增 API 命名不合规（应形如 joy_<域>_<动作>，域见 naming.js DOMAINS）: '
        + f.name);
    }
  }
  /* Windows checkout（core.autocrlf）会把入库的 LF 转成 CRLF，比较前统一换行。 */
  const normEol = s => s.replace(/\r\n/g, '\n');
  const expected = buildReport(entries, meta);
  const onDisk = fs.existsSync(REPORT) ? fs.readFileSync(REPORT, 'utf8') : '';
  if (normEol(onDisk) !== normEol(expected)) {
    errors.push('coverage.md 已过期，请跑 node api/tools/gen_bindings.js');
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
      errors.push('thunks_generated.c 已过期，请跑 node api/tools/gen_bindings.js --emit');
    }
  }
  let expectedCs = null;
  try {
    expectedCs = buildCsGlue(entries);
  } catch (err) {
    errors.push('cs 适配生成失败: ' + err.message);
  }
  if (expectedCs !== null) {
    const cOnDisk = fs.existsSync(CS_C) ? fs.readFileSync(CS_C, 'utf8') : '';
    const csOnDisk = fs.existsSync(CS_CS) ? fs.readFileSync(CS_CS, 'utf8') : '';
    if (normEol(cOnDisk) !== normEol(expectedCs.c)) {
      errors.push('modules/cs/bindings/api/api_generated.c 已过期，请跑 node api/tools/gen_bindings.js --emit');
    }
    if (normEol(csOnDisk) !== normEol(expectedCs.cs)) {
      errors.push('modules/cs/scripts/Api.generated.cs 已过期，请跑 node api/tools/gen_bindings.js --emit');
    }
  }
  /* C++ 后端强签名 vtable：两个生成物同样纳入「无 diff」门禁；
   * 试点条目若被改坏（关掉 / 换成 L3 签名）会让表变空，这里一并拦住。 */
  if (!entries.some(canVtable)) {
    errors.push('vtable 试点条目不可生成，请检查 joy.idl 中该 API 的 status / 签名');
  }
  let expectedVtable = null;
  try {
    expectedVtable = buildVtable(entries);
  } catch (err) {
    errors.push('vtable 生成失败: ' + err.message);
  }
  if (expectedVtable !== null) {
    const hOnDisk = fs.existsSync(VTABLE_H) ? fs.readFileSync(VTABLE_H, 'utf8') : '';
    const cOnDisk = fs.existsSync(VTABLE_C) ? fs.readFileSync(VTABLE_C, 'utf8') : '';
    if (normEol(hOnDisk) !== normEol(expectedVtable.h)) {
      errors.push('joy_api_vtable.h 已过期，请跑 node api/tools/gen_bindings.js --emit');
    }
    if (normEol(cOnDisk) !== normEol(expectedVtable.c)) {
      errors.push('joy_api_vtable.c 已过期，请跑 node api/tools/gen_bindings.js --emit');
    }
  }
  /* jmath 包装头是契约输入，它的声明与实现也是生成物，一并纳入同一门禁。 */
  for (const e of checkJmathWrappers().errors) errors.push('jmath_wrap: ' + e);
  return errors;
}

/* ------------------------------------------------ 批量开放 / 关闭（P2） */

function matches(e, spec) {
  if (spec === 'all') return e.level === 'L0' || e.level === 'L1';
  if (spec === 'L0' || spec === 'L1') return e.level === spec;
  if (spec.startsWith('domain:')) return e.domain === spec.slice(7);
  if (spec.startsWith('name:')) return e.name === spec.slice(5);
  throw new Error('未知选择子：' + spec + '（可用 L0 / L1 / all / domain:<名> / name:<API>）');
}

/* 批量开放只放行生成器确定能安全处理的条目。
 * 返回句柄的还要求人工确认所有权：自动推断的 owns 即使类型匹配，也可能是
 * 借用指针（get_* / wrap_*），批量模式一律跳过，只能显式 name: 点名开放。 */
function canOpen(e, byName, explicit) {
  if (e.orphan) return 'orphan';
  const t = canThunk(e, byName);
  if (t) return t;
  const c = canEmitCs(e);
  if (c) return c;
  if (e.params.some(p => p.ctype === 'void*')) return 'void* 缓冲参数需手写适配';
  if (isOpaque(e.ret) && !explicit) return '返回句柄需人工确认所有权（用 name: 单独开放）';
  return null;
}

function applySpecs(entries, args, enable) {
  const flag = enable ? '--enable' : '--disable';
  const specs = args.slice(args.indexOf(flag) + 1).filter(a => !a.startsWith('--'));
  if (!specs.length) throw new Error('用法：' + flag + ' L0|L1|all|domain:<名>|name:<API>');
  for (const s of specs) matches({ level: '', domain: '', name: '' }, s);
  const byName = new Map(entries.map(e => [e.name, e]));
  const changed = [];
  const skipped = [];
  for (const e of entries) {
    if (!specs.some(s => matches(e, s))) continue;
    const want = enable ? 'on' : 'off';
    if (e.status === want) continue;
    if (enable) {
      const why = canOpen(e, byName, specs.includes('name:' + e.name));
      if (why) { skipped.push(e.name + '(' + why + ')'); continue; }
    }
    e.status = want;
    changed.push(e.name);
  }
  return { changed, skipped };
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
      console.log('check OK: ' + loadIdl().length
        + ' IDL entries, report + thunks + vtable + cs glue + jmath wrappers up to date');
      return 0;
    }
    const hasEnable = args.includes('--enable');
    const hasDisable = args.includes('--disable');
    if (hasEnable && hasDisable) throw new Error('--enable 与 --disable 不能同时使用');
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
    if (hasEnable || hasDisable) {
      const r = applySpecs(entries, args, hasEnable);
      fs.writeFileSync(IDL, idlText(entries), 'utf8');
      console.log((hasEnable ? 'enable' : 'disable') + ': '
        + r.changed.length + ' changed, ' + r.skipped.length + ' skipped');
      for (const s of r.skipped.slice(0, 12)) console.log('  skip ' + s);
      if (r.skipped.length > 12) console.log('  ... 共 ' + r.skipped.length + ' 项被跳过');
    }
    const glue = buildCsGlue(entries);
    const vtable = buildVtable(entries);
    fs.writeFileSync(REPORT, buildReport(entries, meta), 'utf8');
    fs.writeFileSync(THUNKS, buildThunks(entries), 'utf8');
    fs.writeFileSync(VTABLE_H, vtable.h, 'utf8');
    fs.writeFileSync(VTABLE_C, vtable.c, 'utf8');
    fs.writeFileSync(CS_C, glue.c, 'utf8');
    fs.writeFileSync(CS_CS, glue.cs, 'utf8');
    console.log('emit: open=' + entries.filter(e => e.status === 'on').length
      + ', vtable=' + entries.filter(canVtable).length
      + ', report + thunks + vtable + cs glue written');
    return 0;
  } catch (err) {
    console.error('gen_bindings: ' + err.message);
    return 1;
  }
}

process.exit(main());
