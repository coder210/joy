# 可执行文件名（保持不变）
set(EXECUTABLE_NAME "joy2d_editor")
message(STATUS "EXECUTABLE_NAME = '${EXECUTABLE_NAME}'")

# 如果是 Android，必须命名为 main
if (ANDROID)
    set(MOBILE_ASSETS_DIR "${MOBILE_MAIN_DIR}/assets")
    message(STATUS "MOBILE_ASSETS_DIR = '${MOBILE_ASSETS_DIR}'")
    set(EXECUTABLE_NAME main)
    add_library(${EXECUTABLE_NAME} SHARED)
else()
    add_executable(${EXECUTABLE_NAME})
endif()

file(GLOB SRC_FILES "*.h" "*.c" "*.hpp" "*.cpp")

# 合并所有源文件
set(PROJECT_FILES
    ${SRC_FILES}
)

# 添加源文件到目标
target_sources(${EXECUTABLE_NAME}
    PRIVATE
        ${PROJECT_FILES}
        iosLaunchScreen.storyboard   # 直接使用文件名，已在当前目录
)

# Apple 平台额外图标
if(APPLE)
    add_definitions(-DLUA_USE_IOS)
    target_sources(${EXECUTABLE_NAME} PRIVATE logo.png)
endif()

# 设置语言标准
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Web 平台输出 HTML
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

# ------------------- 资产处理 -------------------
set(ASSETS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/assets")

file(GLOB_RECURSE ALL_ASSET_FILES "${ASSETS_ROOT}/*")
set(ASSET_FILES "")
foreach(file ${ALL_ASSET_FILES})
    if(NOT file MATCHES "\\.svn/" AND NOT file MATCHES "\\.git/")
        file(RELATIVE_PATH rel_path "${ASSETS_ROOT}" "${file}")
        list(APPEND ASSET_FILES "${rel_path}")
    endif()
endforeach()

if (APPLE)
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
    set(PRELOAD_FLAGS "")
    foreach(asset_file ${ASSET_FILES})
        list(APPEND PRELOAD_FLAGS "--preload-file \"${ASSETS_ROOT}/${asset_file}@${asset_file}\"")
    endforeach()
    message(STATUS "PRELOAD_FLAGS: ${PRELOAD_FLAGS}")
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${PRELOAD_FLAGS})

else()
    # 其他平台（Windows, Linux, Android 等）
    if(UNIX AND NOT APPLE)
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE m)
    endif()

    macro(copy_helper filename)
        if (ANDROID)
            if (NOT MOBILE_ASSETS_DIR)
                message(FATAL_ERROR "Building for Android, but MOBILE_ASSETS_DIR is not set")
            endif()
            set(outname "${MOBILE_ASSETS_DIR}/${filename}")
        else()
            set(outname "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/${filename}")
        endif()
        add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ASSETS_ROOT}/${filename}" "${outname}"
        )
    endmacro()

    foreach(asset_file ${ASSET_FILES})
        message(STATUS "Copying asset: ${asset_file}")
        copy_helper("${asset_file}")
    endforeach()
endif()

# ------------------- 平台特定属性 -------------------
set(RESOURCE_FILES "logo.png")
if(IOS)
    set(RESOURCE_FILES "iosLaunchScreen.storyboard;logo.png")
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
    XCODE_GENERATE_SCHEME TRUE
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.game.${PROJECT_NAME}"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.game.${PROJECT_NAME}"
    XCODE_ATTRIBUTE_CURRENTYEAR "${CURRENTYEAR}"
    RESOURCE "${RESOURCE_FILES}"
)
if(MSVC)
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>"
    )
endif()

# macOS 非 Xcode 生成器时的 bundle 修复（可选）
if(CMAKE_SYSTEM_NAME MATCHES "Darwin" AND NOT CMAKE_GENERATOR MATCHES "Xcode")
    install(TARGETS ${EXECUTABLE_NAME}
        BUNDLE DESTINATION ./install COMPONENT Runtime
        RUNTIME DESTINATION ./install/bin COMPONENT Runtime
    )
    set(DEP_DIR "${CMAKE_BINARY_DIR}")
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"\$<TARGET_BUNDLE_DIR:${EXECUTABLE_NAME}>\" \"\" \"${DEP_DIR}\")
    ")
    set(CPACK_GENERATOR "DragNDrop")
    include(CPack)
endif()

# 源文件分组（便于 IDE 显示）
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${PROJECT_FILES})

# 调试信息（可选）
message(STATUS "Loader current dir: ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "Loader source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Loader binary dir: ${CMAKE_BINARY_DIR}")

# ------------------- 链接 SDL3 -------------------
target_link_libraries(${EXECUTABLE_NAME} PRIVATE SDL3::SDL3)

# ------------------- 平台特定链接（仅需链接运行时库） -------------------
if(ANDROID)
    # Android 需要链接系统库
    find_library(GLESv2 GLESv2)
    find_library(EGL EGL)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${GLESv2} ${EGL} android log)

elseif(EMSCRIPTEN)
    # Emscripten 链接选项（已包含 WebGL 支持）
    target_link_options(${EXECUTABLE_NAME} PRIVATE
        -sUSE_SDL=3
        -sMAX_WEBGL_VERSION=1
        -sMIN_WEBGL_VERSION=1
        -sALLOW_MEMORY_GROWTH=1
    )

elseif(WIN32)
    # Windows：只链接 SDL3，OpenGL ES 由 SDL 运行时动态加载（通过 ANGLE 或系统驱动）
    # 无需额外链接 OpenGL 或 ES 库，否则会导致符号冲突或找不到库的错误
    #target_link_libraries(${EXECUTABLE_NAME} PRIVATE opengl32.lib)
elseif(APPLE)
    # macOS / iOS：直接链接系统 OpenGL 框架，不需要 find_package
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework OpenGLES")
else()
    # Linux / macOS：桌面 OpenGL 支持（若使用桌面 OpenGL 需链接）
    # 注意：如果你使用 OpenGL ES 代码，在 macOS 上仍需 ANGLE 或使用桌面 OpenGL 核心版本。
    # 这里保留传统的 OpenGL 链接，但若你的代码是纯 ES 且运行在 Linux 原生 ES 驱动下，则无需链接桌面 OpenGL。
    # 为兼容大多数情况，仍然链接 OpenGL 库（Linux 下 libGL.so 包含 ES 转发？不一定）。
    # 如果你的 Linux 系统有 libGLESv2，可以改为查找 GLESv2。
    find_package(OpenGL REQUIRED)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${OPENGL_LIBRARIES})
endif()