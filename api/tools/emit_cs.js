/*
 * cs 后端薄适配生成器（纯字符串函数，不写文件）
 *
 * 产出两个入库生成物：
 *   modules/cs/bindings/api/api_generated.c   Mono 类型 → joy_value[] → 共享 thunk
 *   modules/cs/scripts/Api.generated.cs   强类型 C# 包装（脚本层看不到 IntPtr / 裸句柄）
 *
 * 这里没有任何 joy API 的业务逻辑：参数校验、句柄、所有权全在共享 thunk 里，
 * 本文件只做「Mono 值 ↔ joy_value」的翻译。
 */
'use strict';

const MAP = {
  'bool':          { cs: 'bool',   c: 'int32_t',  kind: 'JOY_V_BOOL', acc: 'as.b',    wrap: 'joy_v_bool', cast: '(int32_t)' },
  'int':           { cs: 'int',    c: 'int32_t',  kind: 'JOY_V_I32',  acc: 'as.i32',  wrap: 'joy_v_i32',  cast: '(int32_t)' },
  'int32_t':       { cs: 'int',    c: 'int32_t',  kind: 'JOY_V_I32',  acc: 'as.i32',  wrap: 'joy_v_i32',  cast: '(int32_t)' },
  'uint8_t':       { cs: 'uint',   c: 'uint32_t', kind: 'JOY_V_U32',  acc: 'as.u32',  wrap: 'joy_v_u32',  cast: '(uint32_t)' },
  'uint16_t':      { cs: 'uint',   c: 'uint32_t', kind: 'JOY_V_U32',  acc: 'as.u32',  wrap: 'joy_v_u32',  cast: '(uint32_t)' },
  'uint32_t':      { cs: 'uint',   c: 'uint32_t', kind: 'JOY_V_U32',  acc: 'as.u32',  wrap: 'joy_v_u32',  cast: '(uint32_t)' },
  'unsigned int':  { cs: 'uint',   c: 'uint32_t', kind: 'JOY_V_U32',  acc: 'as.u32',  wrap: 'joy_v_u32',  cast: '(uint32_t)' },
  'int64_t':       { cs: 'long',   c: 'int64_t',  kind: 'JOY_V_I64',  acc: 'as.i64',  wrap: 'joy_v_i64',  cast: '(int64_t)' },
  'uint64_t':      { cs: 'ulong',  c: 'uint64_t', kind: 'JOY_V_U64',  acc: 'as.u64',  wrap: 'joy_v_u64',  cast: '(uint64_t)' },
  'size_t':        { cs: 'ulong',  c: 'uint64_t', kind: 'JOY_V_U64',  acc: 'as.u64',  wrap: 'joy_v_u64',  cast: '(uint64_t)' },
  'unsigned long': { cs: 'ulong',  c: 'uint64_t', kind: 'JOY_V_U64',  acc: 'as.u64',  wrap: 'joy_v_u64',  cast: '(uint64_t)' },
  'float':         { cs: 'float',  c: 'float',    kind: 'JOY_V_F64',  acc: 'as.f64',  wrap: 'joy_v_f64',  cast: '(double)' },
  'double':        { cs: 'double', c: 'double',   kind: 'JOY_V_F64',  acc: 'as.f64',  wrap: 'joy_v_f64',  cast: '(double)' },
  /* const char* 入参：MonoString → UTF-8 → joy_v_str，调用返回后 mono_free。
   * 返回 char* 的 API 不走这里：所有权（谁 free）必须人工标注。 */
  'char*':         { cs: 'string', c: 'MonoString*', kind: 'JOY_V_STR', acc: 'as.str',
                     wrap: 'joy_v_str', cast: '(const char*)', str: true },
};

const { isOpaque, methodName, handleClass } = require('./naming');
const { collectStructs, canExpand, expandParams } = require('./structs');
const LAYOUTS = collectStructs();
function paramsOf(e) { return expandParams(e, LAYOUTS); }
function q(s) { return '"' + s + '"'; }

function infoOf(t) {
  if (isOpaque(t)) {
    return { cs: 'ulong', c: 'uint64_t', kind: 'JOY_V_HANDLE', acc: 'as.handle',
      wrap: 'joy_v_handle', cast: '(uint64_t)', handle: true, cls: handleClass(t) };
  }
  const m = MAP[t];
  if (!m) throw new Error('cs 适配暂不支持类型：' + t + '（结构体 / 数组需手写）');
  return m;
}

/* --enable 用：cs 后端能否为这条 API 生成包装。 */
function canEmitCs(e) {
  const l2 = e.level === 'L2' && canExpand(e, LAYOUTS);
  if (e.level !== 'L0' && e.level !== 'L1' && !l2) return '仅 L0/L1';
  if (!e.params) return '复杂签名需手写适配';
  if (e.ret === 'char*') return '字符串返回需人工标注所有权';
  if (e.ret !== 'void' && !isOpaque(e.ret) && !MAP[e.ret]) return '不支持返回类型：' + e.ret;
  for (const p of paramsOf(e)) {
    if (!isOpaque(p.ctype) && !MAP[p.ctype]) return '不支持参数类型：' + p.ctype;
  }
  return null;
}

