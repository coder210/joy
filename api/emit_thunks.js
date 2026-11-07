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

const { isOpaque, methodName } = require('./naming');
const { collectStructs, canExpand, expandParams } = require('./structs');
const LAYOUTS = collectStructs();
function typeInfo(t) { return SCALAR[t] || null; }
function valueKind(t) {
  if (isOpaque(t)) return 'JOY_V_HANDLE';
  const ti = typeInfo(t);
  return ti ? ti.kind : 'JOY_V_NULL';
}
function retKind(e) { return e.ret === 'void' ? 'JOY_V_NULL' : valueKind(e.ret); }
function q(s) { return '"' + s + '"'; }

/* 所有权校验：dtor 的参数类型必须与 create 的返回类型严格一致。
 * 只看「是不是指针」不够：owns=r2d_destroy 配 ret=void* 会把错误类型的
 * 指针交给析构函数，运行时才崩，必须在生成阶段拦住。 */
function ownsIssue(e, byName) {
  if (!isOpaque(e.ret)) return null;
  if (!e.owns) return '返回句柄但缺 owns';
  const f = byName.get(e.owns);
  if (!f) return 'owns 指向不存在的函数：' + e.owns;
  if (!f.params || f.params.length !== 1) return 'owns 目标不是单参数：' + e.owns;
  if (!isOpaque(f.params[0].ctype)) return 'owns 目标参数不是句柄：' + e.owns;
  if (f.params[0].ctype !== e.ret) return 'owns 类型不匹配：' + e.ret + ' -> ' + f.params[0].ctype;
  return null;
}

/* --enable 用：这条 API 现在能不能安全地自动生成 thunk。 */
function canThunk(e, byName) {
  const l2 = e.level === 'L2' && canExpand(e, LAYOUTS);
  if (e.level !== 'L0' && e.level !== 'L1' && !l2) return '仅 L0/L1';
  if (!e.params) return '复杂签名需手写 thunk';
  if (l2 && isOpaque(e.ret)) return 'L2 试点暂不支持返回句柄';
  const own = ownsIssue(e, byName);
  if (own) return own;
  if (e.ret !== 'void' && !isOpaque(e.ret) && !typeInfo(e.ret)) return '不支持返回类型：' + e.ret;
  for (const p of expandParams(e, LAYOUTS)) {
    if (!isOpaque(p.ctype) && !typeInfo(p.ctype)) return '不支持参数类型：' + p.ctype;
    if (p.ctype === 'char*' && !p.isConst) return '非 const char* 输出缓冲';
  }
  return null;
}

