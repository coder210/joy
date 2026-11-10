/* ============================================================
 * jmath.h → 脚本绑定包装层生成器
 *
 * jmath.h 的 134 个 JOY_INLINE 函数是 static inline 的头文件内定义，没有外部
 * 链接符号；绑定生成器只识别 JOY_API 声明。本脚本解析 jmath.h 的函数签名与
 * 值类型布局，自动生成两个产物（都在 api/generated/，勿手改）：
 *
 *   1. jmath_wrappers.h  全标量签名的 JOY_API 声明，joy.h 只留一行 include
 *   2. jmath_wrappers.c  外部链接实现，内部调用 jmath.h 的 inline 函数
 *
 * gen_bindings.js 把 jmath_wrappers.h 当作第二个契约输入源，--check 直接调用
 * 本模块导出的 check()，不再起子进程。生成头不是独立公共头：它由 joy.h 在
 * extern "C" 块内引入，JOY_API / 基础类型都由 joy.h 提供。
 *
 * 用法（任意目录）：node api/tools/jmath_wrap.js [--check]
 * ============================================================ */
'use strict';

const fs = require('fs');
const path = require('path');

const ROOT = path.resolve(__dirname, '..', '..');
const JMATH = path.join(ROOT, 'include', 'joy', 'jmath.h');
const JOYH = path.join(ROOT, 'include', 'joy', 'joy.h');
const GEN = path.join(ROOT, 'api', 'generated');
const OUTH = path.join(GEN, 'jmath_wrappers.h');
const OUTC = path.join(GEN, 'jmath_wrappers.c');
const INCLUDE_LINE = '        #include "../../api/generated/jmath_wrappers.h"';
const LEGACY_BEGIN = '/* ==== BEGIN GENERATED jmath wrappers';
const LEGACY_END = '/* ==== END GENERATED jmath wrappers ==== */';

const PATHS = {
  root: ROOT, jmath: JMATH, joyh: JOYH, gen: GEN,
  outH: OUTH, outC: OUTC, includeLine: INCLUDE_LINE,
};

/* 只映射到 emit_thunks.js / emit_cs.js 都认识的 C 类型。 */
const SCALARS = {
  'fp_t': 'int64_t',
  'ft_t': 'float',
  'float': 'float',
  'double': 'double',
  'int': 'int32_t',
  'bool': 'bool',
  'int32_t': 'int32_t',
  'uint32_t': 'uint32_t',
  'int64_t': 'int64_t',
  'uint64_t': 'uint64_t',
  'size_t': 'uint64_t',
};

const MAX_ARGS = 16;      /* LUAX_MAX_ARGS / JSAPI_MAX_ARGS */
const MAX_FIELDS = 4;     /* 结构体返回最多拆成 4 个分量函数 */

