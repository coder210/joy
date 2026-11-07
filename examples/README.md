# joy 多语言示例：同一款游戏的 4 种写法

本目录有 **4 个功能完全相同的可运行示例**，分别用 C++ / C# / Lua / JavaScript 写成，
用来对照「同一套 joy API，在不同脚本后端下的写法差异」。

> 这份 README 同时是**交接任务书**：记录目标、现状、约定与验证方式。
> 接手前请先读完第 1、2、6 节。

---

## 1. 目标

同一款 480×320 顶视角动作小游戏，用四种语言各实现一份，行为与画面一致：

| 目录 | 语言 | 业务逻辑写在哪 | 宿主 |
|---|---|---|---|
| `cpp/` | C++ | 直接写在 C++ 里，调 joy 公共 API | 无脚本后端 |
| `csharp/` | C# | `JoyGame.dll`（.NET 程序集） | `modules/cs`（CoreCLR 嵌入） |
| `lua/` | Lua | `*.lua` | `modules/lua`（Lua 5.4） |
| `js/` | JavaScript | `*.js` | `modules/js`（duktape） |

四份实现共用同一套 `assets/`，同一个固定随机种子（12345），因此同样的输入序列
应产生同样的结果，可以直接横向对比。

## 2. 功能规格（四份实现必须一致）

- 分辨率 480×320，逻辑分辨率固定，绘制按逻辑坐标。
- 三个场景：`Menu` → `Play` → `GameOver` →（回 `Play`）。
- `Menu`：标题、副标题、闪烁的开始提示、操作说明、背景网格。
- `Play`：
  - 玩家 WASD 移动，限制在画面内；`Space` 或鼠标左键射击。
  - 子弹向上飞，超出屏幕销毁。
  - 敌人从上方生成，追踪玩家；玩家与敌人接触即失败。
  - HUD：帧率、存活时间、击杀数。
- `GameOver`：显示击杀数与存活时间，`Space`/`Enter` 重开，`Esc` 退出。
- 缺资源（图片/字体）时退化为纯色矩形 + 关闭文字，不得崩溃。

**允许不同的部分**：各语言的 API 形状没法完全一样——C# 侧有 `Joy.Draw.fill_rect`
这种便利包装，Lua/JS 侧是 joy 原生 `joy.r2d.rectangle`。功能等价即可，
不追求逐行同构。

## 3. 目录约定

```text
examples/
├── README.md          ← 本文件（任务书）
├── CMakeLists.txt     ← 共享构建入口：一个文件定义 4 个 target
├── assets/            ← 四份实现共用的图片/字体（暂无，缺失时走纯色退化路径）
├── cpp/               ← main.cpp：窗口 + 主循环 + 全部游戏逻辑
├── csharp/            ← main.cpp（CLR 宿主）+ JoyGame.csproj + *.cs
├── lua/               ← main.cpp（Lua 宿主）+ scripts/*.lua
└── js/                ← main.cpp（JS 宿主）+ scripts/*.js
```

每个子目录自己带一个 `main.cpp` 宿主。宿主只做四件事：建窗口、建渲染、
把 SDL 事件转发给业务层、按帧驱动。游戏逻辑一律不写在宿主里（`cpp/` 除外，
它没有脚本层）。

## 4. 各后端的宿主写法（已核实）

### C#

```c
csx_p csx = csx_create();
csx_load_assembly(csx, "JoyGame.dll");
csx_method_p on_frame = csx_find_method(csx, "JoyGame", "Host", "OnFrame", 1);
void* args[1] = { &dt };
csx_invoke_method(csx, on_frame, args);
```

`Host.OnInit(IntPtr window, IntPtr r2d, int w, int h, int smokeFrames)` 由宿主播反射调用。

### Lua

`luax_create()` **内部已注册全部 `joy.*` 模块表**，宿主不需要自己 open 库：

```c
luax_p lua = luax_create();
luax_dofile(lua, "scripts/main.lua");
luax_call_joy(lua, "init", 0, 0);          // joy.init()
// 每帧
luax_dispatch_event(lua, &sdlevent);        // → joy.keypressed / joy.mousepressed ...
luax_call_joy(lua, "update", 1, dt);        // joy.update(dt)
luax_call_joy(lua, "draw", 0, 0);           // joy.draw()
```

脚本侧自己建窗口与渲染器：`joy.window.create(...)` → `joy.r2d.create(window)`。

### JavaScript

```c
jsx_p js = jsx_create();
jsx_dofile(js, "scripts/main.js");
jsx_dispatch_event(js, &sdlevent);
jsx_call_joy(js, "update", 1, dt);
```

