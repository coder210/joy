cmake_minimum_required(VERSION 3.16)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# prevent installing to system directories. 
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# Declare the project
project(joy2d)

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
set(EXECUTABLE_NAME ${PROJECT_NAME})

# Create an executable or a shared library based on the platform and add our sources to it
if (ANDROID)
	# The SDL java code is hardcoded to load libmain.so on android, so we need to change EXECUTABLE_NAME
	set(EXECUTABLE_NAME main)
	add_library(${EXECUTABLE_NAME} SHARED)
else()
	add_executable(${EXECUTABLE_NAME})
endif()

set(EXTERNAL_FILES "src/external/lua/*.h" "src/external/lua/*.c"  "src/external/mujs/*.h" "src/external/mujs/*.c"  "src/external/klib/*.h" "src/external/klib/*.c" "src/external/*.h" "src/external/*.c") 
set(CORE_FILES "src/core/*.h" "src/core/*.c") 
set(TEST_FILES "src/test/*.h" "src/test/*.c") 
set(LUACLIB_FILES "src/luaclib/*.h" "src/luaclib/*.c")
set(JSCLIB_FILES "src/jsclib/*.h" "src/jsclib/*.c") 
set(CSCLIB_FILES "src/csclib/*.h" "src/csclib/*.c") 
#set(SRC_FILES "src/*.h" "src/*.c") 
set(SRC_FILES "src/main.c") 

file(GLOB PROJECT_FILES
	${EXTERNAL_FILES}
	#${CORE_FILES}
    #${TEST_FILES}
	#${LUACLIB_FILES}
	#${JSCLIB_FILES} 
	#${CSCLIB_FILES}
    ${SRC_FILES}
)
#file(GLOB PROJECT_FILES "liv-main.c")

# Add your sources to the target
target_sources(${EXECUTABLE_NAME} 
PRIVATE 
    ${PROJECT_FILES}
    src/iosLaunchScreen.storyboard
)
# What is iosLaunchScreen.storyboard? This file describes what Apple's mobile platforms
# should show the user while the application is starting up. If you don't include one,
# then you get placed in a compatibility mode that does not allow HighDPI.
# This file is referenced inside Info.plist.in, where it is marked as the launch screen file.
# It is also ignored on non-Apple platforms. 

# To get an app icon on Apple platforms, add it to your executable.
# Afterward, the image file in Info.plist.in.
if(APPLE)
    target_sources("${EXECUTABLE_NAME}" PRIVATE "src/logo.png")
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
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

# Link SDL to our executable. This also makes its include directory available to us. 
target_link_libraries(${EXECUTABLE_NAME} PUBLIC 
    SDL3::SDL3
)


# ��ȡ����lua�ļ�
#file(GLOB ASSET_FILES "assets/scripts/*.lua")
set(ASSET_FILES
    "scripts/client_config.lua"
    "scripts/server_config.lua"
    "scripts/server2.lua"
    "scripts/client2.lua"

    "fonts/simhei.ttf"

    "textures/livnet.bmp"
    "textures/deco/01.png"
    "textures/deco/04.png"
    "textures/trees/tree.png"

)

# Dealing with assets
# We have some non-code resources that our application needs in order to work. How we deal with those differs per platform.
if (APPLE)
    # on Apple targets, the application bundle has a "resources" subfolder where we can place our assets.
    # SDL_GetBasePath() gives us easy access to that location.
    set(input_root "${CMAKE_CURRENT_LIST_DIR}/src")
    macro(add_resource FILE)
        file(RELATIVE_PATH relpath "${input_root}" "${FILE}")
        get_filename_component(relpath "${relpath}" DIRECTORY)
        target_sources(${EXECUTABLE_NAME} PRIVATE ${FILE})
        set_property(SOURCE ${FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${relpath}")
    endmacro()
    #add_resource("${CMAKE_CURRENT_LIST_DIR}/src/Inter-VariableFont.ttf")
    #add_resource("${CMAKE_CURRENT_LIST_DIR}/src/the_entertainer.ogg")
    #add_resource("${CMAKE_CURRENT_LIST_DIR}/src/gs_tiger.svg")
elseif(EMSCRIPTEN)
    # on the web, we have to put the files inside of the webassembly
    # somewhat unintuitively, this is done via a linker argument.
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE 
        #"--preload-file \"${CMAKE_CURRENT_LIST_DIR}/src/Inter-VariableFont.ttf@/\""
        #"--preload-file \"${CMAKE_CURRENT_LIST_DIR}/src/the_entertainer.ogg@/\""
        #"--preload-file \"${CMAKE_CURRENT_LIST_DIR}/src/gs_tiger.svg@/\""
    )
else()
    if (ANDROID)
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
set(RESOURCE_FILES "src/logo.png")
if(IOS)
    set(RESOURCE_FILES "src/iosLaunchScreen.storyboard;src/logo.png") # make the splash screen show up on iOS
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES 
    # On macOS, make a proper .app bundle instead of a bare executable
    MACOSX_BUNDLE TRUE
    # Set the Info.plist file for Apple Mobile platforms. Without this file, your app
    # will not launch. 
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/src/Info.plist.in"

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