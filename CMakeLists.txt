cmake_minimum_required(VERSION 3.16)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# prevent installing to system directories. 
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# Declare the project
project(joy2d C CXX)

if ((APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES "Darwin") OR EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")    # Disable shared builds on platforms where it does not make sense to use them
    set(SDL_SHARED OFF)
else()
    set(SDL_SHARED ON)
endif()

if(MSVC)
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP)				# parallelize each target, unless Ninja is the generator
    endif()
endif()

# Set the name of the executable
set(EXECUTABLE_NAME "${PROJECT_NAME}_loader")
set(SRC_DIR_NAME "${PROJECT_NAME}_loader")

# Create an executable or a shared library based on the platform and add our sources to it
if (ANDROID)
	# The SDL java code is hardcoded to load libmain.so on android, so we need to change EXECUTABLE_NAME
	set(EXECUTABLE_NAME main)
	add_library(${EXECUTABLE_NAME} SHARED)
else()
	add_executable(${EXECUTABLE_NAME})
endif()

set(EXTERNAL_FILES "${SRC_DIR_NAME}/external/lua/*.h" "${SRC_DIR_NAME}/external/lua/*.c"  "${SRC_DIR_NAME}/external/mujs/*.h" "${SRC_DIR_NAME}/external/mujs/*.c"  "${SRC_DIR_NAME}/external/klib/*.h" "${SRC_DIR_NAME}/external/klib/*.c" "${SRC_DIR_NAME}/external/*.h" "${SRC_DIR_NAME}/external/*.c") 
set(CORE_FILES "${SRC_DIR_NAME}/core/*.h" "${SRC_DIR_NAME}/core/*.c") 
set(TEST_FILES "${SRC_DIR_NAME}/test/*.h" "${SRC_DIR_NAME}/test/*.c") 
set(LUACLIB_FILES "${SRC_DIR_NAME}/luaclib/*.h" "${SRC_DIR_NAME}/luaclib/*.c")
set(JSCLIB_FILES "${SRC_DIR_NAME}/jsclib/*.h" "${SRC_DIR_NAME}/jsclib/*.c") 
set(CSCLIB_FILES "${SRC_DIR_NAME}/csclib/*.h" "${SRC_DIR_NAME}/csclib/*.c") 
set(SRC_FILES "${SRC_DIR_NAME}/*.h" "${SRC_DIR_NAME}/*.cpp") 
#set(SRC_FILES "src/main.c") 

file(GLOB PROJECT_FILES
	${EXTERNAL_FILES}
	${CORE_FILES}
    ${TEST_FILES}
	${LUACLIB_FILES}
	${JSCLIB_FILES} 
	${CSCLIB_FILES}
    ${SRC_FILES}
)
#file(GLOB PROJECT_FILES "liv-main.c")

# Add your sources to the target
target_sources(${EXECUTABLE_NAME} 
PRIVATE 
    ${PROJECT_FILES}
    ${SRC_DIR_NAME}/iosLaunchScreen.storyboard
)

# What is iosLaunchScreen.storyboard? This file describes what Apple's mobile platforms
# should show the user while the application is starting up. If you don't include one,
# then you get placed in a compatibility mode that does not allow HighDPI.
# This file is referenced inside Info.plist.in, where it is marked as the launch screen file.
# It is also ignored on non-Apple platforms. 

# To get an app icon on Apple platforms, add it to your executable.
# Afterward, the image file in Info.plist.in.
if(APPLE)
    add_definitions(-DLUA_USE_IOS)
    target_sources("${EXECUTABLE_NAME}" PRIVATE "${EXECUTABLE_NAME}/logo.png")
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
	CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)

# on Web targets, we need CMake to generate a HTML webpage. 
if(EMSCRIPTEN)
	set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

# Configure SDL by calling its CMake file.
# we use EXCLUDE_FROM_ALL so that its install targets and configs don't
# pollute upwards into our configuration.
add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(joy2d_core EXCLUDE_FROM_ALL)

# Link SDL to our executable. This also makes its include directory available to us. 
target_link_libraries(${EXECUTABLE_NAME} PUBLIC 
    SDL3::SDL3
    joy2d_core
)


# 递归搜索assets目录下的所有文件
file(GLOB_RECURSE ASSET_FILES 
    "${CMAKE_CURRENT_LIST_DIR}/assets/*"
)

# 如果需要排除某些文件类型，可以使用以下方式
file(GLOB_RECURSE ALL_ASSET_FILES 
    "${CMAKE_CURRENT_LIST_DIR}/assets/*"
)

# 过滤掉不需要的文件
set(ASSET_FILES "")
foreach(file ${ALL_ASSET_FILES})
    # 排除.svn、.git等版本控制文件
    if(NOT file MATCHES "\\.svn/" AND NOT file MATCHES "\\.git/")
        # 获取相对路径（相对于assets目录）
        file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_LIST_DIR}/assets" "${file}")
        list(APPEND ASSET_FILES "${rel_path}")
    endif()
endforeach()