/* ------------------------------------------------------------------- C 侧 */

function emitIcall(e) {
  const L = [];
  const flat = paramsOf(e);
  const n = flat.length;
  const ret = e.ret === 'void' ? null : infoOf(e.ret);
  const mn = methodName(e.name);
  const decl = flat.map((p, i) => infoOf(p.ctype).c + ' p' + i).join(', ');
  const strs = flat.map((p, i) => (p.ctype === 'char*' ? i : -1)).filter(i => i >= 0);

  L.push('static ' + (ret ? ret.c : 'void') + ' icall_' + mn + '(' + (decl || 'void') + ')');
  L.push('{');
  L.push('        joy_status_t st = JOY_OK;');
  if (ret) L.push('        joy_value_t r;');
  if (n) L.push('        joy_value_t args[' + n + '];');
  for (const i of strs) L.push('        char* s' + i + ' = NULL;');
  flat.forEach((p, i) => {
    const ti = infoOf(p.ctype);
    if (ti.str) {
      L.push('        s' + i + ' = p' + i + ' ? mono_string_to_utf8(p' + i + ') : NULL;');
      L.push('        args[' + i + '] = joy_v_str((const char*)s' + i + ');');
    } else {
      L.push('        args[' + i + '] = ' + ti.wrap + '(' + ti.cast + 'p' + i + ');');
    }
  });
  L.push('        ' + (ret ? 'r = ' : '(void)') + 'joy_api_call('
    + q(e.name) + ', ' + (n ? 'args' : 'NULL') + ', ' + n + ', &st);');
  for (const i of strs) L.push('        if (s' + i + ') mono_free(s' + i + ');');
  L.push('        if (st != JOY_OK) {');
  L.push('                fprintf(stderr, "[joy.api] %s failed: %s at %s\\n", '
    + q(e.name) + ', joy_status_name(st), joy_last_error_where());');
  L.push('                return' + (e.ret === 'void' ? ';' : ' 0;'));
  L.push('        }');
  if (!ret) {
    L.push('        return;');
  } else {
    L.push('        return (' + ret.c + ')r.' + ret.acc + ';');
  }
  L.push('}');
  L.push('');
  return L.join('\n');
}

function buildC(open) {
  const L = [];
  L.push('/* ============================================================');
  L.push(' * 自动生成，勿手改：node api/tools/gen_bindings.js --emit');
  L.push(' * cs 后端薄适配：Mono 值 → joy_value[] → 共享 thunk（不含业务逻辑）。');
  L.push(' * joy.idl 里 status:on 的 L0/L1，以及可展开的 L2 试点条目会出现在这里。');
  L.push(' * ============================================================ */');
  L.push('#include <stdint.h>');
  L.push('#include <stdio.h>');
  L.push('#include <mono/metadata/object.h>');
  L.push('#include <mono/metadata/appdomain.h>');
  /* mono_free 只声明在 mono/utils/mono-publib.h（details/mono-publib-functions.h），
   * object.h 不传递它；缺这行时 NDK clang 按 C99 隐式函数声明直接报错。 */
  L.push('#include <mono/utils/mono-publib.h>');
  L.push('#include <joy/joy.h>');
  L.push('#include <joy/api/runtime/joy_value.h>');
  L.push('#include <joy/api/runtime/joy_obj.h>');
  L.push('#include <joy/api/runtime/joy_thunk.h>');
  L.push('#include "joy_csharp.h"');
  L.push('');
  for (const e of open) L.push(emitIcall(e));
  L.push('/* 句柄对象被 GC 时由 C# finalizer 调这里；计数仅供自检。 */');
  L.push('static void icall_release(uint64_t h) { joy_obj_release((uint64_t)h); }');
  L.push('static uint32_t icall_live_count(void) { return joy_obj_live_count(); }');
  L.push('static uint32_t icall_destroy_count(void) { return joy_obj_destroy_count(); }');
  L.push('');
  L.push('void csopen_api(void)');
  L.push('{');
  for (const e of open) {
    L.push('        csx_register_internal_call(' + q('Joy.Api.Native::' + methodName(e.name))
      + ', (const void*)icall_' + methodName(e.name) + ');');
  }
  L.push('        csx_register_internal_call("Joy.Api.Native::Release", (const void*)icall_release);');
  L.push('        csx_register_internal_call("Joy.Api.Native::DebugLiveCount", (const void*)icall_live_count);');
  L.push('        csx_register_internal_call("Joy.Api.Native::DebugDestroyCount", (const void*)icall_destroy_count);');
  L.push('}');
  L.push('');
  return L.join('\n');
}

