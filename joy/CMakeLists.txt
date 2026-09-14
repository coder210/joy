# ============================================================
# joy —— 框架源码目录（不再构建独立库 target）
#
# joy 已改为"纯文件引入"：此处不再是库，不产 DLL/SO、无导出符号。
# 使用方式：各游戏项目的 CMakeLists.txt 里
#
#     include(${CMAKE_SOURCE_DIR}/joy/joy_add.cmake)
#     joy_add_sources(<游戏目标>)
#
# 即把 joy 全部源码（含 external/luaclib/jsclib/test）编入该目标，
# 并自动补上 SDL3、msgpack、平台编译选项与头文件路径。
#
# 相关文件：
#   - joy_add.cmake   源码引入助手（唯一入口）
#   - joy.h / j*.c    框架本体（jmodule.c 插件系统保留，按需可 EXCLUDE）
#   - external/       第三方源码（lua / duktape / mongoose / stb 等）
#   - luaclib/ jsclib/ Lua / JS 脚本绑定
#   - test/           自测占位文件（多为注释）
#
# 注意：不再从根 CMakeLists.txt add_subdirectory 本目录。
# ============================================================