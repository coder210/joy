#!/usr/bin/env node
/**
 * 域纯度审计：一个 .c 只能承载一个域的公开接口。
 *
 * 判定口径（刻意从严，避免误报）：
 *   - 只算「函数定义」：`joy_<域>_<动作>` 后紧跟 '('，配对 ')' 之后跳过空白是 '{'；
 *   - 不算调用（`joy_core_get_raw_window(...)` 出现在 r2d 里是正常单向依赖）；
 *   - 不算声明（以 ';' 结尾）；
 *   - 不算类型名（`joy_image_p` / `joy_animation_t` 是 jdefs.h 的共享句柄）；
 *   - 不算注释里的提及。
 *
 * 用法：
 *   node api/tools/audit_domain_purity.js
 * 退出码：0 = 全部纯域；1 = 存在混域文件（可直接用于 CI 门禁）。
 */
const fs = require('fs');
const path = require('path');

const ROOT = path.resolve(__dirname, '../..');
const DOMAINS = ['core', 'math', 'r2d', 'r3d', 'ui', 'scene', 'net', 'audio', 'input', 'script'];

/* 去注释、去行注释，保留换行以维持行号 */
function strip(s) {
  return s.replace(/\/\*[\s\S]*?\*\//g, m => m.replace(/[^\n]/g, ''))
    .replace(/\/\/[^\n]*/g, m => m.replace(/[^\n]/g, ''));
}

function findDefs(src) {
  const out = [];
  for (const m of src.matchAll(/\bjoy_([a-z0-9]+)_([a-z0-9_]+)\b/g)) {
    const name = m[0];
    let i = m.index + name.length;
    while (i < src.length && /\s/.test(src[i])) i++;
    if (src[i] !== '(') continue;
    /* 配对括号 */
    let depth = 0, j = i;
    for (; j < src.length; j++) {
      if (src[j] === '(') depth++;
      else if (src[j] === ')') { depth--; if (depth === 0) { j++; break; } }
    }
    while (j < src.length && /\s/.test(src[j])) j++;
    if (src[j] !== '{') continue;   /* ';' → 声明/调用，跳过 */
    /* 排除 if / while / for / return 里的调用 */
    const before = src.slice(Math.max(0, m.index - 160), m.index);
    if (/(\breturn|\bif|\bfor|\bwhile|\bswitch|&&|\|\||=)\s*$/.test(before)) continue;
    out.push({ name, domain: m[1], line: src.slice(0, m.index).split('\n').length });
  }
  return out;
}

const srcDir = path.join(ROOT, 'src');
if (!fs.existsSync(srcDir)) {
  console.error('找不到 src/ 目录，请在 joy 仓库根目录运行');
  process.exit(2);
}

let mixed = 0, files = 0, defs = 0;
const rows = [];
for (const f of fs.readdirSync(srcDir).filter(x => x.endsWith('.c')).sort()) {
  const list = findDefs(strip(fs.readFileSync(path.join(srcDir, f), 'utf8')));
  if (!list.length) continue;
  files++; defs += list.length;
  const g = {};
  list.forEach(d => (g[d.domain] = (g[d.domain] || 0) + 1));
  const parts = Object.entries(g).sort((a, b) => b[1] - a[1]);
  const bad = parts.some(([d]) => !DOMAINS.includes(d));
  if (parts.length > 1) mixed++;
  rows.push({ f, total: list.length, parts, defs: list, bad });
}

console.log('域纯度审计：一个 .c 只能承载一个域的函数定义');
console.log('='.repeat(66));
for (const r of rows) {
  const tag = r.parts.length > 1 ? '混域' : (r.bad ? '非白名单域' : '  纯域');
  console.log(tag.padEnd(6) + '  ' + r.f.padEnd(18) + String(r.total).padStart(4) + '  ' +
    r.parts.map(([d, n]) => d + ':' + n).join(' '));
}
console.log('='.repeat(66));
console.log(`含函数定义的 .c ${files} 个 | 函数定义 ${defs} 条 | 混域 ${mixed} 个`);

if (mixed) {
  for (const r of rows.filter(x => x.parts.length > 1)) {
    const main = r.parts[0][0];
    console.log(`\n${r.f}（主域 ${main}）的外来函数：`);
    r.defs.filter(d => d.domain !== main)
      .forEach(d => console.log(`  L${String(d.line).padStart(5)}  [${d.domain}] ${d.name}`));
  }
  console.error('\nFAIL：存在混域文件，一个 .c 只能承载一个域的函数定义。');
  process.exit(1);
}
console.log('\nOK：所有 .c 均只承载一个域的函数定义。');
