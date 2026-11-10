/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: compat.h
Description: 公开 API 命名迁移期的旧名兼容层（已废弃）。
*************************************************/
#ifndef JOY_COMPAT_H
#define JOY_COMPAT_H

/*
 * 2026-11 P1 命名收尾：所有旧名已在 src/ 与各模块实现里直接改成
 * joy_<域>_<动作> 形式，不再需要 #define 桥接，本文件留空以防 joy.h 的
 * #include 失效。若日后需重新开放旧名兼容，在下方补 #define 即可。
 */

#endif /* JOY_COMPAT_H */
