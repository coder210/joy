'use strict';
/*
 * C++ 后端（第 4 个后端）强签名 vtable 生成器（纯字符串函数，不写文件）。
 *
 * 输入：api/idl/joy.idl（唯一真相源）+ emit_thunks.js 的 SCALAR 类型映射
 *       + naming.js 的 isOpaque / methodName / handleClass
 *       + structs.js 的按字段展开（与 thunk / cs 共用同一份布局）。
 * 输出：api/generated/joy_api_vtable.h / .c 的完整文本。
 *
 * 与 thunk 层的区别：thunk 暴露统一签名
 *   joy_value_t (*)(joy_value_t*, int, joy_status_t*)   —— 运行时类型擦除；
 * 这里每条 API 一个精确 C 签名，参数/返回类型逐字取自 IDL 的
 * params[].ctype / ret —— 编译期强类型不丢。shim 内部才 pack/unpack
 * joy_value 再走 joy_api_call() 进共享 thunk 表，不重复任何业务逻辑。
 *
 * 覆盖 status:on 的全部 L0/L1，以及可按字段展开的 L2：
 *   - 标量（含 float/double/bool/unsigned long）用原始 C 类型；
 *   - const char* 保持 const char*（非 const char* 是输出缓冲，thunk 也拒绝）；
 *   - 不透明指针 jr2d_p → Jr2d 等强句柄薄包装，不同类型编译期不可混传；
 *   - L2 值结构体（jrect_t / jcolor_t）按字段展开成标量，与 thunk 表 argc 一致。
 * L3（结构体指针 / 输出参数）仍是 status:off，不进表。
 */
const { SCALAR } = require('./emit_thunks');
const { isOpaque, methodName, handleClass } = require('./naming');
const { collectStructs, canExpand, expandParams } = require('./structs');
const LAYOUTS = collectStructs();

/* 运行时自检只跑确定性、无副作用的纯函数。放开白名单后数百条 API 里
 * 混着 create_window / sys_bind / audio_create_* / quit_env 等，全跑会出乱子；
 * 其余 API 由编译期签名 + init 填表保证，不做运行时调用。 */
const SELFTEST_SAFE = new Set([
  'jpm_wave_seed',                                              // 纯标量
  'joy_core_sdbm_hash', 'joy_core_fnv1a_hash', 'joy_core_jenkins_hash',
  'joy_core_murmur3_hash', 'joy_core_kr_hash',                       // const char*
  'jmath_fp_from_float',                                       // float 参数
]);
/* 强句柄链路探针：单参数句柄 getter，传 h=0 应被 thunk 拒绝并返回零值。 */
/* 改名为 joy_<域>_ 后要同步这里，否则探针找不到条目会被静默跳过。 */
const HANDLE_PROBE = 'joy_core_window_get_fullscreen';

/* IDL 里有 63 个参数名叫 r；生成代码的局部变量统一加前缀避免遮蔽。 */
const P = '_jv_';

/* 仅用于自检的输入样本，与类型映射无关；特意取非零值。 */
const SAMPLE = {
  bool: '1', char: '7', int: '3', int32_t: '3', uint8_t: '7',
  uint16_t: '17', uint32_t: '12345', int64_t: '123456789',
  uint64_t: '123456789', size_t: '4', 'unsigned int': '3',
  'unsigned long': '7', float: '1.5f', double: '1.5',
  'char*': '"joy-vtable"',
};

function okHandle(t) { return isOpaque(t) && t !== 'void*'; }

function isParamType(p) {
  if (okHandle(p.ctype)) return true;
  if (p.ctype === 'char*') return !!p.isConst;
  return !isOpaque(p.ctype) && !!SCALAR[p.ctype];
}
function isRetType(t) {
  if (t === 'void' || okHandle(t)) return true;
  return !!SCALAR[t] && t !== 'char*' && !isOpaque(t);
}

function isExpandableL2(e) {
  return e.level === 'L2' && !!e.params && canExpand(e, LAYOUTS);
}
/* 强签名用的参数列表：L2 结构体按字段展开成标量，与 thunk 表 argc 完全一致。
 * shim 只把展开后的标量 pack 成 joy_value[]，重组回结构体由 thunk 完成。 */
function paramsOf(e) {
  return isExpandableL2(e) ? expandParams(e, LAYOUTS) : (e.params || []);
}
function canVtable(e) {
  if (e.status !== 'on' || e.orphan || !e.params) return false;
  if (e.level !== 'L0' && e.level !== 'L1' && !isExpandableL2(e)) return false;
  if (isExpandableL2(e) && okHandle(e.ret)) return false; // 与 thunk 的 L2 限制一致
  if (!isRetType(e.ret)) return false;
  return paramsOf(e).every(isParamType);
}

