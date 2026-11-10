/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: joy.h
Description: 聚合门面，只做 include，不含任何声明。
*************************************************/
#ifndef JOY_H
#define JOY_H

#include <joy/defs.h>
#include <joy/types.h>
#include <joy/api.h>

        /* jmath.h 的全标量包装声明是工具生成物，独立成头；主头只留这一行 include。 */
        #include "../../api/generated/jmath_wrappers.h"

        /* 命名迁移期旧名兼容层；P0 为空骨架，见 api/README.md。 */
        #include <joy/compat.h>

#endif // !JOY_H
