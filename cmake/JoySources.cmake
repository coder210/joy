# ============================================================
# joy 自有源码的显式清单（替代 file(GLOB)）
#
# 这份清单就是架构文档：增删文件必须在这里登记，路径全部相对 JOY_ROOT
# （由 JoyTargets.cmake 设置为本仓库根）。源码按性质分三处，互不混放：
#   src/             人工实现，9 个域文件 + 1 个显式登记的数据分片
#   api/generated/   工具生成，勿手改
#   external/        vendored 第三方（含一个自有包装 aes_util.c）
#
# 组织规则（2026-11-13 起，由 api/tools/audit_domain_layout.js 强制校验）：
#   1. 一个 .c 只承载一个域的函数定义；
#   2. 一个域只有一个 .c —— src/joy_<域>.c；
#   3. 每个 .c 必配一个同名内部头 —— src/joy_<域>.h，且 .c 必须 include 它。
#   函数名前缀 joy_<域>_ 必须与文件名、配套头的域一致。
#   域白名单（9 个）：core math r2d r3d ui scene net script audio。
#   input 已并回 core：13 个 API 统一为 joy_core_input_*，域前缀与文件名一致。
#   4. 数据分片例外：src/joy_lut.c（SIN_LUT / tan_lut）不含任何函数定义，
#      不算域实现文件，但同样配 src/joy_lut.h 且必须 include 它；
#      白名单在 audit_domain_layout.js 的 DATA_SHARDS，新增数据分片必须登记。
#
# 每个域 .c 由若干「分片」拼接而成；各分片原本是独立 TU，合并后在分片末尾
# 用 #undef 隔离私有宏，避免污染同域后续分片。分片归属：
#   core   jvendor_impl / jcore / jutils     math  jcollision / jgjk / jbox2d
#   r2d    jr2d / jfont / jparticle / jspine r3d  jr3d / jlight / jgltf
#   ui     jui / jui_im                      scene jscene / janimator / jtilemap /
#                                                 jecs / jprocmap / jbehavior
#   net    jnetwork / jsys                   script jmsgpack / jmodule
#   audio  jaudio                           core   jinput（原 input 域并入）
#
# 顺序敏感项：
#   - jvendor_impl 分片（stb/dr 的 *_IMPLEMENTATION 全仓唯一定义）必须置于 core
#     最前；实现宏若晚于其它分片对同名头的 #include，实现体不会展开。
#   - jgltf / jspine / jmodule 分片整体分别被 JOY_USE_GLTF / JOY_USE_SPINE /
#     JOY_USE_PLUGINS 包裹；JOY_ENABLE_* 由 JoyTargets.cmake 转发为 JOY_USE_*，
#     未启用时对应分片为空段，因此不再需要按开关追加源文件。
#
# 数据分片 src/joy_lut.c（SIN_LUT / tan_lut 查表，约 5.1 万行）是两张表的
# 唯一定义；它属于 math 源码组但不计入「一域一 .c」。extern 声明与长度宏仍在
# include/joy/jlut.h（jmath.h 的 sizeof 依赖）。api/generated/jmath_wrappers.c
# 是 jmath_wrap.js 的生成物、位于生成目录而非 src/，与 external/ 第三方、
# examples/、modules/ 一样不在本规则管辖内。
#
# 公开 API 声明按域归属在 src/joy_<域>.h，include/joy/japi.h 只做 include 聚合；
# IDL 的 source 字段记的是域头行号，因此改动域头声明后必须跑 --scan。改公开 C 名仍须同步
# api/idl/rename_map.json（继承 status/owns/borrowed/note）与
# api/tools/naming.js 的 ALIASES（冻结脚本层方法名），再跑 --scan / --check。
# ============================================================

set(JOY_SRC_CORE
    src/joy_core.c
)

set(JOY_SRC_MATH
    src/joy_math.c
    src/joy_lut.c
    api/generated/jmath_wrappers.c
)

set(JOY_SRC_R2D
    src/joy_r2d.c
)

set(JOY_SRC_R3D
    src/joy_r3d.c
)

set(JOY_SRC_UI
    src/joy_ui.c
)

set(JOY_SRC_SCENE
    src/joy_scene.c
)

set(JOY_SRC_NET
    src/joy_net.c
)

set(JOY_SRC_SCRIPT
    src/joy_script.c
)

set(JOY_SRC_AUDIO
    src/joy_audio.c
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
    JOY_SRC_R2D
    JOY_SRC_R3D
    JOY_SRC_UI
    JOY_SRC_SCENE
    JOY_SRC_NET
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
