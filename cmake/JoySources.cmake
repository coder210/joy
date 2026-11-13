# ============================================================
# joy 自有源码的显式清单（替代 file(GLOB)）
#
# 这份清单就是架构文档：增删文件必须在这里登记，路径全部相对 JOY_ROOT
# （由 JoyTargets.cmake 设置为本仓库根）。源码按性质分三处，互不混放：
#   src/             人工实现，所有 .c 平铺在一层
#   api/generated/   工具生成，勿手改
#   external/ vendored 第三方（含一个自有包装 aes_util.c）
#
# 下面的分组表达职责分层与模块边界，不只是 IDE 展示：每个 .c 都是独立编译单元，
# 各自持有私有 static 作用域与依赖集，不因"都调用了 jr2d"就合并成巨型 TU。
# 按职责分四层（RENDER 组内部也分层）：
#   绘制基础层    jr2d / jfont / jlight —— 纹理、图元、文字缓存、字体与光照，
#                 提供被上层复用的绘制能力（jlight 复用 jr2d 的图元接口）
#   2D 功能模块层 jparticle / jtilemap / jui / jui_im —— 各自持有独立数据模型
#                 （粒子池 / 瓦片图层 / 控件树 / 即时模式状态），可独立演进
#   场景层        jscene / janimator（SCENE 组）—— 数据驱动为主；
#                 注意 janimator 仍直接调 joy_r2d_image_create/draw_ex 建纹理并绘制，
#                 尚未与渲染层完全解耦（已知边界，非命名问题）
#   可选适配层    jgltf / jspine / jmodule —— 由 JOY_ENABLE_GLTF / _SPINE /
#                 _PLUGINS 控制，默认不参与构建；三个 .c 都整文件由对应的
#                 JOY_USE_GLTF / JOY_USE_SPINE / JOY_USE_PLUGINS 包裹，
#                 保持"未启用即空编译单元"的隔离语义，勿并入必编译的 TU。
#                 JOY_ENABLE_* 由 JoyTargets.cmake 转发成 JOY_USE_*；
#                 japi.h 的 spine / module 声明也放在同名条件段内
# 命名域规则（2026-11-13 起）：一个 .c 只承载一个域的公开接口，函数名前缀
# 必须与该文件的域一致；一个域可以有多个 .c（按子系统拆分，不合并成巨型 TU）。
# 26 个含 joy_* 定义的 .c 已全部纯域，全仓 0 混域。归属：
#   core    jcore / jutils      math   jcollision / jgjk / jbox2d
#   r2d     jr2d / jfont / jparticle / jspine    r3d  jr3d / jlight / jgltf
#   ui      jui / jui_im        scene  jscene / janimator / jtilemap / jecs /
#                                      jprocmap / jbehavior
#   net     jnetwork / jsys     script jmodule / jmsgpack
#   audio   jaudio              input  jinput
# 域归属按技术职责判定：纹理 / 文字 / 图元 / 动画等渲染层功能归 r2d，
# 控件（joystick 等）归 ui，行为树归 scene，glTF 归 r3d。
# 仍存在的边界事项（非命名问题）：
#   - jgltf.c 的部分内部符号在 include/ 里没有对应声明（可选模块边界）
#   - jr3d 与 jgltf 属 3D，勿按"2D"归并
# 改公开 API 的 C 名必须同步 api/idl/rename_map.json（继承 status/owns）与
# api/tools/naming.js 的 ALIASES（冻结脚本层方法名），再跑 --scan / --check。
# 增删文件在这里登记，归属也只记在这一份清单里。
#
# 生成物 api/generated/jmath_wrappers.c 列在 math 域，等 IDL 改 manifest 后
# 由记录类型取代；src/jlut.c 是两张三角查表的唯一定义（原 jlut.h 数据）；
# src/jvendor_impl.c 是 stb/dr 系列全仓唯一实现翻译单元，必须保留。
# ============================================================

set(JOY_SRC_CORE
    src/jcore.c
    src/jinput.c
    src/jsys.c
    src/jecs.c
    src/jutils.c
    src/jprocmap.c
    src/jmsgpack.c
)

set(JOY_SRC_MATH
    src/jlut.c
    api/generated/jmath_wrappers.c
)

set(JOY_SRC_RENDER
    src/jr2d.c
    src/jr3d.c
    src/jfont.c
    src/jlight.c
    src/jparticle.c
    src/jui.c
    src/jui_im.c
    src/jtilemap.c
)

set(JOY_SRC_PHYSICS
    src/jcollision.c
    src/jgjk.c
    src/jbox2d.c
)

set(JOY_SRC_SCENE
    src/jscene.c
    src/janimator.c
)

set(JOY_SRC_BEHAVIOR
    src/jbehavior.c
)

set(JOY_SRC_NET
    src/jnetwork.c
)

set(JOY_SRC_AUDIO
    src/jaudio.c
)

set(JOY_SRC_INTERNAL
    src/jvendor_impl.c
)

# external/ 顶层：vendored 第三方 + 一个自有包装（aes_util.c）。
# 子目录 external/lua|duktape|... 由各脚本后端自行引入，不在这里。
set(JOY_SRC_VENDOR
    external/aes.c
    external/aes_util.c
    external/base64.c
    external/cJSON.c
    external/des.c
    external/http_parser.c
    external/kcp.c
    external/md5.c
    external/mongoose.c
    external/sha1.c
)

set(JOY_ALL_SOURCE_GROUPS
    JOY_SRC_CORE
    JOY_SRC_MATH
    JOY_SRC_RENDER
    JOY_SRC_PHYSICS
    JOY_SRC_SCENE
    JOY_SRC_BEHAVIOR
    JOY_SRC_NET
    JOY_SRC_AUDIO
    JOY_SRC_INTERNAL
    JOY_SRC_VENDOR
)

function(joy_collect_sources OUT)
    set(_all "")
    foreach(_group IN LISTS JOY_ALL_SOURCE_GROUPS)
        foreach(_f IN LISTS ${_group})
            list(APPEND _all "${JOY_ROOT}/${_f}")
        endforeach()
    endforeach()
    if(JOY_ENABLE_GLTF)
        list(APPEND _all "${JOY_ROOT}/src/jgltf.c")
    endif()
    if(JOY_ENABLE_SPINE)
        list(APPEND _all "${JOY_ROOT}/src/jspine.c")
    endif()
    if(JOY_ENABLE_PLUGINS)
        list(APPEND _all "${JOY_ROOT}/src/jmodule.c")
    endif()
    set(${OUT} ${_all} PARENT_SCOPE)
endfunction()
