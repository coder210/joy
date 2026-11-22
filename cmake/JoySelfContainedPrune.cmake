# cmake -P 脚本：删除 RuntimeDir 里不在 KEEP 白名单的 System.*/Microsoft.* BCL。
# 由 JoySelfContained.cmake 的 sc_deploy 目标在 POST_BUILD 时调用。
# 之所以用 -P 而不是 configure 期 file(GLOB)：publish 目录在 configure 时还不存在。
#
# 输入变量（由 -D 传入）：
#   PublishDir  - dotnet publish 的输出目录（仅用于判断，不修改）
#   RuntimeDir  - 已复制全部 publish 文件的运行期目录（在这里删除多余 BCL）
#   KEEP        - 分号分隔的白名单文件名

# cmake -P 是独立脚本模式，不继承工程的 cmake_minimum_required 策略，
# 必须显式开启 IN_LIST（CMP0057），否则 if(... IN_LIST ...) 报 Unknown arguments。
cmake_policy(SET CMP0057 NEW)

if(NOT DEFINED RuntimeDir OR NOT DEFINED KEEP)
    message(FATAL_ERROR "JoySelfContainedPrune: RuntimeDir / KEEP not set")
endif()

file(GLOB _all_bcl LIST_DIRECTORIES false
    "${RuntimeDir}/System.*.dll" "${RuntimeDir}/Microsoft.*.dll")

set(_removed 0)
foreach(_bcl IN LISTS _all_bcl)
    get_filename_component(_name "${_bcl}" NAME)
    if(NOT _name IN_LIST KEEP)
        file(REMOVE "${_bcl}")
        math(EXPR _removed "${_removed}+1")
    endif()
endforeach()

message(STATUS "JoySelfContainedPrune: kept ${KEEP}; removed ${_removed} unused BCL")
