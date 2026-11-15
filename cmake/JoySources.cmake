# ============================================================
# joy 自有源码的显式清单（替代 file(GLOB)）
#
# 这份清单就是架构文档：增删文件必须在这里登记，路径全部相对 JOY_ROOT
# （由 JoyTargets.cmake 设置为本仓库根）。源码按性质分三处，互不混放：
#   src/             人工实现，9 个域文件 + 1 个显式登记的数据分片
#                    （另有生成物域 math，实现由 api/generated/ 生成）
#   api/generated/   工具生成，勿手改
#   external/        vendored 第三方（含一个自有包装 aes_util.c）
#
# 除了编译用的 .c 清单，本文件末尾还有一份头文件清单（JOY_HEADERS_SRC /
# JOY_HEADERS_PUBLIC）。CMake 只会把登记进 target_sources 的文件列到 VS 的
# 解决方案资源管理器里：.c 由 joy_collect_sources() 收集、.h 由
# joy_collect_headers() 收集，两者一起登记进 joy_all 的 INTERFACE sources。
# .h 由 CMake 自动标为 HEADER_FILE_ONLY，只影响 IDE、不生成编译规则。
# 新增/删除 .h 时同样要在这里登记，否则 VS 里看不到该文件。
#
# 组织规则（2026-11-13 起，由 api/tools/audit_domain_layout.js 强制校验）：
#   1. 一个 .c 只承载一个域的函数定义；
#   2. 一个域只有一个 .c —— src/j<域>.c；
#   3. 每个 .c 必配一个同名内部头 —— src/j<域>.h，且 .c 必须 include 它。
#   函数名前缀 joy_<域>_ 必须与文件名、配套头的域一致。
#   域白名单：9 个手写域 core collision r2d r3d scene net sys script audio，
#   外加 1 个仅由生成物实现的域 math（见 audit_domain_layout.js 的
#   GENERATED_ONLY_DOMAINS）；math 与 collision 的分工是：jmath.h 的标量/向量
#   数学由 jmath_wrap.js 生成 joy_math_* 归 math，碰撞/GJK/box2d 求交归
#   collision。原 src/joy_math.c 已拆为 src/jcollision.c。
#   input 已并回 core：13 个 API 统一为 joy_core_input_*，域前缀与文件名一致。
#   ui 已并回 r2d：218 个 API 统一为 joy_r2d_*（begin/end 避让帧起止 -> joy_r2d_im_*），
#   声明与实现并入 src/jr2d.h / .c。域边界按渲染上下文划分：依赖 joy_r2d_p 的
#   2D 控件归 r2d，依赖 joy_r3d_p 的 3D HUD 控件归 r3d，两边都不再单设 UI 文件。
#   4. 数据分片例外：src/jlut.c（SIN_LUT / tan_lut）不含任何函数定义，
#      不算域实现文件，但同样配 src/jlut.h 且必须 include 它；
#      白名单在 audit_domain_layout.js 的 DATA_SHARDS，新增数据分片必须登记。
#
# 每个域 .c 由若干「分片」拼接而成；各分片原本是独立 TU，合并后在分片末尾
# 用 #undef 隔离私有宏，避免污染同域后续分片。分片归属：
#   core   jvendor_impl / jcore / jutils     collision jcollision / jgjk / jbox2d
#   r2d    jr2d / jfont / jparticle / jspine r3d  jr3d / jlight / jgltf
#          / jui / jui_im（原 ui 域并入）
#                                           scene jscene / janimator / jtilemap /
#                                                 jecs / jprocmap / jbehavior
#   net    jnetwork / IOCP 分片              script jmsgpack / jmodule
#   sys    jsys（socket / 时间 / 崩溃 / 网卡枚举，2026-11-15 从 net 拆出）
#   audio  jaudio                           core   jinput（原 input 域并入）
#
# 顺序敏感项：
#   - jvendor_impl 分片（stb/dr 的 *_IMPLEMENTATION 全仓唯一定义）必须置于 core
#     最前；实现宏若晚于其它分片对同名头的 #include，实现体不会展开。
#   - jgltf / jspine / jmodule 分片整体分别被 JOY_USE_GLTF / JOY_USE_SPINE /
#     JOY_USE_PLUGINS 包裹；JOY_ENABLE_* 由 JoyTargets.cmake 转发为 JOY_USE_*，
#     未启用时对应分片为空段，因此不再需要按开关追加源文件。
#
# 数据分片 src/jlut.c（SIN_LUT / tan_lut 查表，约 5.1 万行）是两张表的
# 唯一定义；它属于 math 源码组但不计入「一域一 .c」。extern 声明与长度宏仍在
# include/joy/jlut.h（jmath.h 的 sizeof 依赖）。api/generated/jmath_wrappers.c
# 是 jmath_wrap.js 的生成物、位于生成目录而非 src/，与 external/ 第三方、
# examples/、modules/ 一样不在本规则管辖内。
#
# 公开 API 声明按域归属在 src/j<域>.h，include/joy/japi.h 只做 include 聚合；
# IDL 的 source 字段记的是域头行号，因此改动域头声明后必须跑 --scan。改公开 C 名仍须同步
# api/idl/rename_map.json（继承 status/owns/borrowed/note）与
# api/tools/naming.js 的 ALIASES（冻结脚本层方法名），再跑 --scan / --check。
# ============================================================

