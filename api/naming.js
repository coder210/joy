/*
 * 三端共用的命名与类型判定规则。
 *
 * 同一份 joy.idl 必须生成同一套脚本层方法名 / 对象名，否则
 * lua / js / cs 三个后端又会各自长出一套方言。后端生成器一律从这里取规则，
 * 不要在自己文件里另写一份 pascal() / isOpaque()。
 */
'use strict';

function isOpaque(t) {
  return t === 'void*' || (t.endsWith('_p') && !t.includes('*'));
}

function pascal(s) {
  return s.split('_').filter(Boolean).map(w => w[0].toUpperCase() + w.slice(1)).join('');
}

/* 脚本层方法名：三个后端统一用 PascalCase，参数顺序与 joy.h 一致。 */
function methodName(name) { return pascal(name); }

/* 句柄对象的强类型类名：lcg_num_p → LcgNum。 */
function handleClass(ret) { return pascal(ret.replace(/_p$/, '')); }

module.exports = { isOpaque, pascal, methodName, handleClass };
