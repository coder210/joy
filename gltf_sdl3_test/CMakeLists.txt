# 可执行文件名（glTF/glb 加载 + CPU 骨骼蒙皮动画渲染测试）
set(EXECUTABLE_NAME "gltf_sdl3_test")
message(STATUS "EXECUTABLE_NAME = '${EXECUTABLE_NAME}'")

# 如果是 Android，必须命名为 main
if (ANDROID)
    set(EXECUTABLE_NAME main)
    add_library(${EXECUTABLE_NAME} SHARED)
else()
    add_executable(${EXECUTABLE_NAME})
endif()

file(GLOB SRC_FILES  "*.h"  "*.c" "*.hpp" "*.cpp")

# 添加源文件到目标
target_sources(${EXECUTABLE_NAME}
    PRIVATE
        ${SRC_FILES}
)

# 设置语言标准
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# MSVC 下源文件按 UTF-8 解析（项目文件为 UTF-8 无 BOM，避免 GBK 代码页误读中文注释吞行）
if(MSVC)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE /utf-8)
endif()

# 引入 joy 框架源码，并启用 GLTF 模块（joy/jgltf.c，CPU 骨骼蒙皮动画）
include(${CMAKE_SOURCE_DIR}/joy/joy_add.cmake)
joy_add_sources(${EXECUTABLE_NAME} GLTF)

# 添加项目自身目录为头文件搜索路径
target_include_directories(${EXECUTABLE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# ECS 演示用 entt（单头文件，复用仓库内 project_proxy2d 的副本）
target_include_directories(${EXECUTABLE_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/project_proxy2d")

# ------------------- 资产处理 -------------------
# 资产目录位于当前项目目录的 assets/
set(ASSETS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/assets")

# 递归搜索资产文件（排除版本控制目录）
file(GLOB_RECURSE ALL_ASSET_FILES "${ASSETS_ROOT}/*")
set(ASSET_FILES "")
foreach(file ${ALL_ASSET_FILES})
    if(NOT file MATCHES "\\.svn/" AND NOT file MATCHES "\\.git/")
        file(RELATIVE_PATH rel_path "${ASSETS_ROOT}" "${file}")
        list(APPEND ASSET_FILES "${rel_path}")
    endif()
endforeach()

# 根据不同平台处理资产
if (APPLE)
    # macOS/iOS: 放入 Resources 目录
    macro(add_resource FILE)
        file(RELATIVE_PATH relpath "${ASSETS_ROOT}" "${FILE}")
        get_filename_component(dir "${relpath}" DIRECTORY)
        target_sources(${EXECUTABLE_NAME} PRIVATE "${FILE}")
        set_property(SOURCE "${FILE}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${dir}")
    endmacro()
    foreach(asset_file ${ASSET_FILES})
        add_resource("${ASSETS_ROOT}/${asset_file}")
    endforeach()

elseif(EMSCRIPTEN)
    # Emscripten: 使用 --preload-file
    set(PRELOAD_FLAGS "")
    foreach(asset_file ${ASSET_FILES})
        list(APPEND PRELOAD_FLAGS "--preload-file \"${ASSETS_ROOT}/${asset_file}@${asset_file}\"")
    endforeach()
    message(STATUS "PRELOAD_FLAGS: ${PRELOAD_FLAGS}")
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${PRELOAD_FLAGS})

else()
    # 其他平台（Windows, Linux 等桌面平台）
    if(UNIX AND NOT APPLE)
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE m)
    endif()

    # 复制资产到可执行文件同目录
    foreach(asset_file ${ASSET_FILES})
        message(STATUS "Copying asset: ${asset_file}")
        set(outname "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/${asset_file}")
        add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ASSETS_ROOT}/${asset_file}" "${outname}"
        )
    endforeach()
endif()

# MSVC: 调试运行时工作目录指向可执行文件目录（保证能读到 robot.glb 等资产）
if(MSVC)
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>"
    )
endif()
# Visual Studio 启动项目设置（作用于当前目录）
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "${EXECUTABLE_NAME}")

# 源文件分组（便于 IDE 显示）
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SRC_FILES})

# 调试信息（可选）
message(STATUS "Project current dir: ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "Project source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Project binary dir: ${CMAKE_CURRENT_BINARY_DIR}")