function emitThunk(e, dtorName) {
  const L = [];
  const n = e.params.length;
  const retHandle = isOpaque(e.ret);
  const retInfo = e.ret === 'void' ? null : typeInfo(e.ret);
  const args = e.params.map((p, i) => 'p' + i).join(', ');

  L.push('static joy_value_t t_' + e.name + '(joy_value_t* a, int n, joy_status_t* st)');
  L.push('{');
  e.params.forEach((p, i) => L.push('        '
    + (p.ctype === 'char*' ? 'const char*' : p.ctype) + ' p' + i + ';'));
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

/* L2 试点：结构体按值参数展开成「按字段顺序排列的标量序列」。
 * 注册表 argc / arg_kinds 用展开后的序列，因此 lua / js / cs 三端适配层
 * 无需改动；thunk 里把字段重新组装成 C 结构体再调用真正的 joy API。 */
function emitL2Thunk(e, layouts) {
  const L = [];
  const flat = expandParams(e, layouts);
  const groups = [];
  let idx = 0;
  for (let i = 0; i < e.params.length; i++) {
    const p = e.params[i];
    const f = layouts.has(p.ctype) ? layouts.get(p.ctype) : null;
    groups.push({ p, i, f, start: idx });
    idx += f ? f.length : 1;
  }
  const args = e.params.map((p, i) => 'p' + i).join(', ');
  L.push('static joy_value_t t_' + e.name + '(joy_value_t* a, int n, joy_status_t* st)');
  L.push('{');
  for (const g of groups) {
    const decl = g.p.ctype === 'char*' ? 'const char*' : g.p.ctype;
    L.push('        ' + decl + ' p' + g.i + (g.f ? ' = { 0 }' : '') + ';');
  }
  if (e.ret !== 'void') L.push('        ' + e.ret + ' _r;');
  L.push('        if (n != ' + flat.length + ') return joy_v_err(st, JOY_ERR_ARGC, ' + q(e.name) + ');');
  for (const g of groups) {
    if (g.f) {
      g.f.forEach((fld, k) => {
        const j = g.start + k;
        const ti = typeInfo(fld.ctype);
        L.push('        if (a[' + j + '].kind != ' + ti.kind + ')'
          + ' return joy_v_err(st, JOY_ERR_TYPE, ' + q(e.name + '.arg' + j) + ');');
        L.push('        p' + g.i + '.' + fld.name + ' = ' + ti.cast + 'a[' + j + '].' + ti.acc + ';');
      });
    } else {
      const p = g.p;
      const j = g.start;
      const kind = isOpaque(p.ctype) ? 'JOY_V_HANDLE' : typeInfo(p.ctype).kind;
      L.push('        if (a[' + j + '].kind != ' + kind + ')'
        + ' return joy_v_err(st, JOY_ERR_TYPE, ' + q(e.name + '.arg' + j) + ');');
      if (isOpaque(p.ctype)) {
        L.push('        p' + g.i + ' = (' + p.ctype + ')joy_obj_deref(a[' + j + '].as.handle, '
          + q(p.ctype) + ', st);');
        L.push('        if (!p' + g.i + ') return joy_v_err(st, *st, '
          + q(e.name + '.arg' + j) + ');');
      } else {
        const ti = typeInfo(p.ctype);
        L.push('        p' + g.i + ' = ' + ti.cast + 'a[' + j + '].' + ti.acc + ';');
      }
    }
  }
  if (e.ret === 'void') {
    L.push('        ' + e.name + '(' + args + ');');
    L.push('        return joy_v_null();');
  } else {
    L.push('        _r = ' + e.name + '(' + args + ');');
    const ti = typeInfo(e.ret);
    L.push('        return ' + ti.wrap + '(' + ti.cast + '_r);');
  }
  L.push('}');
  L.push('');
  return L.join('\n');
}

function emitSelftest(open, layouts) {
  const probe = open.find(e => e.level === 'L0' && e.params.length === 0
    && e.ret !== 'void' && typeInfo(e.ret));
  const create = open.find(e => e.owns && isOpaque(e.ret) && e.params.length <= 1
    && e.params.every(p => typeInfo(p.ctype)));
  const handleArg = create ? open.find(e => e.params.length === 1
    && e.params[0].ctype === create.ret && e.ret !== 'void' && typeInfo(e.ret)) : null;

  const l2 = open.filter(e => e.level === 'L2');
  const maxArgs = Math.max(4, ...open.map(e => expandParams(e, layouts).length));
  const L = [];
  L.push('const char* joy_api_selftest(void)');
  L.push('{');
  L.push('        joy_status_t st = JOY_OK;');
  L.push('        uint32_t live0 = joy_obj_live_count();');
  L.push('        uint32_t dead0 = joy_obj_destroy_count();');
  L.push('        joy_value_t r;');
  L.push('        joy_value_t args[' + maxArgs + '];');
  if (l2.length) L.push('        int i;');
  L.push('');
  L.push('        /* 每条开放的 API 都必须在注册表里且 argc 守卫生效。');
  L.push('         * 故意传错 argc：thunk 第一行就返回，不执行任何 joy 业务函数，');
  L.push('         * 因此批量开放有副作用 / 需要上下文的 API 也不会在这里被调用。 */');
  for (const e of open) {
    L.push('        st = JOY_OK;');
    L.push('        joy_api_call(' + q(e.name) + ', NULL, '
      + (expandParams(e, layouts).length + 1) + ', &st);');
    L.push('        if (st != JOY_ERR_ARGC) return ' + q('registry/' + e.name) + ';');
  }
  if (l2.length) {
    L.push('');
    L.push('        /* L2 试点：用展开后的参数个数调用，args 全为 NULL 值，');
    L.push('         * thunk 在类型校验处返回，不会执行任何 joy 业务函数。 */');
    L.push('        for (i = 0; i < ' + maxArgs + '; i++) args[i] = joy_v_null();');
    for (const e of l2) {
      L.push('        st = JOY_OK;');
      L.push('        joy_api_call(' + q(e.name) + ', args, '
        + expandParams(e, layouts).length + ', &st);');
      L.push('        if (st != JOY_ERR_TYPE) return ' + q('L2/' + e.name) + ';');
    }
  }
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
  const layouts = LAYOUTS;
  const open = entries.filter(e => e.status === 'on' && !e.orphan);
  const bad = open.filter(e => e.level !== 'L0' && e.level !== 'L1'
    && !(e.level === 'L2' && canExpand(e, layouts)));
  if (bad.length) {
    throw new Error('只支持 L0/L1 及可展开的 L2，以下条目不能 status:on：'
      + bad.slice(0, 5).map(e => e.name + '(' + e.level + ')').join(', '));
  }
  const byName = new Map(entries.map(e => [e.name, e]));
  for (const e of open) {
    if (isOpaque(e.ret)) {
      const issue = ownsIssue(e, byName);
      if (issue) throw new Error(e.name + ': ' + issue);
    }
  }
  const owners = [...new Set(open.filter(e => isOpaque(e.ret)).map(e => e.owns))];

  const L = [];
  L.push('/* ============================================================');
  L.push(' * 自动生成，勿手改：node api/gen_bindings.js --emit');
  L.push(' * joy.idl 里 status:on 的 L0/L1，以及可展开的 L2 试点条目会出现在这里。');
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
    L.push(e.level === 'L2' ? emitL2Thunk(e, layouts) : emitThunk(e, dtorName));
  }

  for (const e of open) {
    const flat = expandParams(e, layouts);
    if (flat.length) {
      L.push('static const joy_value_kind k_' + e.name + '[] = { '
        + flat.map(p => valueKind(p.ctype)).join(', ') + ' };');
    }
  }
  L.push('');
  L.push('static const joy_thunk_entry s_thunks[] = {');
  for (const e of open) {
    const flat = expandParams(e, layouts);
    L.push('        { ' + q(e.domain) + ', ' + q(e.name) + ', ' + q(methodName(e.name))
      + ', t_' + e.name + ', ' + flat.length + ', '
      + (flat.length ? 'k_' + e.name : 'NULL') + ', ' + retKind(e) + ' },');
  }
  L.push('};');
  L.push('');
  L.push('const joy_thunk_entry* joy_thunks_table(int* count)');
  L.push('{');
  L.push('        if (count) *count = (int)(sizeof(s_thunks) / sizeof(s_thunks[0]));');
  L.push('        return s_thunks;');
  L.push('}');
  L.push('');
  L.push(emitSelftest(open, layouts));
  L.push('');
  return L.join('\n');
}

module.exports = { buildThunks, canThunk, ownsIssue };
