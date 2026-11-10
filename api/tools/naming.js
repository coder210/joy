/*
 * 三端共用的命名与类型判定规则。
 *
 * 同一份 joy.idl 必须生成同一套脚本层方法名 / 对象名，否则
 * lua / js / cs 三个后端又会各自长出一套方言。后端生成器一律从这里取规则，
 * 不要在自己文件里另写一份 pascal() / isOpaque()。
 *
 * 同时这里也是「公开 API 命名规范」的唯一真相源：gen_bindings.js 的
 * domainOf() / --check 门禁以及三端方法名都从这里取域白名单。
 */
'use strict';

/* ---------------------------------------------------------------- 域白名单
 * 封闭清单：新增域必须改这里并评审，避免 domain 再次自发生长成 85 个。
 * 值里记的是被归并进来的历史 domain（见 api/README.md 的域表）。
 * 目标格式：joy_<域>_<动作>[_宾语]，例如 joy_core_create_window。
 */
const NS_PREFIX = 'joy';
const DOMAINS = new Set([
  'core',   // env/window/log/error/ticks/clipboard/event/utf8/hash
  'math',   // jmath + collision*/gjk*/box2d/shape
  'r2d',    // r2d/font/jtext/image/jspine/jparticle
  'r3d',    // r3d/jlight/jgltf
  'ui',     // jui/jui_im/jlayout + 全部控件（button/combobox/inputbox/...）
  'scene',  // scene/game/jecs/jtilemap/janimator/animation/jtimer/jpm
  'net',    // net*/tcp*/kcp*/ws*/iocp/pack/unpack/netmessage
  'audio',  // audio
  'input',  // joystick/mouse
  'script', // modules/module/jmsgpack
]);

/* 三段式新名字 → 域名；不符合（存量名字、拼错的前缀）返回 null。
 * 严格校验域白名单，joy_foo_bar 里 foo 不在清单时不能混过门禁。 */
function namespacedDomain(name) {
  const parts = String(name).split('_');
  if (parts.length < 3 || parts[0] !== NS_PREFIX) return null;
  return DOMAINS.has(parts[1]) ? parts[1] : null;
}

function isNamespaced(name) { return namespacedDomain(name) !== null; }

/* ------------------------------------------------------------ 脚本层名冻结
 * P1 迁移改名时，C 名变了但三端方法名必须保持旧值（否则 Lua / JS / C#
 * 绑定和 C++ vtable 全部跟着变）。迁移哪一条就在这里登记一条：
 *   'joy_core_create_window': 'CreateWindow'
 * 注意不能用「简单剥掉 joy_<域>」推导，历史名如 JanimatorCreate 带 J 前缀，
 * 显式表才不会把它们改坏。P0 阶段表为空，所有生成物一字不变。 */
const ALIASES = new Map([
  /* P1 第一批：core/window 子域。旧名 → 生成层一直使用的 PascalCase 名。 */
  ['joy_core_create_window', 'CreateWindow'],
  ['joy_core_destroy_window', 'DestroyWindow'],
  ['joy_core_get_raw_window', 'GetRawWindow'],
  ['joy_core_window_set_fullscreen', 'WindowSetFullscreen'],
  ['joy_core_window_get_fullscreen', 'WindowGetFullscreen'],
  ['joy_core_window_set_icon', 'WindowSetIcon'],
]);

function isOpaque(t) {
  return t === 'void*' || (t.endsWith('_p') && !t.includes('*'));
}

function pascal(s) {
  return s.split('_').filter(Boolean).map(w => w[0].toUpperCase() + w.slice(1)).join('');
}

/* 脚本层方法名：三个后端统一用 PascalCase，参数顺序与 joy.h 一致。 */
function methodName(name) { return ALIASES.get(name) || pascal(name); }

/* 句柄对象的强类型类名：lcg_num_p → LcgNum。 */
function handleClass(ret) { return pascal(ret.replace(/_p$/, '')); }

module.exports = {
  isOpaque, pascal, methodName, handleClass,
  NS_PREFIX, DOMAINS, namespacedDomain, isNamespaced, ALIASES,
};
