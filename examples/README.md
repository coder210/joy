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
| `cpp/` | C++ | 编译成原生 DLL，通过宿主注入的 `JoyApi` 函数指针表调 joy | 无脚本 VM，native plugin |
| `csharp/` | C# | `JoyGame.dll`（.NET 程序集） | `modules/cs`（CoreCLR 嵌入） |
| `lua/` | Lua | `*.lua` | `modules/lua`（Lua 5.4） |
| `js/` | JavaScript | `*.js` | `modules/js`（duktape） |

### 平台矩阵

| 平台 | cpp | lua | js | csharp | 产物组织 |
|---|---|---|---|---|---|
| Windows / Linux / macOS | ✓ | ✓ | ✓ | ✓ | 一份共享资源目录 + 四个 exe，一个 artifact |
| Android | ✓ | ✓ | ✓ | ✓ | 一个 flavor 一个 APK，各自带 assets 副本 |
| Web（wasm） | ✓ | ✓ | ✓ | — | 一个后端一个 `html/js/wasm/data` 子目录，一个 Pages 站点 |

Web 不带 C#（方案 A）：C# 后端是 C/C++ 宿主嵌入 MonoVM（`mono_jit_init` +
`mono_add_internal_call` + 动态库 + 真实文件系统里的 BCL），`Joy.Bindings`
的绑定全是 `[MethodImpl(InternalCall)]`，靠 C 侧注册。.NET 8 虽正式支持
`browser-wasm`，但那是 SDK 托管的 interpreter 应用（`dotnet.js` 启动
`mono_wasm_load_runtime`），宿主模型不兼容。因此 `csharp_host/CMakeLists.txt`
在 `EMSCRIPTEN` 下直接跳过，`examples/CMakeLists.txt` 也不启用 cs 模块。

Web 打包按 Android 的「每后端自包含」组织，不像桌面共用一份资源目录：
Emscripten 的 `--preload-file` 在**链接期**就要读到源文件，而桌面 / Android
的资源是 POST_BUILD 才拷进资源根，所以 CMake 直接对源码资源生成
`<绝对路径>@<虚拟 FS 相对路径>` 映射（`@` 右侧与 APK assets 根一致，不带
`assets/` 前缀），每个 wasm 模块自带一份 `.data`，脚本按后端裁剪。

四份实现共用同一套 `assets/`，同一个固定随机种子（12345），因此同样的输入序列
应产生同样的结果，可以直接横向对比。

## 2. 功能规格（四份实现必须一致）