# Dealing with assets
# We have some non-code resources that our application needs in order to work. How we deal with those differs per platform.
if (APPLE)
    # on Apple targets, the application bundle has a "resources" subfolder where we can place our assets.
    # SDL_GetBasePath() gives us easy access to that location.
    set(input_root "${CMAKE_CURRENT_LIST_DIR}/${SRC_DIR_NAME}")
    macro(add_resource FILE)
        file(RELATIVE_PATH relpath "${input_root}" "${FILE}")
        get_filename_component(relpath "${relpath}" DIRECTORY)
        target_sources(${EXECUTABLE_NAME} PRIVATE ${FILE})
        set_property(SOURCE ${FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${relpath}")
    endmacro()
    foreach(asset_file ${ASSET_FILES})
        add_resource("${CMAKE_CURRENT_LIST_DIR}/assets/${asset_file}")
    endforeach()
elseif(EMSCRIPTEN)
    # on the web, we have to put the files inside of the webassembly
    # somewhat unintuitively, this is done via a linker argument.
    # set(PRELOAD_FLAGS "")
    # foreach(asset_file ${ASSET_FILES})
    #     string(APPEND PRELOAD_FLAGS " --preload-file \"${CMAKE_CURRENT_LIST_DIR}/assets/${asset_file}@/\"")
    # endforeach()
    set(PRELOAD_FLAGS "")
    foreach(asset_file ${ASSET_FILES})
        # 添加两个参数：--preload-file 和 文件路径
        list(APPEND PRELOAD_FLAGS "--preload-file \"${CMAKE_CURRENT_LIST_DIR}/assets/${asset_file}@${asset_file}\"")
    endforeach()

    message(${PRELOAD_FLAGS})
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE 
        ${PRELOAD_FLAGS}
    )
else()
    if(UNIX AND NOT APPLE)
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE m)
    elseif (ANDROID)
        if (NOT MOBILE_ASSETS_DIR)
            message(FATAL_ERROR "Building for Android, but MOBILE_ASSETS_DIR is not set")
        endif()
        file(MAKE_DIRECTORY "${MOBILE_ASSETS_DIR}")    # we need to create the project Assets dir otherwise CMake won't copy our assets.
    endif()

    
    macro(copy_helper filename)
        if (ANDROID)
            # MOBILE_ASSETS_DIR is set in the gradle file via the cmake command line and points to the Android Studio Assets folder. 
            # when we copy assets there, the Android build pipeline knows to add them to the apk. 
            set(outname "${MOBILE_ASSETS_DIR}/${filename}")    
        else()
            # for platforms that do not have a good packaging format, all we can do is copy the assets to the process working directory.
			set(outname "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/${filename}")
        endif()
        add_custom_command(POST_BUILD
            TARGET "${EXECUTABLE_NAME}"
		    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/assets/${filename}" "${outname}"
	    )
        if(MSVC)
			add_custom_command(POST_BUILD
				TARGET "${EXECUTABLE_NAME}"
				COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/assets/${filename}" "${CMAKE_BINARY_DIR}/${filename}"
			)
		endif()
    endmacro()


	foreach(asset_file ${ASSET_FILES})
		# ������·������ȡ�ļ��������·��
		#get_filename_component(file_name ${asset_file} NAME)
        message(${asset_file})
		copy_helper(${asset_file})
	endforeach()

    #copy_helper("scripts/adventure/server_config.lua")
    #copy_helper("scripts/adventure/server2.lua")
    #copy_helper("scripts/adventure/server.lua")
    #copy_helper("the_entertainer.ogg")
    #copy_helper("gs_tiger.svg")
endif()

# set some extra configs for each platform
set(RESOURCE_FILES "${SRC_DIR_NAME}/logo.png")
if(IOS)
    set(RESOURCE_FILES "${SRC_DIR_NAME}/iosLaunchScreen.storyboard;${SRC_DIR_NAME}/logo.png") # make the splash screen show up on iOS
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES 
    # On macOS, make a proper .app bundle instead of a bare executable
    MACOSX_BUNDLE TRUE
    # Set the Info.plist file for Apple Mobile platforms. Without this file, your app
    # will not launch. 
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR_NAME}/Info.plist.in"

    # in Xcode, create a Scheme in the schemes dropdown for the app.
    XCODE_GENERATE_SCHEME TRUE
    # Identification for Xcode
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.game.${PROJECT_NAME}"
	XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.game.${PROJECT_NAME}"
	XCODE_ATTRIBUTE_CURRENTYEAR "${CURRENTYEAR}"
    RESOURCE "${RESOURCE_FILES}"
)

# on Visual Studio, set our app as the default project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${EXECUTABLE_NAME}")

# On macOS Platforms, ensure that the bundle is valid for distribution by calling fixup_bundle.
# note that fixup_bundle does not work on iOS, so you will want to use static libraries 
# or manually copy dylibs and set rpaths
if(CMAKE_SYSTEM_NAME MATCHES "Darwin" AND NOT CMAKE_GENERATOR MATCHES "Xcode")
    # tell Install about the target, otherwise fixup won't know about the transitive dependencies
    install(TARGETS ${EXECUTABLE_NAME}
    	BUNDLE DESTINATION ./install COMPONENT Runtime
   	    RUNTIME DESTINATION ./install/bin COMPONENT Runtime
    )
	
    set(DEP_DIR "${CMAKE_BINARY_DIR}")  # where to look for dependencies when fixing up
    INSTALL(CODE 
        "include(BundleUtilities)
        fixup_bundle(\"$<TARGET_BUNDLE_DIR:${EXECUTABLE_NAME}>\" \"\" \"${DEP_DIR}\")
        " 
    )
    set(CPACK_GENERATOR "DragNDrop")
    include(CPack)
endif()

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${PROJECT_FILES})

message(${CMAKE_CURRENT_LIST_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})
message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_BINARY_DIR})