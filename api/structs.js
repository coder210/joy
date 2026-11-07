'use strict';
/*
 * P2-A 试点：值类型结构体布局解析 + 「按字段展开成标量参数」。
 *
 * gen_bindings.js / emit_thunks.js / emit_cs.js 共用这一份，避免三个生成器
 * 各自解析 joy.h 的结构体。当前只放行「字段全是已知标量」的 POD 结构体；
 * 数组 / 嵌套 / 指针 / 未知类型一律不展开，保守地留给手写 thunk。
 */
const fs = require('fs');
const path = require('path');

const JOY_DIR = path.resolve(__dirname, '..');

function stripComments(src) {
  return src
    .replace(/\/\*[\s\S]*?\*\//g, m => m.replace(/[^\n]/g, ''))
    .replace(/\/\/[^\n]*/g, '');
}

function normType(t) {
  return t.replace(/\bconst\b/g, ' ')
    .replace(/\s+/g, ' ')
    .trim()
    .replace(/\s*\*\s*/g, '*');
}

/* emit_thunks.SCALAR 与 emit_cs.MAP 的交集：两边都能翻译的标量。 */
const SCALARS = new Set([
  'bool', 'int', 'int32_t', 'uint8_t', 'uint16_t', 'uint32_t', 'unsigned int',
  'int64_t', 'uint64_t', 'size_t', 'unsigned long', 'float', 'double',
]);

/* Map<name, fields|null>。null = 布局不可展开，但名字仍参与分级判定。 */
function collectStructs() {
  const out = new Map();
  for (const file of [path.join(JOY_DIR, 'joy.h'), path.join(JOY_DIR, 'jmath.h')]) {
    if (!fs.existsSync(file)) continue;
    const src = stripComments(fs.readFileSync(file, 'utf8'));
    const re = /typedef\s+struct\s*\w*\s*\{([^}]*)\}\s*([A-Za-z_]\w*)\s*;/g;
    let m;
    while ((m = re.exec(src)) !== null) {
      if (!m[2].endsWith('_t')) continue;
      const fields = [];
      let ok = true;
      for (const raw of m[1].split(';')) {
        const line = raw.trim();
        if (!line) continue;
        const fm = line.match(/^(.*?)([A-Za-z_]\w*)\s*$/);
        if (!fm || /[\[\]*()]/.test(fm[1])) { ok = false; break; }
        const ctype = normType(fm[1]);
        if (!SCALARS.has(ctype)) { ok = false; break; }
        fields.push({ name: fm[2], ctype });
      }
      out.set(m[2], ok && fields.length ? fields : null);
    }
  }
  return out;
}

/* 只有 L2 且每个结构体参数都可展开时才走试点路径。 */
function canExpand(e, layouts) {
  if (e.level !== 'L2' || !e.params) return false;
  let found = false;
  for (const p of e.params) {
    if (!layouts.has(p.ctype)) continue;
    const f = layouts.get(p.ctype);
    if (!f || !f.length) return false;
    found = true;
  }
  return found;
}

/* 结构体参数 → 若干字段标量；其它参数原样保留（保持 L0/L1 生成物不变）。 */
function expandParams(e, layouts) {
  const out = [];
  for (const p of e.params || []) {
    const f = layouts.has(p.ctype) ? layouts.get(p.ctype) : null;
    if (f) {
      for (const fld of f) {
        out.push({ name: p.name + '_' + fld.name, ctype: fld.ctype, isConst: false,
          field: fld.name, base: p.name });
      }
    } else {
      out.push({ name: p.name, ctype: p.ctype, isConst: p.isConst });
    }
  }
  return out;
}

module.exports = { collectStructs, canExpand, expandParams, SCALARS };