- 分辨率 480×320，逻辑分辨率固定，绘制按逻辑坐标。屏幕适配照
  `joy_topdown_template` 的做法放在应用层（见 `mobile_display.h`）：四份宿主在
  建窗口前设 `SDL_HINT_ORIENTATIONS` 固定横屏；四份示例在建好 renderer 后调
  `R2dSetLogicalPresentationMode(480, 320, LETTERBOX)`，覆盖 `joy_r2d_create()`
  默认的「窗口物理尺寸 + STRETCH」——Android 的 `SDL_CreateWindow` 会把窗口
  改成屏幕物理尺寸，拿它当逻辑分辨率时 480×320 的画面只会落在屏幕一角。
  `LETTERBOX` 等比缩放居中，任意屏宽比都不拉伸；桌面窗口本征 480×320，
  与 `STRETCH` 等价。模板游戏用 `joy_r2d_get_logical_presentation()` 做自适应布局，
  所以能进一步铺满且无黑边；本仓四份示例把 480×320 写进玩法逻辑，只能用
  模板保留的 `presentation_set_design()` 方案。
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
├── assets/            ← 源码资源根：静态图片/字体（运行时铺进资源根，不带 assets/ 前缀）
├── cpp/               ← src/host.cpp（薄宿主）+ src/game.cpp（游戏 DLL）+ joy_game_hooks.h
├── csharp/            ← 纯 C# 工程：JoyGame.csproj + game/ runtime/ test/（无 native 代码）
├── csharp_host/       ← src/main.cpp（CLR native 宿主）+ CMakeLists.txt（example_csharp target）
├── lua/               ← src/main.cpp（Lua 宿主）+ src/main.lua（游戏逻辑）
└── js/                ← src/main.cpp（JS 宿主）+ src/main.js（游戏逻辑）
```

每个子目录的真实 `src/` 里放本示例自己的代码（宿主，lua/js 另有游戏脚本）。
C# 是例外：`csharp/` 只放托管工程，native 宿主独立在 `csharp_host/`，两者由
`csharp_host/CMakeLists.txt` 的 `_cs_proj` 关联。这样 MSBuild 的默认 None glob
不会再挂进 `main.cpp` / `CMakeLists.txt`，C# 项目节点只出现显式列出的 `.cs`。
宿主只做四件事：建窗口、建渲染、把 SDL 事件转发给业务层、按帧驱动。
游戏逻辑一律不写在宿主里：`cpp/` 把逻辑编译成 `JoyGameCpp.dll`，宿主用
`SDL_LoadObject` 加载、取唯一入口 `joy_game_get_hooks`，并把 `JoyApi` 大函数
指针表注入 DLL；DLL 不链接 `joy::all`，所有引擎调用都走表里的强签名函数指针。

运行期资源根不含 `assets/` 前缀：桌面就是 exe 同目录（`fonts/`、`sprites/`、
`scripts/`、`cpp/`、`lib/`、`JoyGame.dll` 直接铺开），Android 是构建树的
`pack/`，打包时原样成为 APK 的 `assets/` 根。两者相对布局一致，宿主一律按
`fonts/simhei.ttf` / `scripts/main.lua` / `JoyGame.dll` 这类相对路径加载。
原因：Android 的 `SDL_IOFromFile` 会把路径原样交给 `AAssetManager`，其路径
已经相对 APK 的 `assets/` 根，写成 `assets/foo` 会去找 `assets/assets/foo`；
`joy_topdown_template` 也是这套布局。`coreclr.dll` 是原生库，仍留在 exe 旁
由系统加载器查找，不进资源根。
VS 里 `src` 筛选器是本示例的游戏代码，注入的 joy 引擎源码统一收在 `joy\...` 下。

> **平台边界**：由 `JOY_GAME_STATIC` 选择构建形态（`-DJOY_GAME_STATIC=ON/OFF`），
> iOS / wasm 默认开启。动态形态下桌面用 `SDL_LoadObject` 从 `cpp/` 加载；
> Android 的 `SDL_LoadObject` 同样可用，但 `.so` 必须进 APK 的 `lib/<abi>/` 由系统
> 解压，宿主改用裸库名让 linker 查找（压缩的 `assets/` 里 `dlopen` 打不开）。
> iOS 运行时只能加载构建期签名并随包的代码、wasm 需 `MAIN_MODULE/SIDE_MODULE`
> 加虚拟 FS 预加载，因此这两者默认走静态形态，把 `game.cpp` 直接编进宿主、
> 链接期解析 `joy_game_get_hooks`。两种形态共用同一份 `game.cpp` 与契约头。
> 桌面已验证动态与静态两种形态；Android/iOS/wasm 需对应工具链验证。

## 4. 各后端的宿主写法（已核实）

### C++（native plugin）

下面是**动态形态**（桌面 / Android）。静态形态（iOS / wasm）把 `game.cpp` 编进
宿主、链接期直接调 `joy_game_get_hooks()`，不加载共享库；下面的 `JoyApi` 注入
与帧驱动写法完全相同。

```c
/* 宿主：游戏 DLL 只有一个导出入口，不需要 .lib / __declspec(dllimport) */
SDL_SharedObject* lib = SDL_LoadObject("cpp/JoyGameCpp.dll");
const JoyGameHooks* hooks = ((const JoyGameHooks* (*)(void))
    SDL_LoadFunction(lib, "joy_game_get_hooks"))();

