/*
 * P1 thunk 生成器（纯字符串函数，不写文件）
 *
 * 输入：api/joy.idl 里 status:on 的条目（只支持 L0 / L1）
 * 输出：api/thunks_generated.c 的完整文本
 *
 * 生成的每个 thunk 都是「joy_value[] 进、joy_value 出」的纯 C 函数，
 * 不含任何 VM 类型；句柄由 joy_obj 管理，析构由 IDL 的 owns 驱动。
 */
'use strict';

const SCALAR = {
  'bool':          { kind: 'JOY_V_BOOL', acc: 'as.b',    cast: '(bool)',           wrap: 'joy_v_bool', sample: '1' },
  'char':          { kind: 'JOY_V_I32',  acc: 'as.i32',  cast: '(char)',           wrap: 'joy_v_i32',  sample: '0' },
  'int':           { kind: 'JOY_V_I32',  acc: 'as.i32',  cast: '(int)',            wrap: 'joy_v_i32',  sample: '0' },
  'int32_t':       { kind: 'JOY_V_I32',  acc: 'as.i32',  cast: '(int32_t)',        wrap: 'joy_v_i32',  sample: '0' },
  'uint8_t':       { kind: 'JOY_V_U32',  acc: 'as.u32',  cast: '(uint8_t)',        wrap: 'joy_v_u32',  sample: '0' },
  'uint16_t':      { kind: 'JOY_V_U32',  acc: 'as.u32',  cast: '(uint16_t)',       wrap: 'joy_v_u32',  sample: '0' },
  'uint32_t':      { kind: 'JOY_V_U32',  acc: 'as.u32',  cast: '(uint32_t)',       wrap: 'joy_v_u32',  sample: '0' },
  'unsigned int':  { kind: 'JOY_V_U32',  acc: 'as.u32',  cast: '(unsigned int)',   wrap: 'joy_v_u32',  sample: '0' },
  'int64_t':       { kind: 'JOY_V_I64',  acc: 'as.i64',  cast: '(int64_t)',        wrap: 'joy_v_i64',  sample: '0' },
  'uint64_t':      { kind: 'JOY_V_U64',  acc: 'as.u64',  cast: '(uint64_t)',       wrap: 'joy_v_u64',  sample: '12345' },
  'size_t':        { kind: 'JOY_V_U64',  acc: 'as.u64',  cast: '(size_t)',         wrap: 'joy_v_u64',  sample: '0' },
  'unsigned long': { kind: 'JOY_V_U64',  acc: 'as.u64',  cast: '(unsigned long)',  wrap: 'joy_v_u64',  sample: '0' },
  'float':         { kind: 'JOY_V_F64',  acc: 'as.f64',  cast: '(float)',          wrap: 'joy_v_f64',  sample: '0.0' },
  'double':        { kind: 'JOY_V_F64',  acc: 'as.f64',  cast: '(double)',         wrap: 'joy_v_f64',  sample: '0.0' },
  'char*':         { kind: 'JOY_V_STR',  acc: 'as.str',  cast: '(const char*)',    wrap: 'joy_v_str',  sample: '""' },
};

function isOpaque(t) {
  return t === 'void*' || (t.endsWith('_p') && !t.includes('*'));
}
function typeInfo(t) { return SCALAR[t] || null; }
function q(s) { return '"' + s + '"'; }

