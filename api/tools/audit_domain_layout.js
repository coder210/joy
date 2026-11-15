#!/usr/bin/env node
/**
 * 域布局门禁：一个 .c 一定是一个域；一个域不允许多个 .c；每个 .c 要有一个头文件。
 *
 * 具体规则（全部作用于 src/ 下的人工实现文件）：
 *   R1 文件名必须是 j<域>.c（如 jaudio.c），域须在白名单 DOMAINS 内；
 *      文件名用短前缀 j<域>，公开 API 名仍用长前缀 joy_<域>_*，两者域段相同。
 *   R2 每个域恰好一个 .c；GENERATED_ONLY_DOMAINS 中的域由 api/generated/ 的
 *      生成物实现，豁免「必须有 src/j<域>.c」，其余域与 .c 集合严格相等；
 *   R3 每个 .c 必须有同名 .h；该 .h 须有 include guard，且 .c 必须 include 它；
 *   R4 .c 内所有 joy_* 函数定义的域前缀必须与文件名域一致（纯域）。
 *      只算函数定义，不算跨域调用、注释、声明与共享句柄类型。
 *   R5 引用方向：域配套头是该域声明的归属地，不得反向 include 聚合门面 joy.h，
 *      须自带 <joy/jtypes.h> 与 extern "C"；include/joy/japi.h 必须逐个
 *      include 全部手写域头，且自身不得残留 JOY_API 声明；生成物域的声明由
 *      joy.h 聚合 api/generated/jmath_wrappers.h，不占 src/j<域>.h。
 *   R6 数据分片：DATA_SHARDS 里显式登记的文件不是域实现文件，不参与 R1/R2，
 *      但必须配同名 .h、include 它、不反向 include joy.h，且不得含 joy_* 函数定义。
 *   R7 域头声明归位：src/j<域>.h 里的函数声明，带 joy_<域2>_ 前缀的域段必须
 *      等于文件域；不带前缀的裸名，其定义所在 .c 的域也必须等于文件域。
 *      只管声明（R4 只管定义），防止行为树之类整块声明被错放进别的域头。
 *      带 JOY_API 与否都检查：生成链不收的声明同样不能放错位置。
 *
 * 范围：api/generated/ 生成物、external/ 第三方、examples/、modules/、SDL/ 等
 * 不属于「joy 手写域实现」，不参与本规则，仍由各自清单管理。
 *
 * 用法： node api/tools/audit_domain_layout.js
 * 退出码：0 = 全部通过；1 = 存在违规（可直接用于 CI 门禁）。
 */
const fs = require('fs');
const path = require('path');

const ROOT = path.resolve(__dirname, '../..');
const SRC = path.join(ROOT, 'src');
const DOMAINS = ['core', 'collision', 'r2d', 'r3d', 'scene', 'net', 'sys', 'script', 'audio'];
/* 仅由工具生成物实现的域：没有 src/j<域>.c，实现落在 api/generated/。
 * math 域的 220 条 API 由 jmath_wrap.js 从 include/joy/jmath.h 生成，声明在
 * api/generated/jmath_wrappers.h（由 joy.h 聚合）、实现在 jmath_wrappers.c。
 * 它仍是合法域名（naming.js 的 DOMAINS 保留 math，IDL domain 判定要用），
 * 但豁免 R2/R5，且必须核对生成物确实存在、确实只承载该域的函数定义。
 * 碰撞/GJK/box2d 等几何求交已拆到 collision 域，见 src/jcollision.c。 */
const GENERATED_ONLY_DOMAINS = ['math'];
const GENERATED_IMPL = { math: 'api/generated/jmath_wrappers.c' };
/* 数据分片白名单：只放纯数据、没有任何 joy_* 函数定义的文件。
 * 新增数据分片必须同时登记在 cmake/JoySources.cmake 的对应源码组里。 */
const DATA_SHARDS = ['jlut.c'];

