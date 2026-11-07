# ============================================================
# 旧接口兼容层
#
# 历史上本文件用 file(GLOB) 把 joy 全部源码直接 target_sources 塞进宿主。
# 现在 GLOB 换成 cmake/JoySources.cmake 的显式清单，但**源码注入的语义不变**：
# joy::all 是 INTERFACE target，link 它会把清单里的 .c 编进宿主，
# 宿主自己的编译定义（如 PIXELS_PER_METER）照常作用于 joy 源码。
#
# 新写法：
#   set_property(GLOBAL PROPERTY JOY_REQUESTED_MODULES cs lua js)
#   add_subdirectory(<joy 目录>)
#   target_link_libraries(<宿主> PRIVATE joy::all)
#
# 保留本文件是为了不破坏仍在调用 joy_add_sources() 的外部工程。
# ============================================================

if(NOT DEFINED JOY_ADD_CMAKE_DIR)
    set(JOY_ADD_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")
endif()

function(joy_add_sources TARGET)
    cmake_parse_arguments(JAS "SPINE" "" "EXCLUDE;MODULES" ${ARGN})

    if(JAS_MODULES)
        set_property(GLOBAL PROPERTY JOY_REQUESTED_MODULES ${JAS_MODULES})
    endif()
    if(JAS_SPINE)
        set(JOY_ENABLE_SPINE ON CACHE BOOL "编译 jspine.c 并链接 spine-c" FORCE)
    endif()
    if(JAS_EXCLUDE)
        message(WARNING
            "joy_add_sources: EXCLUDE 已废弃；新构建用显式清单 "
            "${JOY_ADD_CMAKE_DIR}/cmake/JoySources.cmake，请改清单而不是排除文件")
    endif()

    if(NOT TARGET joy_all)
        include("${JOY_ADD_CMAKE_DIR}/cmake/JoyTargets.cmake")
    endif()
    target_link_libraries(${TARGET} PRIVATE joy::all)
endfunction()
