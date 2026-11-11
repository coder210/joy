#!/usr/bin/env node
'use strict';
/* C 类型名统一工具（TYPE_RENAME_TASK.md §6）。
 *
 *   node api/tools/rename_types.js --build-map  生成 type_rename_map.json + private_type_map.json + handle_aliases.json
 *   node api/tools/rename_types.js --dry-run    只打印命中报告，不改文件
 *   node api/tools/rename_types.js --write      执行替换（可配 --only / --map / --tags）
 *   node api/tools/rename_types.js --audit      全词审计旧名残留（合并公开+私有两张表）
 *   node api/tools/rename_types.js --check-aliases  校验冻结表与现有生成物类名集合一致
 *
 * 选项：--only <前缀,逗号分隔>  仅处理这些相对路径前缀（如 src/,include/joy/）
 *       --map public|private|all  选映射表（默认 public）
 *       --tags                  额外做 struct-tag 定向替换（默认关闭，见 §1 说明）
 *
 * 批 1 用 --map private --only src/：私有类型不外泄，生成器不扫 src/*.c，
 * 因此 --check 应零变化。公开类型名在批 2 才原子处理。
 *
 * 约束：字符类全词边界、原位替换不增删行、保留每个文件原换行风格。
 */
const fs = require('fs');
const path = require('path');

const ROOT = path.resolve(__dirname, '../..');
const rel = f => path.relative(ROOT, f).split(path.sep).join('/');
const P = (...s) => path.join(ROOT, ...s);

const MAP_FILE = P('api/idl/type_rename_map.json');
const PRIVATE_MAP_FILE = P('api/idl/private_type_map.json');
const ALIAS_FILE = P('api/idl/handle_aliases.json');

/* §3.2 标准 C 类型（含 SCALARS 里出现的别名），不能加 joy_ */
const STDLIB = new Set([
  'int8_t', 'uint8_t', 'int16_t', 'uint16_t', 'int32_t', 'uint32_t',
  'int64_t', 'uint64_t', 'size_t', 'ssize_t', 'intptr_t', 'uintptr_t',
  'wchar_t', 'time_t', 'va_list', 'ptrdiff_t', 'jmp_buf',
]);
/* §3.3 这两个是宏函数 jvector_t(name)/jvector_p(name)，不是类型；改成 joy_vector_t
 * 会与真类型撞车。映射表里不建条目。 */
const MACRO_FNS = new Set(['jvector_t', 'jvector_p']);
/* 注意：注释里 `r3d_p`（src/jr3d.c:3321、include/joy/api.h:1695）是对 jr3d_p 的
 * 笔误旧称，不是真实 typedef。采集时剥离注释，避免它进映射表后把冻结表的
 * joy_r3d_p 从 Jr3d 覆盖成 R3d。这两处注释在批 2 人工改为 joy_r3d_p。 */

/* 不参与替换的路径（生成物由生成器重出、文档是历史留档、第三方与构建产物） */
const EXCLUDE_PREFIX = [
  'SDL/', 'msgpack/', 'spine/', 'external/', 'runtimes/', 'android/',
  'build/', '.git/', 'api/generated/', 'api/idl/',
  'modules/cs/bin/', 'modules/cs/obj/',
];
const EXCLUDE_FILES = new Set([
  'modules/cs/bindings/api/api_generated.c',
  'modules/cs/scripts/Api.generated.cs',
  'api/tools/rename_types.js',   /* 自身含排除名单字面量，不能被替换 */
]);
const SRC_EXT = /\.(c|h|cpp|hpp|cc|cxx|js|cs)$/;

function pascal(s) {
  return s.split('_').filter(Boolean).map(w => w[0].toUpperCase() + w.slice(1)).join('');
}

/* §1 命名规则：j<概念> -> joy_<概念>，<概念> -> joy_<概念>，后缀不变，不插域 */
function toNew(n) {
  if (n.startsWith('joy_')) return n;
  if (n.startsWith('joystick_')) return 'joy_joystick_' + n.slice('joystick_'.length);
  if (n.startsWith('j')) return 'joy_' + n.slice(1);
  return 'joy_' + n;
}