/* 强签名里的类型：句柄换成类名，const char* 保留 const，标量原样。 */
function cType(t, isConst) {
  if (okHandle(t)) return handleClass(t);
  if (t === 'char*') return isConst ? 'const char*' : 'char*';
  return t;
}

function q(s) { return '"' + s + '"'; }
function shimName(e) { return 'shim_' + e.name; }
function paramDecl(e) {
  return ['void* ' + P + 'ctx'].concat(
    paramsOf(e).map(p => cType(p.ctype, p.isConst) + ' ' + p.name)).join(', ');
}
function fnPtr(e) {
  return cType(e.ret) + ' (*' + methodName(e.name) + ')(' + paramDecl(e) + ')';
}
function sampleExpr(p) {
  if (p.ctype === 'char*') return '(const char*)' + SAMPLE['char*'];
  return '(' + cType(p.ctype, p.isConst) + ')' + SAMPLE[p.ctype];
}
function handleTypes(open) {
  const set = new Set();
  for (const e of open) {
    if (okHandle(e.ret)) set.add(handleClass(e.ret));
    for (const p of paramsOf(e)) if (okHandle(p.ctype)) set.add(handleClass(p.ctype));
  }
  return [...set].sort();
}

const BANNER = [
  '/* ============================================================',
  ' * 自动生成，勿手改：node api/tools/gen_bindings.js --emit',
  ' * 强签名 JoyApi vtable（C++ 后端 / 第 4 个后端）。',
  ' * 覆盖 joy.idl 里 status:on 的 L0/L1 与可展开的 L2；参数类型逐字取自 IDL。',
  ' * ============================================================ */',
];

function buildVtableHeader(entries) {
  const open = entries.filter(canVtable);
  const handles = handleTypes(open);
  const L = BANNER.slice();
  L.push('#ifndef JOY_API_VTABLE_H');
  L.push('#define JOY_API_VTABLE_H');
  L.push('');
  L.push('#include <stdint.h>');
  L.push('#include <stdbool.h>');
  L.push('#include <stddef.h>');
  L.push('');
  L.push('#ifdef __cplusplus');
  L.push('extern "C" {');
  L.push('#endif');
  L.push('');
  L.push('#define JOY_API_VTABLE_ABI 1u');
  L.push('');
  L.push('/* 不透明句柄的强类型薄包装：jr2d_p → Jr2d。');
  L.push(' * Jr2d / Font / Jwindow 在 C 里互不兼容，编译期即可拦住混传。 */');
  for (const h of handles) {
    L.push('typedef struct ' + h + ' { uint64_t h; } ' + h + ';');
  }
  L.push('');
  L.push('/* C99 编译期断言：句柄必须只占一个 uint64_t，不能改变 ABI。 */');
  for (const h of handles) {
    L.push('typedef char joy_vtable_size_' + h
      + '[sizeof(' + h + ') == sizeof(uint64_t) ? 1 : -1];');
  }
  L.push('');
  L.push('/* 头部三字段固定：DLL 侧校验 abi_version 与 size 即可前后兼容加 API。 */');
  L.push('typedef struct JoyApi {');
  L.push('        uint32_t abi_version;');
  L.push('        uint32_t size;');
  L.push('        void*    ctx;');
  for (const e of open) {
    L.push('        ' + fnPtr(e) + ';');
  }
  L.push('} JoyApi;');
  L.push('');
  L.push('/* 宿主侧填表；返回 0 成功。DLL 侧只需校验 abi_version / size。 */');
  L.push('int joy_api_vtable_init(JoyApi* api, void* ctx);');
  L.push('');
  L.push('/* 自检：表结构 + 确定性纯函数的强签名调用与直接 C 调用比对。');
  L.push(' * 返回 NULL 表示通过；不做任何 create/destroy 类副作用调用。 */');
  L.push('const char* joy_api_vtable_selftest(void);');
  L.push('');
  L.push('#ifdef __cplusplus');
  L.push('}');
  L.push('#endif');
  L.push('#endif /* JOY_API_VTABLE_H */');
  return L.join('\n');
}