function strip(s) {
  return s.replace(/\/\*[\s\S]*?\*\//g, m => m.replace(/[^\n]/g, ''))
    .replace(/\/\/[^\n]*/g, m => m.replace(/[^\n]/g, ''));
}

/* 与 audit_domain_purity.js 同口径：只认「函数定义」 */
function findDefs(src) {
  const out = [];
  for (const m of src.matchAll(/\bjoy_([a-z0-9]+)_([a-z0-9_]+)\b/g)) {
    const name = m[0];
    let i = m.index + name.length;
    while (i < src.length && /\s/.test(src[i])) i++;
    if (src[i] !== '(') continue;
    let depth = 0, j = i;
    for (; j < src.length; j++) {
      if (src[j] === '(') depth++;
      else if (src[j] === ')') { depth--; if (depth === 0) { j++; break; } }
    }
    while (j < src.length && /\s/.test(src[j])) j++;
    if (src[j] !== '{') continue;
    const before = src.slice(Math.max(0, m.index - 160), m.index);
    if (/(\breturn|\bif|\bfor|\bwhile|\bswitch|&&|\|\||=)\s*$/.test(before)) continue;
    out.push({ name, domain: m[1], line: src.slice(0, m.index).split('\n').length });
  }
  return out;
}

if (!fs.existsSync(SRC)) {
  console.error('找不到 src/ 目录，请在 joy 仓库根目录运行');
  process.exit(2);
}

const cFiles = fs.readdirSync(SRC).filter(x => x.endsWith('.c')).sort();
const hFiles = new Set(fs.readdirSync(SRC).filter(x => x.endsWith('.h')));
const errors = [];
const rows = [];

/* R5：聚合头只做 include，声明全部在域配套头里。 */
const JAPI = path.join(ROOT, 'include/joy/japi.h');
const japiRaw = fs.existsSync(JAPI) ? fs.readFileSync(JAPI, 'utf8') : '';
if (!japiRaw) errors.push('R5 找不到 include/joy/japi.h');
if (/\bJOY_API\s+[\w*]/.test(japiRaw)) {
  errors.push('R5 include/joy/japi.h 仍含 JOY_API 声明，应只做 include 聚合');
}

/* R2：集合严格相等 */
const got = new Set();
for (const f of cFiles) {
  /* R6：数据分片不算域实现文件，但同样要配头、include 头、保持纯数据。 */
  if (DATA_SHARDS.includes(f)) {
    const h = f.replace(/\.c$/, '.h');
    const raw = fs.readFileSync(path.join(SRC, f), 'utf8');
    const defs = findDefs(strip(raw));
    if (defs.length) errors.push(`R6 ${f}：数据分片不得含 joy_* 函数定义（发现 ${defs.length} 个）`);
    if (!hFiles.has(h)) errors.push(`R6 ${f}：缺少配套头 src/${h}`);
    else {
      const hRaw = fs.readFileSync(path.join(SRC, h), 'utf8');
      if (!/^[ \t]*#[ \t]*ifndef\s+\w+/m.test(hRaw)) {
        errors.push(`R6 src/${h}：缺少 include guard`);
      }
      if (/^[ \t]*#[ \t]*include[ \t]+["<]joy\.h[">]/m.test(hRaw)) {
        errors.push(`R6 src/${h}：不得反向 #include "joy.h"`);
      }
    }
    const incRe = new RegExp('^[ \\t]*#[ \\t]*include[ \\t]+["<]'
      + h.replace('.', '\\.') + '[">]', 'm');
    if (!incRe.test(raw)) errors.push(`R6 ${f}：未 #include "${h}"`);
    rows.push({ f, dom: 'data', defs: 0, h: hFiles.has(h) ? 'ok' : 'MISS', foreign: 0 });
    continue;
  }

  const m = f.match(/^j([a-z0-9]+)\.c$/);
  if (!m) { errors.push(`R1 ${f}：文件名必须是 j<域>.c 或登记进 DATA_SHARDS`); continue; }
  const dom = m[1];
  if (!DOMAINS.includes(dom)) { errors.push(`R1 ${f}：域 "${dom}" 不在白名单内`); continue; }
  if (got.has(dom)) { errors.push(`R2 域 ${dom} 有多个 .c：${f}`); continue; }
  got.add(dom);

  const h = `j${dom}.h`;
  const raw = fs.readFileSync(path.join(SRC, f), 'utf8');
  const defs = findDefs(strip(raw));

  /* R3：配套头存在、有 guard、被 include */
  if (!hFiles.has(h)) errors.push(`R3 ${f}：缺少配套头 src/${h}`);
  else {
    const hRaw = fs.readFileSync(path.join(SRC, h), 'utf8');
    if (!/^[ \t]*#[ \t]*ifndef\s+\w+/m.test(hRaw)) errors.push(`R3 src/${h}：缺少 include guard`);
    /* R5：域头是声明归属地，不能反向依赖聚合门面，且必须被 japi.h 聚合。 */
    if (/^[ \t]*#[ \t]*include[ \t]+["<]joy\.h[">]/m.test(hRaw)) {
      errors.push(`R5 src/${h}：不得反向 #include "joy.h"`);
    }
    if (!/^[ \t]*#[ \t]*include[ \t]+<joy\/jtypes\.h>/m.test(hRaw)) {
      errors.push(`R5 src/${h}：缺少 #include <joy/jtypes.h>`);
    }
    if (!/extern\s+"C"/.test(hRaw)) errors.push(`R5 src/${h}：缺少 extern "C" 包裹`);
    if (!japiRaw.includes(`src/${h}`)) {
      errors.push(`R5 include/joy/japi.h：未聚合包含 src/${h}`);
    }
  }
  const incRe = new RegExp('^[ \\t]*#[ \\t]*include[ \\t]+["<]j' + dom + '\\.h[">]', 'm');
  if (!incRe.test(raw)) errors.push(`R3 ${f}：未 #include "j${dom}.h"`);

  /* R4：纯域 */
  const foreign = defs.filter(d => d.domain !== dom);
  for (const d of foreign) {
    errors.push(`R4 ${f}:L${d.line}：函数 ${d.name} 属于 ${d.domain} 域，与文件域 ${dom} 不符`);
  }
  rows.push({ f, dom, defs: defs.length, h: hFiles.has(h) ? 'ok' : 'MISS', foreign: foreign.length });
}
for (const d of DOMAINS) if (!got.has(d)) errors.push(`R2 域 ${d} 缺少 src/j${d}.c`);

/* R2 生成物实现域：src/ 下不得出现该域的 .c/.h，生成物必须存在且纯域。 */
for (const d of GENERATED_ONLY_DOMAINS) {
  if (DOMAINS.includes(d)) errors.push(`R2 域 ${d} 同时登记在 DOMAINS 与 GENERATED_ONLY_DOMAINS`);
  if (fs.existsSync(path.join(SRC, `j${d}.c`)) || hFiles.has(`j${d}.h`)) {
    errors.push(`R2 生成物域 ${d} 不应有 src/j${d}.c/.h（实现应在 api/generated/）`);
  }
  const impl = GENERATED_IMPL[d];
  if (!impl || !fs.existsSync(path.join(ROOT, impl))) {
    errors.push(`R2 生成物域 ${d} 缺少实现文件 ${impl || '(未登记)'}`);
    continue;
  }
  const gDefs = findDefs(strip(fs.readFileSync(path.join(ROOT, impl), 'utf8')));
  const gForeign = gDefs.filter(x => x.domain !== d);
  if (!gDefs.length) errors.push(`R2 生成物域 ${d}：${impl} 未发现任何 joy_${d}_* 函数定义`);
  if (gForeign.length) {
    errors.push(`R2 生成物域 ${d}：${impl} 含 ${gForeign.length} 个跨域定义（首个 ${gForeign[0].name}）`);
  }
  rows.push({ f: path.basename(impl), dom: d, defs: gDefs.length, h: 'gen', foreign: gForeign.length });
}

/* 游离头文件（有 .h 无对应 .c） */
for (const h of hFiles) {
  const m = h.match(/^j([a-z0-9]+)\.h$/);
  if (m && DOMAINS.includes(m[1]) && !got.has(m[1])) errors.push(`R3 src/${h}：没有对应的 .c`);
}

/* ------------------------------------------------------------------ R7
 * 域头声明归位。R4 只检查 .c 的函数「定义」，头里的「声明」无人管，
 * 曾出现 17 条 joy_scene_bt_* / bt_* 行为树声明整块错放在 jaudio.h。
 * 两类判定：
 *   - 带 joy_<域2>_ 前缀：域段必须等于文件域（DOMAINS 内的合法域才算跨域）；
 *   - 裸名（P1 历史名）：若它在本仓某个手写域 .c 里有定义，则该域必须等于文件域。
 * 不要求「声明必须有定义」：msgpack 内联、宏包装、条件编译下不存在实现等
 * 26 处存量会误伤，因此只在能明确定位到别域实现时才报错。
 * 关键字与短占位符（if/for/bool/d/solid…）是行首正则的误匹配，直接排除。
 */
const DECL_KEYWORDS = new Set(['if', 'for', 'while', 'switch', 'return', 'sizeof', 'bool', 'd', 'solid']);
function findAnyDefs(src) {
  return new Set([...strip(src).matchAll(/\b([a-zA-Z_]\w*)\s*\([^;{}]*\)\s*\{/g)].map(m => m[1]));
}
function findDecls(src) {
  const out = [];
  for (const line of strip(src).split(/\r?\n/)) {
    const m = line.match(/^\s*(?:JOY_API\s+)?[\w*]+\s*\*?\s*([A-Za-z_]\w*)\s*\([^;]*\)\s*;/);
    if (m && !DECL_KEYWORDS.has(m[1])) out.push(m[1]);
  }
  return out;
}
/* name -> 定义所在的手写域；同名多处定义不报（R2 已保证一域一 .c）。 */
const defOwner = new Map();
for (const d of got) {
  const p = path.join(SRC, `j${d}.c`);
  for (const n of findAnyDefs(fs.readFileSync(p, 'utf8'))) {
    if (!defOwner.has(n)) defOwner.set(n, d);
  }
}
for (const d of got) {
  const h = `j${d}.h`;
  const hSrc = fs.readFileSync(path.join(SRC, h), 'utf8');
  for (const n of findDecls(hSrc)) {
    const m = n.match(/^joy_([a-z0-9]+)_/);
    if (m) {
      if (DOMAINS.includes(m[1]) && m[1] !== d) {
        errors.push(`R7 src/${h}：声明 ${n} 属于 ${m[1]} 域，应放在 src/j${m[1]}.h`);
      }
    } else if (defOwner.has(n) && defOwner.get(n) !== d) {
      errors.push(`R7 src/${h}：裸名声明 ${n} 的定义在 ${defOwner.get(n)} 域，应放在 src/j${defOwner.get(n)}.h`);
    }
  }
}

console.log('域布局审计：一域一 .c、每 .c 配 .h、定义与声明均纯域、引用方向正确、数据分片已登记');
console.log('='.repeat(70));
for (const r of rows) {
  console.log(`  ${r.f.padEnd(20)} 域=${r.dom.padEnd(7)} 函数定义=${String(r.defs).padStart(4)}  配套头=${r.h.padEnd(5)} 跨域=${r.foreign}`);
}
console.log('='.repeat(70));
console.log(`src/ .c ${cFiles.length} 个（域实现 ${got.size} + 数据分片 ${DATA_SHARDS.length}）`
  + ` | 覆盖域 ${got.size}/${DOMAINS.length} + 生成物域 ${GENERATED_ONLY_DOMAINS.length}`
  + ` | 违规 ${errors.length} 条`);

if (errors.length) {
  console.log('');
  for (const e of errors) console.log(`  ${e}`);
  console.error(`\nFAIL：域布局规则有 ${errors.length} 处违规。`);
  process.exit(1);
}
console.log('\nOK：每个域恰有一个 .c，每个 .c 都有配套头且纯域，'
  + '声明也归位到所属域头、引用方向正确，数据分片已登记且无函数定义。');
