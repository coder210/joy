/* 公共 API 声明：IDL / thunk 生成器的聚合入口
 * 各域声明的归属地是 src/joy_<域>.h，本头只做 include 聚合，方向不可反转。
 * 每个域头自带 guard、基础类型头与 extern "C"，可单独被本域 .c 包含。 */
#ifndef JOY_API_H
#define JOY_API_H

#include <joy/jtypes.h>

/* 顺序即依赖顺序：先被 include 的域头其类型可供后续域头使用。 */
#include "../../src/joy_core.h"
#include "../../src/joy_math.h"
#include "../../src/joy_r2d.h"
#include "../../src/joy_r3d.h"
#include "../../src/joy_ui.h"
#include "../../src/joy_scene.h"
#include "../../src/joy_net.h"
#include "../../src/joy_script.h"
#include "../../src/joy_audio.h"

#endif /* JOY_API_H */