set(JOY_SRC_CORE
    src/jcore.c
)

# math 域没有手写 .c：220 条 joy_math_* 由 jmath_wrap.js 从 include/joy/jmath.h
# 生成到 api/generated/jmath_wrappers.c；src/jlut.c 是它的数据分片。
set(JOY_SRC_MATH
    src/jlut.c
    api/generated/jmath_wrappers.c
)

# 原 src/joy_math.c 里 87 条碰撞/GJK/box2d/tilemap 求交 API 改名为
# joy_collision_*，拆到独立的 collision 域（src/jcollision.{c,h}）。
set(JOY_SRC_COLLISION
    src/jcollision.c
)

set(JOY_SRC_R2D
    src/jr2d.c
)

set(JOY_SRC_R3D
    src/jr3d.c
)

set(JOY_SRC_SCENE
    src/jscene.c
)

set(JOY_SRC_NET
    src/jnet.c
)

# 原 src/jnet.c 的 jsys.c 分片拆出为 sys 域：23 条跨平台 OS 原语
# （socket / 时间 / 崩溃处理 / 网卡枚举）改名 joy_sys_*；3 条 IOCP 服务器
# 属于网络层，留在 jnet.c，不随分片搬走。
set(JOY_SRC_SYS
    src/jsys.c
)

set(JOY_SRC_SCRIPT
    src/jscript.c
)

set(JOY_SRC_AUDIO
    src/jaudio.c
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
    JOY_SRC_COLLISION
    JOY_SRC_R2D
    JOY_SRC_R3D
    JOY_SRC_SCENE
    JOY_SRC_NET
    JOY_SRC_SYS
    JOY_SRC_SCRIPT
    JOY_SRC_AUDIO
    JOY_SRC_VENDOR
)

function(joy_collect_sources OUT)
    set(_all "")
    foreach(_group IN LISTS JOY_ALL_SOURCE_GROUPS)
        foreach(_f IN LISTS ${_group})
            list(APPEND _all "${JOY_ROOT}/${_f}")
        endforeach()
    endforeach()
    set(${OUT} ${_all} PARENT_SCOPE)
endfunction()

# --------------------------------------------------------------- IDE 头文件
# CMake 只会把登记进 target_sources 的文件列进 VS 解决方案资源管理器。
# 上面的源码组只含 .c；若不同时登记 .h，joy\src 筛选器下就只有实现文件，
# 配套域头与公开 API 声明全都看不见。.h 不是编译单元（CMake 自动按
# HEADER_FILE_ONLY 处理），登记只影响 IDE，不改变编译与链接结果。
# 与源码组一样用显式清单：新增域文件必须在这里同步登记配套头。
set(JOY_HEADERS_SRC
    src/jcore.h
    src/jcollision.h
    src/jr2d.h
    src/jr3d.h
    src/jscene.h
    src/jnet.h
    src/jsys.h
    src/jscript.h
    src/jaudio.h
    src/jlut.h
)

set(JOY_HEADERS_PUBLIC
    include/joy/jconfig.h
    include/joy/jdefs.h
    include/joy/jtypes.h
    include/joy/jlut.h
    include/joy/jmath.h
    include/joy/japi.h
    include/joy/joy.h
)

set(JOY_ALL_HEADER_GROUPS
    JOY_HEADERS_SRC
    JOY_HEADERS_PUBLIC
)

function(joy_collect_headers OUT)
    set(_all "")
    foreach(_group IN LISTS JOY_ALL_HEADER_GROUPS)
        foreach(_f IN LISTS ${_group})
            list(APPEND _all "${JOY_ROOT}/${_f}")
        endforeach()
    endforeach()
    set(${OUT} ${_all} PARENT_SCOPE)
endfunction()
