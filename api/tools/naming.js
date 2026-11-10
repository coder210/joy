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
  /* P1 第二批：core 剩余 39 条（hash / utf8 / lcg / env / log / event / fps / clipboard）。 */
  ['joy_core_lcg_random_create', 'LcgRandomCreate'],
  ['joy_core_lcg_random_destroy', 'LcgRandomDestroy'],
  ['joy_core_lcg_random_next', 'LcgRandomNext'],
  ['joy_core_lcg_random_get_state', 'LcgRandomGetState'],
  ['joy_core_lcg_random_set_state', 'LcgRandomSetState'],
  ['joy_core_sdbm_hash', 'SdbmHash'],
  ['joy_core_fnv1a_hash', 'Fnv1aHash'],
  ['joy_core_jenkins_hash', 'JenkinsHash'],
  ['joy_core_murmur3_hash', 'Murmur3Hash'],
  ['joy_core_kr_hash', 'KrHash'],
  ['joy_core_utf8_decode', 'Utf8Decode'],
  ['joy_core_utf8_next', 'Utf8Next'],
  ['joy_core_utf8_decode_all', 'Utf8DecodeAll'],
  ['joy_core_init_env', 'InitEnv'],
  ['joy_core_quit_env', 'QuitEnv'],
  ['joy_core_get_error', 'GetError'],
  ['joy_core_get_ticks', 'GetTicks'],
  ['joy_core_get_mouse_pos', 'GetMousePos'],
  ['joy_core_get_mouse_button', 'GetMouseButton'],
  ['joy_core_dispatch_event', 'DispatchEvent'],
  ['joy_core_get_event_type', 'GetEventType'],
  ['joy_core_get_event_code', 'GetEventCode'],
  ['joy_core_is_user_event', 'IsUserEvent'],
  ['joy_core_should_close', 'ShouldClose'],
  ['joy_core_log_info', 'LogInfo'],
  ['joy_core_log_debug', 'LogDebug'],
  ['joy_core_log_error', 'LogError'],
  ['joy_core_log_title', 'LogTitle'],
  ['joy_core_env_create', 'EnvCreate'],
  ['joy_core_env_get', 'EnvGet'],
  ['joy_core_env_set', 'EnvSet'],
  ['joy_core_env_destroy', 'EnvDestroy'],
  ['joy_core_simple_fps_create', 'SimpleFpsCreate'],
  ['joy_core_simple_fps_destory', 'SimpleFpsDestory'],
  ['joy_core_simple_fps_update', 'SimpleFpsUpdate'],
  ['joy_core_simple_fps_value', 'SimpleFpsValue'],
  ['joy_core_get_clipboard_text', 'GetClipboardText'],
  ['joy_core_set_clipboard_text', 'SetClipboardText'],
  ['joy_core_get_local_ips', 'GetLocalIps'],
  /* P1 第三批：碎片归位（pack/unpack → net，jtimer/game_timer → scene，
   * push → core，find_min_separation → math）。 */
  ['joy_net_pack_int8', 'PackInt8'],
  ['joy_net_pack_int16', 'PackInt16'],
  ['joy_net_pack_int32', 'PackInt32'],
  ['joy_net_pack_uint32', 'PackUint32'],
  ['joy_net_pack_int64', 'PackInt64'],
  ['joy_net_unpack_int8', 'UnpackInt8'],
  ['joy_net_unpack_int16', 'UnpackInt16'],
  ['joy_net_unpack_int32', 'UnpackInt32'],
  ['joy_net_unpack_uint32', 'UnpackUint32'],
  ['joy_net_unpack_int64', 'UnpackInt64'],
  ['joy_net_pack_string', 'PackString'],
  ['joy_net_unpack_string', 'UnpackString'],
  ['joy_scene_jtimer_create', 'JtimerCreate'],
  ['joy_scene_jtimer_destroy', 'JtimerDestroy'],
  ['joy_scene_jtimer_trigger', 'JtimerTrigger'],
  ['joy_scene_jtimer_set_interval', 'JtimerSetInterval'],
  ['joy_scene_jtimer_set_active', 'JtimerSetActive'],
  ['joy_scene_jtimer_reset', 'JtimerReset'],
  ['joy_scene_jtimer_get_elapsed', 'JtimerGetElapsed'],
  ['joy_core_push_user_event', 'PushUserEvent'],
  ['joy_core_push_quit_event', 'PushQuitEvent'],
  ['joy_scene_game_timer_init', 'GameTimerInit'],
  ['joy_scene_game_timer_update', 'GameTimerUpdate'],
  ['joy_scene_game_timer_get_delta_time', 'GameTimerGetDeltaTime'],
  ['joy_scene_game_timer_get_unscaled_delta_time', 'GameTimerGetUnscaledDeltaTime'],
  ['joy_scene_game_timer_set_time_scale', 'GameTimerSetTimeScale'],
  ['joy_scene_game_timer_get_time_scale', 'GameTimerGetTimeScale'],
  ['joy_scene_game_timer_set_target_fps', 'GameTimerSetTargetFps'],
  ['joy_scene_game_timer_get_target_fps', 'GameTimerGetTargetFps'],
  ['joy_math_find_min_separation', 'FindMinSeparation'],
  /* P1 第四批：net 协议域合并（iocp / kcp / tcp / ws / net / netmessage）。 */
  ['joy_net_iocp_server_create', 'IocpServerCreate'],
  ['joy_net_iocp_server_destroy', 'IocpServerDestroy'],
  ['joy_net_iocp_process_events', 'IocpProcessEvents'],
  ['joy_net_kcpserver_create', 'KcpserverCreate'],
  ['joy_net_kcpserver_destroy', 'KcpserverDestroy'],
  ['joy_net_kcpserver_send', 'KcpserverSend'],
  ['joy_net_kcpserver_broadcast', 'KcpserverBroadcast'],
  ['joy_net_kcpserver_offline', 'KcpserverOffline'],
  ['joy_net_kcpserver_update', 'KcpserverUpdate'],
  ['joy_net_kcpserver_connection_count', 'KcpserverConnectionCount'],
  ['joy_net_kcpserver_poll_message', 'KcpserverPollMessage'],
  ['joy_net_kcpserver_set_callback', 'KcpserverSetCallback'],
  ['joy_net_kcpclient_create', 'KcpclientCreate'],
  ['joy_net_kcpclient_destroy', 'KcpclientDestroy'],
  ['joy_net_kcpclient_send', 'KcpclientSend'],
  ['joy_net_kcpclient_update', 'KcpclientUpdate'],
  ['joy_net_kcpclient_poll_message', 'KcpclientPollMessage'],
  ['joy_net_kcpclient_set_callback', 'KcpclientSetCallback'],
  ['joy_net_tcpserver_create', 'TcpserverCreate'],
  ['joy_net_tcpserver_destroy', 'TcpserverDestroy'],
  ['joy_net_tcpserver_send', 'TcpserverSend'],
  ['joy_net_tcpserver_broadcast', 'TcpserverBroadcast'],
  ['joy_net_tcpserver_offline', 'TcpserverOffline'],
  ['joy_net_tcpserver_update', 'TcpserverUpdate'],
  ['joy_net_tcpserver_connection_count', 'TcpserverConnectionCount'],
  ['joy_net_tcpserver_poll_message', 'TcpserverPollMessage'],
  ['joy_net_tcpclient_create', 'TcpclientCreate'],
  ['joy_net_tcpclient_destroy', 'TcpclientDestroy'],
  ['joy_net_tcpclient_send', 'TcpclientSend'],
  ['joy_net_tcpclient_update', 'TcpclientUpdate'],
  ['joy_net_tcpclient_poll_message', 'TcpclientPollMessage'],
  ['joy_net_wsnetserver_create', 'WsnetserverCreate'],
  ['joy_net_wsnetserver_destroy', 'WsnetserverDestroy'],
  ['joy_net_wsnetserver_send', 'WsnetserverSend'],
  ['joy_net_wsnetserver_broadcast', 'WsnetserverBroadcast'],
  ['joy_net_wsnetserver_offline', 'WsnetserverOffline'],
  ['joy_net_wsnetserver_update', 'WsnetserverUpdate'],
  ['joy_net_wsnetserver_connection_count', 'WsnetserverConnectionCount'],
  ['joy_net_wsnetserver_poll_message', 'WsnetserverPollMessage'],
  ['joy_net_wsnetserver_set_callback', 'WsnetserverSetCallback'],
  ['joy_net_wsnetclient_create', 'WsnetclientCreate'],
  ['joy_net_wsnetclient_destroy', 'WsnetclientDestroy'],
  ['joy_net_wsnetclient_send', 'WsnetclientSend'],
  ['joy_net_wsnetclient_update', 'WsnetclientUpdate'],
  ['joy_net_wsnetclient_poll_message', 'WsnetclientPollMessage'],
  ['joy_net_wsnetclient_set_callback', 'WsnetclientSetCallback'],
  ['joy_net_netclient_create', 'NetclientCreate'],
  ['joy_net_netclient_destroy', 'NetclientDestroy'],
  ['joy_net_netclient_send', 'NetclientSend'],
  ['joy_net_netclient_update', 'NetclientUpdate'],
  ['joy_net_netclient_poll_message', 'NetclientPollMessage'],
  ['joy_net_netclient_set_callback', 'NetclientSetCallback'],
  ['joy_net_netclient_get_type', 'NetclientGetType'],
  ['joy_net_netserver_create', 'NetserverCreate'],
  ['joy_net_netserver_destroy', 'NetserverDestroy'],
  ['joy_net_netserver_send', 'NetserverSend'],
  ['joy_net_netserver_broadcast', 'NetserverBroadcast'],
  ['joy_net_netserver_offline', 'NetserverOffline'],
  ['joy_net_netserver_update', 'NetserverUpdate'],
  ['joy_net_netserver_connection_count', 'NetserverConnectionCount'],
  ['joy_net_netserver_poll_message', 'NetserverPollMessage'],
  ['joy_net_netserver_set_callback', 'NetserverSetCallback'],
  ['joy_net_netserver_get_type', 'NetserverGetType'],
  ['joy_net_netmessage_free_data', 'NetmessageFreeData'],
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
