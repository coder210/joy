if(MSVC)
	enable_language(CSharp)

    file(GLOB JOY_SHARP_FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/Joy/*.cs"
        "${CMAKE_CURRENT_SOURCE_DIR}/Joy/Native/*.cs"
        "${CMAKE_CURRENT_SOURCE_DIR}/Joy/FixMath/*.cs"
        "${CMAKE_CURRENT_SOURCE_DIR}/Joy/Collision/*.cs"
        "${CMAKE_CURRENT_SOURCE_DIR}/Joy/GJK2D/*.cs")
    add_library(JoySharp SHARED ${JOY_SHARP_FILES})
    
    file(GLOB TEST1_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Test1/*.cs")
    add_library(Test1 SHARED ${TEST1_FILES})
    
    file(GLOB TEST2_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Test2/*.cs")
    add_library(Test2 SHARED ${TEST2_FILES})

    file(GLOB ASPOSEXLSTEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/AsposeXlsTest/*.cs")
    add_executable(AsposeXlsTest ${ASPOSEXLSTEST_FILES})

    file(GLOB SPIREXLSTEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/SpireXlsTest/*.cs")
    add_library(SpireXlsTest SHARED ${SPIREXLSTEST_FILES})

    # 设置 .NET 8 目标框架
    set_property(TARGET JoySharp PROPERTY DOTNET_SDK "Microsoft.NET.Sdk")
    set_property(TARGET JoySharp PROPERTY DOTNET_TARGET_FRAMEWORK "net8.0")
    
    set_property(TARGET Test1 PROPERTY DOTNET_SDK "Microsoft.NET.Sdk")
    set_property(TARGET Test1 PROPERTY DOTNET_TARGET_FRAMEWORK "net8.0")
    
    set_property(TARGET Test2 PROPERTY DOTNET_SDK "Microsoft.NET.Sdk")
    set_property(TARGET Test2 PROPERTY DOTNET_TARGET_FRAMEWORK "net8.0")
    
    set_property(TARGET AsposeXlsTest PROPERTY DOTNET_SDK "Microsoft.NET.Sdk")
    set_property(TARGET AsposeXlsTest PROPERTY DOTNET_TARGET_FRAMEWORK "net8.0")
    
    set_property(TARGET SpireXlsTest PROPERTY DOTNET_SDK "Microsoft.NET.Sdk")
    set_property(TARGET SpireXlsTest PROPERTY DOTNET_TARGET_FRAMEWORK "net8.0")

    # 项目依赖
    add_dependencies(Test1 JoySharp)
    add_dependencies(Test2 JoySharp)

endif()