function stripComments(src) {
  return src
    .replace(/\/\*[\s\S]*?\*\//g, m => m.replace(/[^\n]/g, ''))
    .replace(/\/\/[^\n]*/g, '');
}

function norm(src) { return src.replace(/\r\n/g, '\n'); }

/* ---------------------------------------------------------- struct layout */
function parseStructs(src) {
  const raw = new Map();
  const re = /typedef\s+struct\s*\w*\s*\{([\s\S]*?)\}\s*([A-Za-z_]\w*)\s*(?:,\s*\*\s*[A-Za-z_]\w*)?\s*;/g;
  let m;
  while ((m = re.exec(src)) !== null) {
    const fields = [];
    for (const seg of m[1].split(';')) {
      const s = seg.replace(/\s+/g, ' ').trim();
      if (!s) continue;
      const toks = s.split(' ');
      const type = toks[0];
      const names = toks.slice(1).join('').split(',').filter(Boolean);
      for (const n of names) fields.push({ type, name: n });
    }
    if (fields.length) raw.set(m[2], fields);
  }
  return raw;
}

/* 展平成 [{name, expr, flat, ctype}]，expr 是 C 访问表达式（可能带 .）。 */
function flatten(type, prefix, raw, depth) {
  if (depth > 3) return null;
  const fields = raw.get(type);
  if (!fields) return null;
  const out = [];
  for (const f of fields) {
    const expr = prefix ? prefix + '.' + f.name : f.name;
    if (SCALARS[f.type]) {
      out.push({ name: f.name, expr, flat: expr.replace(/\./g, '_'), ctype: SCALARS[f.type] });
    } else {
      const sub = flatten(f.type, expr, raw, depth + 1);
      if (!sub) return null;
      for (const s of sub) out.push(s);
    }
  }
  return out;
}

/* --------------------------------------------------------- function parse */
function parseFunctions(src) {
  const lines = stripComments(src).split(/\r?\n/);
  const out = [];
  for (let i = 0; i < lines.length; i++) {
    const m = lines[i].match(/\bJOY_INLINE\b([\s\S]*)$/);
    if (!m) continue;
    let sig = m[1];
    let j = i;
    while (sig.indexOf('{') < 0 && j + 1 < lines.length) {
      j++;
      sig += ' ' + lines[j];
    }
    const brace = sig.indexOf('{');
    if (brace < 0) continue;
    sig = sig.slice(0, brace).replace(/\s+/g, ' ').trim();
    const sm = sig.match(/^(.+?)\s+([A-Za-z_]\w*)\s*\((.*)\)$/);
    if (!sm) continue;
    out.push({ ret: sm[1].trim(), name: sm[2], paramsRaw: sm[3].trim(), line: i + 1 });
    i = j;
  }
  return out;
}

function parseParams(raw) {
  if (!raw || raw === 'void') return [];
  const out = [];
  for (const seg of raw.split(',')) {
    const s = seg.trim().replace(/\s+/g, ' ');
    if (!s || /[*\[]/.test(s)) return null;
    const pm = s.match(/^(.+?)\s+([A-Za-z_]\w*)$/);
    if (!pm) return null;
    out.push({ type: pm[1].trim(), name: pm[2] });
  }
  return out;
}

/* ------------------------------------------------------------- generation */
function makeWrapper(spec) {
  const L = [];
  const ps = spec.params.map(p => p.ctype + ' ' + p.name).join(', ') || 'void';
  L.push(spec.ret + ' ' + spec.cname + '(' + ps + ')');
  L.push('{');
  for (const d of spec.decls) {
    L.push('        ' + d.ctype + ' ' + d.varName + ';');
    for (const f of d.fields) {
      L.push('        ' + d.varName + '.' + f.expr + ' = ' + d.varName + '_' + f.flat + ';');
    }
  }
  if (spec.retStruct) L.push('        ' + spec.retStruct + ' _r;');
  const call = spec.orig + '(' + spec.callArgs.join(', ') + ')';
  if (spec.ret === 'void') {
    L.push('        ' + call + ';');
  } else if (spec.fieldExpr) {
    L.push('        _r = ' + call + ';');
    L.push('        return (' + spec.ret + ')_r.' + spec.fieldExpr + ';');
  } else {
    L.push('        return (' + spec.ret + ')' + call + ';');
  }
  L.push('}');
  return L.join('\n');
}

function makePacked(spec) {
  const L = [];
  const ps = spec.params.map(p => p.ctype + ' ' + p.name).join(', ') || 'void';
  L.push('uint64_t ' + spec.cname + '(' + ps + ')');
  L.push('{');
  for (const d of spec.decls) {
    L.push('        ' + d.ctype + ' ' + d.varName + ';');
    for (const f of d.fields) {
      L.push('        ' + d.varName + '.' + f.expr + ' = ' + d.varName + '_' + f.flat + ';');
    }
  }
  L.push('        vec2_t _r;');
  L.push('        float _v0, _v1;');
  L.push('        uint32_t _b0, _b1;');
  L.push('        _r = ' + spec.orig + '(' + spec.callArgs.join(', ') + ');');
  L.push('        _v0 = (float)_r.' + spec.fields[0].expr + ';');
  L.push('        _v1 = (float)_r.' + spec.fields[1].expr + ';');
  L.push('        memcpy(&_b0, &_v0, sizeof(_b0));');
  L.push('        memcpy(&_b1, &_v1, sizeof(_b1));');
  L.push('        return (uint64_t)_b0 | ((uint64_t)_b1 << 32);');
  L.push('}');
  return L.join('\n');
}

function generate(src) {
  const raw = parseStructs(src);
  const fns = parseFunctions(src);
  const wrappers = [];
  const seen = new Set();
  const skip = [];

  const add = w => {
    if (seen.has(w.cname)) { skip.push(w.cname + ': duplicate'); return; }
    seen.add(w.cname);
    wrappers.push(w);
  };

  for (const f of fns) {
    const params = parseParams(f.paramsRaw);
    if (!params) { skip.push(f.name + ': unsupported params'); continue; }

    const flat = [];
    const decls = [];
    const callArgs = [];
    let bad = false;
    for (const p of params) {
      if (SCALARS[p.type]) {
        flat.push({ ctype: SCALARS[p.type], name: p.name });
      } else {
        const fields = flatten(p.type, '', raw, 0);
        if (!fields) { bad = true; break; }
        decls.push({ ctype: p.type, varName: p.name, fields });
        for (const fld of fields) {
          flat.push({ ctype: fld.ctype, name: p.name + '_' + fld.flat });
        }
      }
      callArgs.push(p.name);
    }
    if (bad) { skip.push(f.name + ': unknown struct layout'); continue; }
    if (flat.length > MAX_ARGS) { skip.push(f.name + ': ' + flat.length + ' args > ' + MAX_ARGS); continue; }

    const base = { params: flat, decls, callArgs, orig: f.name, fieldExpr: null, retStruct: null };
    if (f.ret === 'void' || SCALARS[f.ret]) {
      add(Object.assign({}, base, {
        cname: 'joy_math_' + f.name,
        ret: f.ret === 'void' ? 'void' : SCALARS[f.ret],
      }));
      continue;
    }
    const retFields = flatten(f.ret, '', raw, 0);
    if (!retFields || retFields.length > MAX_FIELDS) {
      skip.push(f.name + ': struct return ('
        + (retFields ? retFields.length + ' fields > ' + MAX_FIELDS : 'layout not parsed')
        + ')');
      continue;
    }
    for (const fld of retFields) {
      add(Object.assign({}, base, {
        cname: 'joy_math_' + f.name + '_' + fld.flat,
        ret: fld.ctype,
        retStruct: f.ret,
        fieldExpr: fld.expr,
      }));
    }
    /* vec2_t 的两个 float 正好能塞进 uint64_t，给一个一次调用拿到结果版本。 */
    if (f.ret === 'vec2_t' && retFields.length === 2) {
      add(Object.assign({}, base, {
        cname: 'joy_math_' + f.name,
        ret: 'uint64_t',
        packed: true,
        fields: retFields,
      }));
    }
  }

  const declLines = wrappers.map(w =>
    'JOY_API ' + w.ret + ' ' + w.cname + '('
    + (w.params.map(p => p.ctype + ' ' + p.name).join(', ') || 'void') + ');');

  const hLines = [
    '/* 自动生成，勿手改：由 api/tools/jmath_wrap.js 从 jmath.h 产出。',
    ' * jmath.h 的数学函数是 static inline，没有外部链接符号；这里给出一层全标量',
    ' * 签名的 JOY_API 声明，让 IDL / thunk / 三端包装能从契约里看到它们。',
    ' * 实现见同目录 jmath_wrappers.c。本头由 joy.h 在 extern "C" 块内引入，',
    ' * JOY_API 与基础类型由 joy.h 提供；改 jmath.h 后重跑 node api/tools/jmath_wrap.js。 */',
    '#ifndef JOY_GENERATED_JMATH_WRAPPERS_H',
    '#define JOY_GENERATED_JMATH_WRAPPERS_H',
    '',
    '#ifdef __cplusplus',
    'extern "C" {',
    '#endif',
    '',
    ...declLines,
    '',
    '#ifdef __cplusplus',
    '}',
    '#endif',
    '',
    '#endif /* JOY_GENERATED_JMATH_WRAPPERS_H */',
    '',
  ];

  const cLines = [
    '/* 自动生成，勿手改：由 api/tools/jmath_wrap.js 从 jmath.h 产出。',
    ' * jmath.h 的数学函数是 static inline，这里给它们一层外部链接的全标量包装，',
    ' * 让 jmath_wrappers.h 里的 JOY_API 声明能进 IDL、生成三端 thunk。',
    ' * 改 jmath.h 后重跑 node api/tools/jmath_wrap.js。 */',
    '#include "joy.h"',
    '#include <string.h>',
    '',
  ];
  for (const w of wrappers) {
    cLines.push(w.packed ? makePacked(w) : makeWrapper(w));
    cLines.push('');
  }

  return {
    declLines,
    hText: norm(hLines.join('\n')),
    cText: norm(cLines.join('\n')),
    wrappers, fns, skip,
  };
}

/* -------------------------------------------------------------- emit/check */

function build() {
  return generate(fs.readFileSync(JMATH, 'utf8'));
}

function emit() {
  fs.mkdirSync(GEN, { recursive: true });
  const out = build();
  fs.writeFileSync(OUTH, out.hText, 'utf8');
  fs.writeFileSync(OUTC, out.cText, 'utf8');
  return out;
}

/* 返回错误数组而不退出，供 gen_bindings.js --check 汇总成单一门禁。 */
function check() {
  const errors = [];
  const out = build();
  const curH = fs.existsSync(OUTH) ? fs.readFileSync(OUTH, 'utf8') : '';
  const curC = fs.existsSync(OUTC) ? fs.readFileSync(OUTC, 'utf8') : '';
  if (norm(curH) !== out.hText) {
    errors.push('api/generated/jmath_wrappers.h 已过期，请跑 node api/tools/jmath_wrap.js');
  }
  if (norm(curC) !== out.cText) {
    errors.push('api/generated/jmath_wrappers.c 已过期，请跑 node api/tools/jmath_wrap.js');
  }
  const joy = fs.readFileSync(JOYH, 'utf8');
  if (!joy.includes(INCLUDE_LINE)) {
    errors.push('joy.h 缺少生成头 include: ' + INCLUDE_LINE.trim());
  }
  if (joy.includes(LEGACY_BEGIN) || joy.includes(LEGACY_END)) {
    errors.push('joy.h 仍残留旧的 BEGIN/END 生成块');
  }
  return { errors, wrappers: out.wrappers.length };
}

module.exports = { PATHS, generate, build, emit, check };

/* --------------------------------------------------------------------- CLI */
if (require.main === module) {
  const args = process.argv.slice(2);
  if (args.includes('--check')) {
    const r = check();
    if (r.errors.length) {
      for (const e of r.errors) console.error('  - ' + e);
      console.error('jmath_wrap check FAILED (' + r.errors.length + ')');
      process.exit(1);
    }
    console.log('jmath_wrap check OK: ' + r.wrappers + ' wrappers');
  } else {
    const out = emit();
    console.log('jmath_wrap: ' + out.fns.length + ' JOY_INLINE, '
      + out.wrappers.length + ' wrappers generated, ' + out.skip.length + ' skipped');
    if (out.skip.length) {
      if (process.env.JMATH_WRAP_VERBOSE) console.log(out.skip.join('\n'));
      const byReason = {};
      for (const s of out.skip) {
        const k = s.replace(/^[^:]+: /, '');
        byReason[k] = (byReason[k] || 0) + 1;
      }
      for (const k of Object.keys(byReason).sort((x, y) => byReason[y] - byReason[x]).slice(0, 8)) {
        console.log('  skip ' + byReason[k] + 'x ' + k);
      }
    }
  }
}