/* ----------------------------------------------------------------- C# 侧 */

function buildCs(open) {
  const handles = new Set();
  for (const e of open) {
    if (isOpaque(e.ret)) handles.add(e.ret);
    for (const p of e.params) if (isOpaque(p.ctype)) handles.add(p.ctype);
  }

  const L = [];
  L.push('// <auto-generated> node api/tools/gen_bindings.js --emit </auto-generated>');
  L.push('// 强类型包装：脚本层只见对象，不见 IntPtr / 裸句柄。');
  L.push('using System;');
  L.push('using System.Runtime.CompilerServices;');
  L.push('');
  L.push('namespace Joy.Api');
  L.push('{');
  L.push('    internal static class Native');
  L.push('    {');
  for (const e of open) {
    const ret = e.ret === 'void' ? 'void' : infoOf(e.ret).cs;
    const ps = paramsOf(e).map((p, i) => infoOf(p.ctype).cs + ' ' + (p.name || ('arg' + i))).join(', ');
    L.push('        [MethodImpl(MethodImplOptions.InternalCall)]');
    L.push('        internal static extern ' + ret + ' ' + methodName(e.name) + '(' + ps + ');');
  }
  L.push('        [MethodImpl(MethodImplOptions.InternalCall)]');
  L.push('        internal static extern void Release(ulong handle);');
  L.push('        [MethodImpl(MethodImplOptions.InternalCall)]');
  L.push('        internal static extern uint DebugLiveCount();');
  L.push('        [MethodImpl(MethodImplOptions.InternalCall)]');
  L.push('        internal static extern uint DebugDestroyCount();');
  L.push('    }');
  L.push('');
  L.push('    public static class Diagnostics');
  L.push('    {');
  L.push('        public static uint LiveCount { get { return Native.DebugLiveCount(); } }');
  L.push('        public static uint DestroyCount { get { return Native.DebugDestroyCount(); } }');
  L.push('    }');
  L.push('');
  for (const h of [...handles].sort()) {
    const cls = handleClass(h);
    L.push('    public sealed class ' + cls);
    L.push('    {');
    L.push('        internal ulong Handle { get; private set; }');
    L.push('        internal ' + cls + '(ulong handle) { Handle = handle; }');
    L.push('        ~' + cls + '()');
    L.push('        {');
    L.push('            ulong h = Handle;');
    L.push('            Handle = 0;');
    L.push('            if (h != 0) Native.Release(h);');
    L.push('        }');
    L.push('    }');
    L.push('');
  }
  L.push('    public static class Bindings');
  L.push('    {');
  for (const e of open) {
    const mn = methodName(e.name);
    const retHandle = isOpaque(e.ret);
    const pubRet = retHandle ? handleClass(e.ret) : (e.ret === 'void' ? 'void' : infoOf(e.ret).cs);
    const pubParams = paramsOf(e).map((p, i) => {
      const ti = infoOf(p.ctype);
      const nm = p.name || ('arg' + i);
      return (ti.handle ? ti.cls : ti.cs) + ' ' + nm;
    }).join(', ');
    L.push('        public static ' + pubRet + ' ' + mn + '(' + pubParams + ')');
    L.push('        {');
    paramsOf(e).forEach((p, i) => {
      if (isOpaque(p.ctype)) {
        const nm = p.name || ('arg' + i);
        L.push('            if (' + nm + ' == null)'
          + ' throw new ArgumentNullException(nameof(' + nm + '));');
      }
    });
    const callArgs = paramsOf(e).map((p, i) => {
      const nm = p.name || ('arg' + i);
      return isOpaque(p.ctype) ? nm + '.Handle' : nm;
    }).join(', ');
    const call = 'Native.' + mn + '(' + callArgs + ')';
    if (e.ret === 'void') {
      L.push('            ' + call + ';');
    } else if (retHandle) {
      /* 局部名用 __h 而不是 h：参数名可能与 h 撞车（create_window 的高度就是 h），
       * 否则生成的代码报 CS0136「无法在此范围中声明名为 h 的局部变量」。 */
      L.push('            ulong __h = ' + call + ';');
      L.push('            if (__h == 0) throw new InvalidOperationException(' + q(e.name + ' failed') + ');');
      L.push('            return new ' + handleClass(e.ret) + '(__h);');
    } else {
      L.push('            return ' + call + ';');
    }
    L.push('        }');
  }
  L.push('    }');
  L.push('}');
  L.push('');
  return L.join('\n');
}

function buildCsGlue(entries) {
  const open = entries.filter(e => e.status === 'on' && !e.orphan);
  for (const e of open) {
    const why = canEmitCs(e);
    if (why) throw new Error('cs 适配不支持 ' + e.name + '：' + why);
  }
  return { c: buildC(open), cs: buildCs(open) };
}

module.exports = { buildCsGlue, canEmitCs };