JoyApi api;                        /* 701 个强签名函数指针 */
joy_api_vtable_init(&api, NULL);   /* 宿主是唯一链接 joy::all 的一方 */
hooks->init(hooks->game, &api, smoke_frames);
hooks->on_frame(hooks->game, dt, &input, &result);
hooks->shutdown(hooks->game);
SDL_UnloadObject(lib);
```

`JoyApi` 由 `gen_bindings.js` 从 `joy.idl` 生成；DLL 只 include
`generated/joy_api_vtable.h` 与 `joy_game_hooks.h`，不 include `joy.h`、
不链接 SDL 库。表头 `abi_version` / `size` / `ctx` 三字段固定，DLL 侧
校验版本即可前后兼容加 API。游戏侧用 `Jwindow` / `Jr2d` / `Font` / `Image`
等强句柄薄包装，编译期不能混传。

### C#

```c
csx_p csx = csx_create();
csx_load_assembly(csx, "JoyGame.dll");
csx_method_p on_frame = csx_find_method(csx, "JoyGame", "Host", "OnFrame", 1);
void* args[1] = { &dt };
csx_invoke_method(csx, on_frame, args);
```

`Host.OnInit(int width, int height, int smokeFrames)` 由宿主反射调用（3 个标量参数）。
**窗口与渲染器不传进来**：由 `GameApp` 自己用 `Joy.Api.Bindings.CreateWindow` /
`R2dCreate` 创建。原因是生成链路用句柄表管理对象，C 宿主直接创建的裸指针不在表里，
`Jr2d` 拿不到它。宿主因此不持有窗口/渲染器，也不再负责 `R2dBegin` / `R2dEnd`
（这两步搬到了 `GameApp.Tick`）。

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

### 三端共有的新绑定链路（生成 API）

三个脚本后端都注册了 IDL 驱动的 `Joy.Api.Bindings.*`，方法名三端一致：
Lua 是 `Joy.Api.Bindings.Xxx`，JS 是 `Joy.Api.Bindings.Xxx`（挂在全局 `Joy` 上），
C# 是强类型包装 `Joy.Api.Bindings.Xxx(...)`（返回 `Jwindow` / `Jr2d` / `Font`，
不暴露 `IntPtr`）。

**本示例的绘制与窗口全部走生成链路**，为此在 `api/idl/joy.idl` 里把这 6 条从
`status:off` 翻成 `on`（`node api/tools/gen_bindings.js` 重新生成后三端自动获得）：

| IDL 条目（P1 后的 C 名） | 脚本名（`ALIASES` 冻结） | 参数（展开后） |
|---|---|---|
| `joy_core_create_window` | `CreateWindow` | `(title, w, h)` → handle |
| `joy_r2d_create` | `R2dCreate` | `(window)` → handle |
| `joy_r2d_font_create` | `FontCreate` | `(path, size)` → handle |
| `joy_ui_image_create` | `ImageCreate` | `(r2d, path)` → handle |
| `joy_math_shape_draw_rectangle` | `ShapeDrawRectangle` | `(r2d, mode, x, y, w, h)`（`jrect_t` 按字段展开） |
| `joy_r2d_jtext_printx` | `JtextPrintx` | `(r2d, font, str, len, cr, cg, cb, ca, x, y)` |

脚本层名字由 `api/tools/naming.js` 的 `ALIASES` 冻结，C 名改了也不跟着变。

配套用法：`R2dSetColorf(r2d, r, g, b, a)` 先设颜色，再 `ShapeDrawRectangle(..., "fill")`
画实心、`"line"` 画描边；`R2dBegin` 用当前颜色清屏。

**仍然只能走旧胶水 / 退化的两处**（`L3` 未开放，见第 5 节）：
事件输入（`keypressed` / `mousepressed` 回调不在 IDL）、文字宽度测量
（`joy_r2d_jtext_measurex` 是 `int*` 输出参数）。图片绘制已由全标量 L1
`joy_ui_image_draw_rect` 绕开 `jrect_t*`：四份示例统一用 `ImageCreate` +
`ImageDrawRect` 画真实贴图，加载失败才退回同尺寸色块。

## 5. 已核实的能力边界

> 下表是**旧手写胶水时代的历史记录**（`modules/*/bindings/legacy/` 已删除）。
> 当前四份示例的窗口与绘制全部走生成链路 `Joy.Api.Bindings.*`（见第 4 节），
> 表里的 `window.create` / `r2d.print` / `Joy.Draw.*` 等都已不存在，保留仅供对照
> 早期设计，不要据此写新代码。

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
   `free_image` 只在 Lua 版注册了，JS 版缺失。这是旧胶水时代 `js/` 把精灵画成
   同尺寸色块的原因；当前四份示例已全量改走生成链路，统一用 `ImageCreate` +
   `ImageDrawRect` 画真实贴图，此差异不再影响渲染。
3. **Lua 的 `joy.keyboard.is_down(event, "A")` 不适合做游戏输入**：它要传单个
   `SDL_Event`、且键名表只有 A/Q/ESCAPE/SPACE/四个方向共 8 个（没有 W/S/D/J/Return）。
   Lua 版改为在 `joy.keypressed` / `joy.keyreleased` 里自维护按键表，与 C++/C#
   的轮询语义等价。

**另**：Lua/JS 都没有「查询鼠标当前坐标」的直接 API，只能在 `mousemoved` 回调里
缓存；`cpp/` 与 `csharp/` 可以直接读状态。

### 对引擎的两处改动

1. **新增 `jsx_get_context()`**（`modules/js/joy_js.h` + `jsx.c`，7 行）。
   `examples/js/src/main.cpp` 需要读写脚本全局变量（冒烟帧数、退出标志），而
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
> Android 还要把官方 runtime pack 的 4 个 `libSystem.*.so` 一起放进 `lib/<abi>/`：
> 该平台的 CoreLib 把 P/Invoke 目标拆成独立共享库，缺一个 C# APK 就会在
> MonoVM 启动时 `dlopen` 失败闪退（详见 `runtimes/README.md`）。

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