function emitThunk(e, dtorName) {
  const L = [];
  const n = e.params.length;
  const retHandle = isOpaque(e.ret);
  const retInfo = e.ret === 'void' ? null : typeInfo(e.ret);
  const args = e.params.map((p, i) => 'p' + i).join(', ');

  L.push('static joy_value_t t_' + e.name + '(joy_value_t* a, int n, joy_status_t* st)');
  L.push('{');
  e.params.forEach((p, i) => L.push('        ' + p.ctype + ' p' + i + ';'));
  if (e.ret !== 'void') L.push('        ' + e.ret + ' _r;');
  if (retHandle) L.push('        uint64_t _h;');
  L.push('        if (n != ' + n + ') return joy_v_err(st, JOY_ERR_ARGC, ' + q(e.name) + ');');
  e.params.forEach((p, i) => {
    const kind = isOpaque(p.ctype) ? 'JOY_V_HANDLE' : typeInfo(p.ctype).kind;
    L.push('        if (a[' + i + '].kind != ' + kind + ')'
      + ' return joy_v_err(st, JOY_ERR_TYPE, ' + q(e.name + '.arg' + i) + ');');
  });
  e.params.forEach((p, i) => {
    if (isOpaque(p.ctype)) {
      L.push('        p' + i + ' = (' + p.ctype + ')joy_obj_deref(a[' + i + '].as.handle, '
        + q(p.ctype) + ', st);');
      L.push('        if (!p' + i + ') return joy_v_err(st, *st, '
        + q(e.name + '.arg' + i) + ');');
    } else {
      L.push('        p' + i + ' = ' + typeInfo(p.ctype).cast + 'a[' + i + '].'
        + typeInfo(p.ctype).acc + ';');
    }
  });
  if (e.ret === 'void') {
    L.push('        ' + e.name + '(' + args + ');');
    L.push('        return joy_v_null();');
  } else {
    L.push('        _r = ' + e.name + '(' + args + ');');
    if (retHandle) {
      L.push('        if (!_r) return joy_v_err(st, JOY_ERR_CREATE, ' + q(e.name) + ');');
      L.push('        _h = joy_obj_register((void*)_r, ' + q(e.ret) + ', '
        + dtorName + ', ' + q(e.name) + ');');
      L.push('        if (!_h) { ' + e.owns + '(_r);'
        + ' return joy_v_err(st, JOY_ERR_CAPACITY, ' + q(e.name) + '); }');
      L.push('        return joy_v_handle(_h);');
    } else {
      L.push('        return ' + retInfo.wrap + '(' + retInfo.cast + '_r);');
    }
  }
  L.push('}');
  L.push('');
  return L.join('\n');
}

function emitSelftest(open) {
  const probe = open.find(e => e.level === 'L0' && e.params.length === 0
    && e.ret !== 'void' && typeInfo(e.ret));
  const create = open.find(e => e.owns && isOpaque(e.ret) && e.params.length <= 1
    && e.params.every(p => typeInfo(p.ctype)));
  const handleArg = create ? open.find(e => e.params.length === 1
    && e.params[0].ctype === create.ret && e.ret !== 'void' && typeInfo(e.ret)) : null;

  const L = [];
  L.push('const char* joy_api_selftest(void)');
  L.push('{');
  L.push('        joy_status_t st = JOY_OK;');
  L.push('        uint32_t live0 = joy_obj_live_count();');
  L.push('        uint32_t dead0 = joy_obj_destroy_count();');
  L.push('        joy_value_t r;');
  L.push('        joy_value_t args[4];');
  L.push('');
  if (probe) {
    const pi = typeInfo(probe.ret);
    L.push('        /* L0：纯标量往返 */');
    L.push('        st = JOY_OK;');
    L.push('        r = t_' + probe.name + '(NULL, 0, &st);');
    L.push('        if (st != JOY_OK || r.kind != ' + pi.kind + ') return ' + q('L0/' + probe.name) + ';');
    L.push('        st = JOY_OK;');
    L.push('        t_' + probe.name + '(args, 1, &st);');
    L.push('        if (st != JOY_ERR_ARGC) return "argc-check";');
    L.push('');
  }
  if (create) {
    L.push('        /* L1：create -> 句柄 -> 类型校验 -> 自动析构 */');
    L.push('        st = JOY_OK;');
    create.params.forEach((p, i) => {
      const ti = typeInfo(p.ctype);
      L.push('        args[' + i + '] = ' + ti.wrap + '(' + ti.cast + ti.sample + ');');
    });
    L.push('        r = t_' + create.name + '(args, ' + create.params.length + ', &st);');
    L.push('        if (st != JOY_OK || r.kind != JOY_V_HANDLE) return ' + q('L1/' + create.name) + ';');
    L.push('        if (joy_obj_live_count() != live0 + 1) return "live-after-create";');
    L.push('');
    if (handleArg) {
      const hi = typeInfo(handleArg.ret);
      L.push('        st = JOY_OK;');
      L.push('        {');
      L.push('                joy_value_t h = t_' + handleArg.name + '(&r, 1, &st);');
      L.push('                if (st != JOY_OK || h.kind != ' + hi.kind + ') return "handle-arg";');
      L.push('        }');
      L.push('');
    }
    L.push('        st = JOY_OK;');
    L.push('        if (joy_obj_deref(r.as.handle, "__wrong_tag__", &st) != NULL || st == JOY_OK)');
    L.push('                return "type-tag-check";');
    L.push('');
    L.push('        joy_obj_release(r.as.handle);');
    L.push('        if (joy_obj_destroy_count() != dead0 + 1) return "auto-destroy";');
    L.push('        if (joy_obj_live_count() != live0) return "live-after-release";');
    L.push('        st = JOY_OK;');
    L.push('        if (joy_obj_deref(r.as.handle, ' + q(create.ret) + ', &st) != NULL)');
    L.push('                return "stale-handle";');
    L.push('');
  }
  if (!probe && !create) {
    L.push('        /* 当前没有可自检的 L0 / L1 条目；开放后有内容会自动生成。 */');
  }
  L.push('        return NULL;');
  L.push('}');
  return L.join('\n');
}

