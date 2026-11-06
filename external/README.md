# external/ — 第三方源码（参与编译）

根目录顶层的 `.c/.h` 是 joy 核心直接使用的第三方单文件库（mongoose、kcp、cJSON、
stb_truetype 等）；子目录按第三方项目分组，由**启用它的脚本模块**显式引入。
`joy_add.cmake` 的核心 GLOB 只收 `external/*.c`（不递归），所以模块关闭时这些
VM 源码零成本，不会被编进可执行文件。

| 目录 | 用途 | 引入方 |
|---|---|---|
| `lua/` | Lua VM 源码 | `modules/lua`（`lua.c` 独立解释器入口被排除） |
| `lua_cjson/` | lua-cjson 源码 | `modules/lua` |
| `duktape/` | duktape JS 引擎 | `modules/js` |
| `cgltf/`、`klib/` | glTF 解析 / 通用容器 | joy 核心 |

模块胶水用基于 include 根的逻辑路径引用第三方：`#include "lua/lauxlib.h"`、
`#include "lua_cjson/lua_cjson.h"`、`#include "duktape/duktape.h"`，位置由
`JOY_EXTERNAL_DIR`（默认 `${JOY_SOURCE_DIR}/external`）决定，不再写
`../external/...` 这类把模块与第三方位置绑死的跨目录相对路径。

许可证与版权声明见各源文件头部；这些是源码快照，升级时整体替换目录即可。
预编译二进制不放这里，放 `runtimes/`。