function emitShim(e) {
  const ret = cType(e.ret);
  const params = paramsOf(e);
  const n = params.length;
  const L = [];
  L.push('static ' + ret + ' ' + shimName(e) + '(' + paramDecl(e) + ')');
  L.push('{');
  L.push(n ? '        joy_value_t ' + P + 'args[' + n + '];'
           : '        joy_value_t* ' + P + 'args = NULL;');
  L.push('        joy_value_t ' + P + 'r;');
  L.push('        joy_status_t ' + P + 'st = JOY_OK;');
  if (okHandle(e.ret)) L.push('        ' + ret + ' ' + P + 'out;');
  L.push('        (void)' + P + 'ctx;');
  for (let i = 0; i < n; i++) {
    const p = params[i];
    const pack = okHandle(p.ctype)
      ? 'joy_v_handle(' + p.name + '.h)'
      : SCALAR[p.ctype].wrap + '(' + p.name + ')';
    L.push('        ' + P + 'args[' + i + '] = ' + pack + ';');
  }
  L.push('        ' + P + 'r = joy_api_call(' + q(e.name) + ', '
    + P + 'args, ' + n + ', &' + P + 'st);');
  if (e.ret === 'void') {
    L.push('        (void)' + P + 'r;');
    L.push('        (void)' + P + 'st;');
    L.push('        return;');
  } else if (okHandle(e.ret)) {
    L.push('        ' + P + 'out.h = (' + P + 'st == JOY_OK && '
      + P + 'r.kind == JOY_V_HANDLE) ? ' + P + 'r.as.handle : 0;');
    L.push('        return ' + P + 'out;');
  } else {
    const ti = SCALAR[e.ret];
    L.push('        if (' + P + 'st != JOY_OK || ' + P + 'r.kind != '
      + ti.kind + ') return (' + ret + ')0;');
    L.push('        return ' + ti.cast + ' ' + P + 'r.' + ti.acc + ';');
  }
  L.push('}');
  return L.join('\n');
}

function emitInit(open) {
  const L = [];
  L.push('int joy_api_vtable_init(JoyApi* api, void* ctx)');
  L.push('{');
  L.push('        if (!api) return -1;');
  L.push('        memset(api, 0, sizeof(*api));');
  L.push('        api->abi_version = JOY_API_VTABLE_ABI;');
  L.push('        api->size = (uint32_t)sizeof(JoyApi);');
  L.push('        api->ctx = ctx;');
  for (const e of open) {
    L.push('        api->' + methodName(e.name) + ' = ' + shimName(e) + ';');
  }
  L.push('        return 0;');
  L.push('}');
  return L.join('\n');
}

function emitSelftest(open) {
  const L = [];
  L.push('const char* joy_api_vtable_selftest(void)');
  L.push('{');
  L.push('        JoyApi api;');
  L.push('        if (joy_api_vtable_init(&api, NULL) != 0) return "init";');
  L.push('        if (api.abi_version != JOY_API_VTABLE_ABI) return "abi_version";');
  L.push('        if (api.size != sizeof(JoyApi)) return "size";');
  for (const e of open.filter(x => SELFTEST_SAFE.has(x.name))) {
    const m = methodName(e.name);
    const args = paramsOf(e).map(sampleExpr).join(', ');
    const ret = cType(e.ret);
    L.push('        if (!api.' + m + ') return ' + q('missing shim: ' + m) + ';');
    L.push('        {');
    L.push('                ' + ret + ' direct = ' + e.name + '(' + args + ');');
    L.push('                ' + ret + ' via = api.' + m + '(api.ctx, ' + args + ');');
    L.push('                if (direct != via) return ' + q(e.name + ' mismatch') + ';');
    L.push('        }');
  }
  const probe = open.find(e => e.name === HANDLE_PROBE && paramsOf(e).length === 1
    && okHandle(paramsOf(e)[0].ctype) && !okHandle(e.ret) && e.ret !== 'void');
  if (probe) {
    const m = methodName(probe.name);
    L.push('        /* 强句柄链路：传 h=0，thunk 的 joy_obj_deref 必须拒绝，');
    L.push('         * shim 应翻译成零值而不是崩溃（不触碰任何真实对象）。 */');
    L.push('        if (!api.' + m + ') return ' + q('missing shim: ' + m) + ';');
    L.push('        {');
    L.push('                ' + handleClass(paramsOf(probe)[0].ctype) + ' ' + P + 'bad = {0};');
    L.push('                if (api.' + m + '(api.ctx, ' + P + 'bad)) return "invalid handle accepted";');
    L.push('        }');
  }
  L.push('        return NULL;');
  L.push('}');
  return L.join('\n');
}

function buildVtableC(entries) {
  const open = entries.filter(canVtable);
  const L = BANNER.slice();
  L.push('#include <string.h>');
  L.push('#include <joy/joy.h>');
  L.push('#include "generated/joy_api_vtable.h"');
  L.push('#include "runtime/joy_value.h"');
  L.push('#include "runtime/joy_thunk.h"');
  L.push('');
  L.push('/* 宿主侧实现：DLL 侧只 include .h，不链接本文件。 */');
  L.push('');
  for (const e of open) {
    L.push(emitShim(e));
    L.push('');
  }
  L.push(emitInit(open));
  L.push('');
  L.push(emitSelftest(open));
  return L.join('\n');
}

function buildVtable(entries) {
  return { h: buildVtableHeader(entries), c: buildVtableC(entries) };
}

module.exports = { buildVtable, canVtable };