function buildThunks(entries) {
  const open = entries.filter(e => e.status === 'on' && !e.orphan);
  const bad = open.filter(e => e.level !== 'L0' && e.level !== 'L1');
  if (bad.length) {
    throw new Error('P1 只支持 L0/L1，以下条目不能 status:on：'
      + bad.slice(0, 5).map(e => e.name + '(' + e.level + ')').join(', '));
  }
  const byName = new Map(entries.map(e => [e.name, e]));
  const owners = [...new Set(open.filter(e => isOpaque(e.ret)).map(e => e.owns))];
  for (const o of owners) {
    const f = byName.get(o);
    if (!f || !f.params || f.params.length !== 1 || !isOpaque(f.params[0].ctype)) {
      throw new Error('owns 目标签名不符合单指针析构：' + o);
    }
  }

  const L = [];
  L.push('/* ============================================================');
  L.push(' * 自动生成，勿手改：node api/gen_bindings.js --emit');
  L.push(' * 只有 joy.idl 里 status:on 的 L0/L1 条目会出现在这里。');
  L.push(' * ============================================================ */');
  L.push('#include <joy/joy.h>');
  L.push('#include "joy_api.h"');
  L.push('#include "runtime/joy_value.h"');
  L.push('#include "runtime/joy_obj.h"');
  L.push('#include "runtime/joy_thunk.h"');
  L.push('');
  for (const o of owners) {
    const f = byName.get(o);
    L.push('static void dtor_' + o + '(void* p) { ' + o + '((' + f.params[0].ctype + ')p); }');
  }
  if (owners.length) L.push('');

  for (const e of open) {
    const dtorName = isOpaque(e.ret) ? 'dtor_' + e.owns : null;
    L.push(emitThunk(e, dtorName));
  }

  L.push('static const joy_thunk_entry s_thunks[] = {');
  for (const e of open) {
    L.push('        { ' + q(e.domain) + ', ' + q(e.name) + ', t_' + e.name + ', '
      + e.params.length + ' },');
  }
  L.push('};');
  L.push('');
  L.push('const joy_thunk_entry* joy_thunks_table(int* count)');
  L.push('{');
  L.push('        if (count) *count = (int)(sizeof(s_thunks) / sizeof(s_thunks[0]));');
  L.push('        return s_thunks;');
  L.push('}');
  L.push('');
  L.push(emitSelftest(open));
  L.push('');
  return L.join('\n');
}

module.exports = { buildThunks };