与 Lua 同构，`joy.*` 表由 `jsx_create()` 注册。

### 三端共有的新绑定链路

除各自的旧胶水外，三个脚本后端都注册了 IDL 驱动的 `Joy.Api.Bindings.*`（方法名
三端一致）。本示例优先用旧胶水（`joy.r2d.*` / `Joy.Draw.*`），因为它覆盖绘制
与输入更完整；新链路当前覆盖 668 条纯标量 API，绘制类多未开放。

## 5. 已核实的能力边界

| 能力 | Lua (`joy.*`) | JS (`joy.*`) | C# (`Joy.*`) |
|---|---|---|---|
| 建窗口 | `window.create/get_size/should_close/destroy` | 同构 | `Joy.Host` 由 C 宿主建 |
| 渲染 2D | `r2d.create/begin/end_frame/set_color/rectangle/line/circle/polygon/grid` | 同构 | `Joy.Draw.fill_rect/stroke_rect/draw_line` |
| 图片 | `r2d.new_image/draw_image/draw_image_ex/free_image` | 同构 | `Joy.Image.image_create` + `Joy.Draw.draw_image*` |
| 文字 | `r2d.create_font/print_text/print`（另有 `joy.font.create`） | 同构 | `Joy.Font.font_create` + `Joy.Draw.draw_text*` |
| 键盘 | `joy.keyboard.is_down/is_keydown/is_up/is_keyup` + `joy.keypressed/keyreleased` 回调 | 同构 | `Joy.Input` 轮询 |
| 鼠标 | `joy.mousepressed/mousereleased/mousemoved` 回调（脚本自己缓存坐标） | 同构 | `Joy.Input` 轮询 |
| 时间 | `joy.update(dt)` 的 dt；`joy.timer.*` | 同构 | `Host.OnFrame(dt)` |

**已确认的三处能力差异**（都是旧胶水现状，示例按「功能等价」处理，没有改引擎）：

1. **没有文字宽度测量**。cs 有 `jtext_measurex` 包装（`Joy.Draw.text_width`），
   Lua / JS 的 `r2d.print` 只负责绘制。四份实现的居中/右对齐需要宽度，
   Lua / JS 版用 `字符数 × 字号 × 0.58` 近似——文字排版会有几像素偏差，
   不影响冒烟指纹（断言只看逻辑数值）。
2. **JS 的 `r2d` 没有图片函数**。`new_image` / `draw_image` / `draw_image_ex` /
   `free_image` 只在 Lua 版注册了，JS 版缺失。因此 `js/` 的精灵统一画成同尺寸色块，
   正好走「缺美术资源」的退化路径；逻辑与另外三份完全一致。
3. **Lua 的 `joy.keyboard.is_down(event, "A")` 不适合做游戏输入**：它要传单个
   `SDL_Event`、且键名表只有 A/Q/ESCAPE/SPACE/四个方向共 8 个（没有 W/S/D/J/Return）。
   Lua 版改为在 `joy.keypressed` / `joy.keyreleased` 里自维护按键表，与 C++/C#
   的轮询语义等价。

**另**：Lua/JS 都没有「查询鼠标当前坐标」的直接 API，只能在 `mousemoved` 回调里
缓存；`cpp/` 与 `csharp/` 可以直接读状态。

### 对引擎的两处改动

1. **新增 `jsx_get_context()`**（`modules/js/joy_js.h` + `jsx.c`，7 行）。
   `examples/js/main.cpp` 需要读写脚本全局变量（冒烟帧数、退出标志），而
   `modules/js` 原本没有取 `duk_context` 的接口（Lua 有 `luax_get_state` 这个对称品）。

2. **修复 `jg_create_font` 的参数错位**（`modules/js/bindings/legacy/r2d.c`）。
   jsclib 的 r2d 绑定是 0 基索引（renderer 在 `index 0`），但 `create_font` 是
   唯一不带 renderer 的函数，却沿用了 `1/2` 的偏移——`path` 实际取到字号，
   字体永远建不出来，日志表现为 `Failed to load font file: 12`。
   同时把失败返回值从 `pointer(NULL)` 改成 `undefined`：duktape 的 pointer 值
   恒为真，调用方拿 NULL 指针无法判空。

除这两处外，四份实现都只调公开接口。

## 6. 当前进度