/* 采集公开头里所有 _t/_p 标识符 */
function collectTypeNames() {
  const dir = P('include/joy');
  const out = new Set();
  for (const f of fs.readdirSync(dir).filter(x => x.endsWith('.h'))) {
    /* 只清注释字符、保留换行（不影响后续按行原位替换）；注释里的笔误旧称不应
     * 被当成类型名。 */
    const s = fs.readFileSync(path.join(dir, f), 'utf8')
      .replace(/\/\*[\s\S]*?\*\//g, m => m.replace(/[^\n]/g, ''))
      .replace(/\/\/[^\n]*/g, '');
    let m;
    const re = /\b([A-Za-z_]\w*_(?:t|p))\b/g;
    while ((m = re.exec(s)) !== null) out.add(m[1]);
  }
  return [...out].filter(n => !STDLIB.has(n) && !MACRO_FNS.has(n)).sort();
}

function buildMap() {
  const map = {};
  for (const n of collectTypeNames()) map[n] = toNew(n);
  return map;
}

/* 批 1：采集 src/*.c 内定义、且完全不外泄的私有 typedef 名。
 * 排除：标准 C 类型、公开头同名（归批 2）、外泄到公开头/生成物/示例、无后缀通用词
 * （client/server/v/e 等，全词替换会误伤变量）。 */
function buildPrivateMap(publicMap) {
  const pubNames = new Set(Object.keys(publicMap));
  const defined = new Map();
  const strip = s => s.replace(/\/\*[\s\S]*?\*\//g, m => m.replace(/[^\n]/g, ''))
    .replace(/\/\/[^\n]*/g, '');
  const src = fs.readdirSync(P('src')).filter(f => f.endsWith('.c')).map(f => P('src', f));
  for (const f of src) {
    const s = strip(fs.readFileSync(f, 'utf8'));
    let m;
    const patterns = [
      /\btypedef\b[^;{}]*?(?:\*\s*)?([A-Za-z_]\w*)\s*;/g,
      /^\s*\}\s*([A-Za-z_]\w*)\s*;/gm,
      /\btypedef\s+(?:struct|union|enum)\s+(\w+)\s*;/g,
    ];
    for (const re of patterns) while ((m = re.exec(s)) !== null) defined.set(m[1], true);
  }
  const outside = [];
  const walk = d => {
    let e; try { e = fs.readdirSync(d, { withFileTypes: true }); } catch { return; }
    for (const x of e) {
      const fp = path.join(d, x.name);
      if (x.isDirectory()) {
        if (/^(build|bin|obj|node_modules|\.git|SDL|msgpack|spine|external|runtimes)/.test(x.name)) continue;
        walk(fp);
      } else if (/\.(c|h|cpp|hpp|js|cs|idl|json|md)$/.test(x.name)) outside.push(fp);
    }
  };
  for (const d of ['include/joy','api/generated','api/idl','api/tools','api/runtime','examples','modules']) walk(P(d));
  const text = outside.map(f => { try { return fs.readFileSync(f, 'utf8'); } catch { return ''; } }).join('\n');
  const map = {};
  for (const n of [...defined.keys()].sort()) {
    if (STDLIB.has(n) || pubNames.has(n)) continue;
    if (!/_(?:t|p|e|k)$/.test(n)) continue;
    if (new RegExp('\\b' + esc(n) + '\\b').test(text)) continue;
    map[n] = toNew(n);
  }
  return map;
}

/* 运行期一律从文件读映射表，避免批 2 改完公开头后重新采集出 joy_x -> joy_x 自映射。 */
function loadMap(which) {
  const read = f => JSON.parse(fs.readFileSync(f, 'utf8'));
  if (which === 'private') return read(PRIVATE_MAP_FILE);
  if (which === 'all') return { ...read(MAP_FILE), ...read(PRIVATE_MAP_FILE) };
  return read(MAP_FILE);
}

/* §4 冻结脚本层句柄类名：key = 新类型名，value = 改名前的类名基线 */
function buildHandleAliases(map) {
  const out = {};
  for (const [oldN, newN] of Object.entries(map)) {
    if (!oldN.endsWith('_p')) continue;
    out[newN] = pascal(oldN.replace(/_p$/, ''));
  }
  return out;
}

/* 需要替换的文件集合 */
function collectFiles(only) {
  const roots = ['src', 'include', 'examples', 'modules', 'api'];
  const out = [];
  const walk = d => {
    let ents;
    try { ents = fs.readdirSync(d, { withFileTypes: true }); } catch { return; }
    for (const e of ents) {
      const full = path.join(d, e.name);
      const r = rel(full);
      if (e.isDirectory()) {
        if (/^(node_modules|\.git|bin|obj|build)/.test(e.name)) continue;
        walk(full);
      } else if (SRC_EXT.test(e.name) && !/\.tmp-/.test(e.name)) {
        out.push(r);
      }
    }
  };
  for (const r of roots) walk(P(r));
  return out.filter(r =>
    !EXCLUDE_PREFIX.some(x => r.startsWith(x)) &&
    !EXCLUDE_FILES.has(r) &&
    (!only.length || only.some(x => r.startsWith(x)))
  ).sort();
}

const esc = s => s.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
function boundaryRe(name) {
  return new RegExp('(^|[^A-Za-z0-9_])' + esc(name) + '(?=$|[^A-Za-z0-9_])', 'g');
}
/* struct tag 定向替换：只改 `struct <tag>` 上下文，绝不碰同名变量/字段 */
function tagRe(name) {
  return new RegExp('(\\bstruct\\s+)' + esc(name) + '(?=$|[^A-Za-z0-9_])', 'g');
}

function readEol(s) { return s.includes('\r\n') ? '\r\n' : '\n'; }

function doReplace(src, map, tags) {
  let hits = 0;
  for (const [oldN, newN] of Object.entries(map)) {
    const re = boundaryRe(oldN);
    const n = (src.match(re) || []).length;
    if (n) { src = src.replace(re, (m, pre) => pre + newN); hits += n; }
  }
  if (tags) {
    for (const [oldN, newN] of Object.entries(tags)) {
      const re = tagRe(oldN);
      const n = (src.match(re) || []).length;
      if (n) { src = src.replace(re, (m, pre) => pre + newN); hits += n; }
    }
  }
  return { src, hits };
}

/* struct tag：只在 `typedef struct TAG` 与 `struct TAG` 上下文出现，单独成表 */
function buildTagMap() {
  const dir = P('include/joy');
  const out = new Set();
  for (const f of ['types.h', 'defs.h', 'jmath.h', 'jcont.h']) {
    const fp = path.join(dir, f);
    if (!fs.existsSync(fp)) continue;
    const s = fs.readFileSync(fp, 'utf8');
    let m;
    const re = /\btypedef\s+struct\s+(\w+)\s/g;
    while ((m = re.exec(s)) !== null) {
      if (!m[1].endsWith('_t') && !m[1].endsWith('_p')) out.add(m[1]);
    }
  }
  const map = {};
  for (const t of [...out].sort()) map[t] = toNew(t);
  return map;
}

function parseArgs(argv) {
  const a = { only: [], tags: false, cmd: null, map: 'public' };
  for (let i = 0; i < argv.length; i++) {
    if (argv[i] === '--only') a.only = argv[++i].split(',').filter(Boolean);
    else if (argv[i] === '--map') a.map = argv[++i];
    else if (argv[i] === '--tags') a.tags = true;
    else a.cmd = argv[i];
  }
  return a;
}

function main() {
  const args = parseArgs(process.argv.slice(2));

  if (args.cmd === '--build-map') {
    const eol = '\n';
    const pub = buildMap();
    const priv = buildPrivateMap(pub);
    fs.writeFileSync(MAP_FILE, JSON.stringify(pub, null, 2) + eol, 'utf8');
    fs.writeFileSync(PRIVATE_MAP_FILE, JSON.stringify(priv, null, 2) + eol, 'utf8');
    fs.writeFileSync(ALIAS_FILE, JSON.stringify(buildHandleAliases(pub), null, 2) + eol, 'utf8');
    console.log('type_rename_map.json', Object.keys(pub).length, '条');
    console.log('private_type_map.json', Object.keys(priv).length, '条');
    console.log('handle_aliases.json', Object.keys(buildHandleAliases(pub)).length, '条');
    return;
  }

  const map = loadMap(args.map);
  const tagMap = args.tags ? buildTagMap() : null;

  if (args.cmd === '--check-aliases') {
    const aliases = JSON.parse(fs.readFileSync(ALIAS_FILE, 'utf8'));
    const vt = fs.readFileSync(P('api/generated/joy_api_vtable.h'), 'utf8');
    const cs = fs.readFileSync(P('modules/cs/scripts/Api.generated.cs'), 'utf8');
    const vCls = new Set([...vt.matchAll(/typedef\s+struct\s+(\w+)\s*\{\s*uint64_t\s+h;\s*\}\s*\1\s*;/g)].map(m => m[1]));
    const cCls = new Set([...cs.matchAll(/public\s+sealed\s+class\s+(\w+)/g)].map(m => m[1]));
    const baseline = new Set(Object.values(aliases));
    const missV = [...vCls].filter(x => !baseline.has(x) && x !== 'JoyApi');
    const missC = [...cCls].filter(x => !baseline.has(x));
    console.log('vtable 句柄类', vCls.size, 'C# sealed 类', cCls.size, '冻结基线', baseline.size);
    console.log('未被冻结表覆盖(vtable):', JSON.stringify(missV));
    console.log('未被冻结表覆盖(C#):', JSON.stringify(missC));
    return;
  }

  const files = collectFiles(args.only);
  const perFile = [];
  const perName = new Map();
  let total = 0;
  for (const r of files) {
    const s = fs.readFileSync(P(r), 'utf8');
    let hits = 0;
    for (const [oldN, newN] of Object.entries(map)) {
      const n = (s.match(boundaryRe(oldN)) || []).length;
      if (n) { hits += n; perName.set(oldN, (perName.get(oldN) || 0) + n); }
    }
    if (tagMap) {
      for (const oldN of Object.keys(tagMap)) {
        const n = (s.match(tagRe(oldN)) || []).length;
        if (n) hits += n;
      }
    }
    if (hits) { perFile.push([r, hits]); total += hits; }
  }

  console.log('映射表', Object.keys(map).length, '条', tagMap ? '+ tag ' + Object.keys(tagMap).length + ' 条' : '');
  console.log('扫描文件', files.length, '命中文件', perFile.length, '总命中', total);
  if (args.only.length) console.log('限定范围:', args.only.join(', '));

  if (args.cmd === '--dry-run' || args.cmd === '--write') {
    console.log('\n=== 命中文件 ===');
    for (const [r, n] of perFile) console.log('  ' + String(n).padStart(5) + '  ' + r);
    console.log('\n=== 命中最多 15 个名字 ===');
    for (const [k, n] of [...perName.entries()].sort((a, b) => b[1] - a[1]).slice(0, 15))
      console.log('  ' + String(n).padStart(5) + '  ' + k + ' -> ' + map[k]);
    console.log('\n=== 零命中名字（前 20）===');
    const zero = Object.keys(map).filter(k => !perName.has(k));
    console.log('  共', zero.length, ':', zero.slice(0, 20).join(', '));
  }

  if (args.cmd === '--write') {
    let written = 0;
    for (const [r] of perFile) {
      const fp = P(r);
      const raw = fs.readFileSync(fp, 'utf8');
      const eol = readEol(raw);
      const lines = raw.split(/\r?\n/);
      let changed = 0;
      for (let i = 0; i < lines.length; i++) {
        const out = doReplace(lines[i], map, tagMap);
        if (out.hits) { lines[i] = out.src; changed += out.hits; }
      }
      if (changed) { fs.writeFileSync(fp, lines.join(eol), 'utf8'); written++; }
    }
    console.log('\n已写入', written, '个文件');
  }

  if (args.cmd === '--audit') {
    console.log('\n=== 审计：残留旧名（应为空）===');
    for (const [r, n] of perFile) console.log('  ' + String(n).padStart(5) + '  ' + r);
  }
}

main();
