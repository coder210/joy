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
#   场景层        jscene / janimator（SCENE 组）—— 数据驱动、与渲染解耦；
#                 janimator 只持有一个 joy_r2d_p 句柄设颜色，不属于渲染层
#   可选适配层    jgltf / jspine / jmodule —— 由 JOY_ENABLE_GLTF / _SPINE /
#                 _PLUGINS 控制，默认不参与构建；整文件由 JOY_USE_* 包裹，
#                 保持"未启用即空编译单元"的隔离语义，勿并入必编译的 TU
# jr3d 与 jgltf 属 3D，勿按"2D"归并；physics / behavior / net / audio 各自独立。
# 增删文件在这里登记，归属也只记在这一份清单里。
#
# 生成物 api/generated/jmath_wrappers.c 列在 math 域，等 IDL 改 manifest 后
# 由记录类型取代；src/jlut.c 是两张三角查表的唯一定义（原 jlut.h 数据）；
# src/jvendor_impl.c 是 stb/dr 系列全仓唯一实现翻译单元，必须保留。
# ============================================================

set(JOY_SRC_CORE
    src/jcore.c
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