| 项 | 状态 |
|---|---|
| 目标与规格确认 | 完成 |
| 四后端能力摸底 | 完成（见第 5 节） |
| `examples/` 骨架 + 共享 CMake | 完成 |
| `cpp/` 实现 | **完成，冒烟通过** |
| `csharp/` 实现（从旧仓迁移） | **完成，冒烟通过** |
| `lua/` 实现 | **完成，冒烟通过** |
| `js/` 实现 | **完成，冒烟通过** |

四份实现的冒烟指纹已完全对齐（见第 7 节），`ctest` 一把跑完。

`cpp/` 已实测通过，输出即第 7 节的基准指纹：

```text
[host] init 480x320 smoke=180
[scene] Menu
[scene] Play
[scene] GameOver kills=1 time=2.0
[scene] Play
[smoke] PASS api=n/a abi=True play=2 gameover=True moved=161.0 fired=9 kills=1 enemies=3
```

注意 `api=n/a`：`api` 一项是 C# 侧「IDL 新绑定链路自检」的结果，其余三种语言没有这套
东西，按 n/a 输出。**对齐口径是 `abi/play/gameover/moved/fired/kills/enemies` 七项。**

### 迁移来源

`csharp/` 的内容来自旧仓库 `joy_csharp_test`（480×320 C# 顶视角游戏，约 723 行：
`PlayScene.cs` 281、`MenuScene.cs` 56、`GameOverScene.cs` 52、`Painter.cs` 57、
`Assets.cs` 55、`InputState.cs` 46、`GameApp.cs` 47、`Host.cs` 33、`Enemy.cs` 23、
`Keys.cs` 19、`Player.cs` 17、`Bullet.cs` 14、`Texts.cs` 14、`IScene.cs` 9）。
该仓库已废弃删除，**代码只存在于本示例中**，不要再去找原仓。

## 7. 构建与验证

```powershell
# 在 joy 仓根目录
cmake -S . -B build
cmake --build build --config Release

# C# 需先构建程序集（其余三个不需要）
dotnet build examples/csharp/JoyGame.csproj -c Release -o build/examples/csharp
```

每个 target 都应支持 `--smoke N` 无窗口冒烟（dummy 视频驱动 + 固定 1/60 步长），
跑 N 帧后自动退出并按结果返回 0/非 0。这是四份实现唯一的自动化验证手段：

```powershell
$env:SDL_VIDEODRIVER="dummy"
./build/Release/example_cpp.exe   --smoke 180
./build/Release/example_lua.exe   --smoke 180
./build/Release/example_js.exe    --smoke 180
./build/Release/example_csharp.exe --smoke 180
```

> **注意：coreclr.dll 必须与每个 exe 同目录。**
> 只要本次配置启用了 `cs` 模块，`joy::all` 就会把 cs 模块的源码注入**每个** target，
> 于是 cpp / lua / js 三个 exe 的导入表里同样带 `coreclr.dll` 的符号（`mono_free` 等），
> 哪怕它们从不调用 `csx_*`。缺这个 dll 时，单独运行任何示例都会失败：
> `无法定位程序输入点 mono_free 于动态链接库 example_js.exe 上`。
> 因此四个 target 的 POST_BUILD 都通过 `joy_examples_deploy_clr()` 部署 `coreclr.dll`；
> BCL（`lib/`）只有 `example_csharp` 需要，因为只有它真正初始化 CLR。

判定口径：**四者的冒烟输出必须与旧仓基线一致**——

```text
[scene] Menu
[scene] Play
[scene] GameOver kills=1 time=2.0
[scene] Play
[smoke] PASS api=True abi=True play=2 gameover=True moved=161.0 fired=9 kills=1 enemies=3
```

`moved=161.0 fired=9 kills=1 enemies=3` 这四个数是固定种子 12345 + 固定 180 帧下的
行为指纹。四份实现跑出来必须相同，否则说明业务逻辑没有真正对齐。

## 8. 接手须知

1. **joy 是独立仓**（`https://github.com/coder210/joy.git`），本示例在它的
   `examples/` 下。改动要在 joy 仓提交推送。
2. **不要为了示例改引擎**：例子只调 `include/joy/*.h` 与 `modules/*/joy_*.h` 的
   公开接口。确实缺能力时，先在本文件第 5 节记一笔再决定。
3. **三个生成物门禁**：`node api/tools/gen_bindings.js --check` 必须通过；
   它校验 `api/generated/` 与 `modules/*/` 里的生成物是否与 IDL 同步。
4. 旧仓 `joy_csharp_test` 里有可复用的 `main.cpp` + `context.h` + `host/` + `systems/`
   分层写法（C++ 宿主与三个同类项目风格一致），`cpp/` 与 `csharp/` 的宿主可以参考。
