# 可执行文件名（保持不变）
set(EXECUTABLE_NAME "adventure_client")
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

file(GLOB SRC_FILES "*.h" "*.c" "*.cc" "*.cpp")

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

# 链接依赖库（由根目录的 add_subdirectory 引入）
target_link_libraries(${EXECUTABLE_NAME} PUBLIC
    SDL3::SDL3
    protobuf::libprotobuf
    joy2d
)

# ------------------- 资产处理 -------------------
# 资产目录位于根目录的 assets/，使用 PROJECT_SOURCE_DIR 定位
# CMAKE_CURRENT_SOURCE_DIR换成项目目录
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
    #target_link_libraries(${EXECUTABLE_NAME} PUBLIC websocket)
else()
    # 其他平台（Windows, Linux, Android 等）
    if(UNIX AND NOT APPLE)
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE m)
    endif()

    # 辅助宏：复制资产到目标位置
    macro(copy_helper filename)
        if (ANDROID)
            # Android: 复制到 MOBILE_ASSETS_DIR（由外部传入）
            if (NOT MOBILE_ASSETS_DIR)
                message(FATAL_ERROR "Building for Android, but MOBILE_ASSETS_DIR is not set")
            endif()
            set(outname "${MOBILE_ASSETS_DIR}/${filename}")
        else()
            # 其他桌面平台：复制到可执行文件同目录
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
# Visual Studio 启动项目设置（作用于当前目录）
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "${EXECUTABLE_NAME}")

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
message(STATUS "Adventure current dir: ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "Adventure source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Adventure binary dir: ${CMAKE_CURRENT_BINARY_DIR}")