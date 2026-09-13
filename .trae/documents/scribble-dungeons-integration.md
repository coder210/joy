# Scribble Dungeons 像素资源接入方案（project_proxy2d）

## Context（为什么做改动）

用户希望把 `project_proxy2d`（及 l4d_2d）现有"矢量几何绘制（白底墨线图纸风）"的表现，替换为 Kenney **Scribble Dungeons** 像素资源包（CC0、64×64 瓦片、黑白手绘涂鸦风）。

已和用户对齐的关键点：
- **核心诉求是"求生之路式游戏模式"**（单关单波、清怪→出口→过关、固定地图、20Hz 帧同步），**不是**必须丧尸/血腥。选 Scribble 是因其小清新涂鸦风 + 全套 CC0 可商用。
- 用户会**稍后把 Scribble 的 PNG 复制进项目**，本方案需标明资源放置路径与"素材就位后需确认"的占位点。
- **保留一切帧同步/逻辑/关卡/E循环，只改渲染表达**。

### 关键事实修正（决定方案走向）
- 渲染每格像素 = `SERVER_PPM = 50`（`project_proxy2d/protocol.h:10`），**不是 16px**。
  - `MAP_TILE_PX=16`（`systems/map_def.h:9`）只用于 Tiled 对象坐标→格换算，不参与渲染。
  - `drawing_map_pass.h`/`drawing_entity_pass.h` 全程用 `SERVER_PPM` 作为格边长/相机偏移/像素尺寸。
- 因此 Scribble 64×64 图块仅需缩到 50×50（0.78 倍），**线条仍清晰**，无需改逻辑分辨率。
- 逻辑分辨率 480×320（`main.cpp`），视野约 10×7 格。
- r2d 后端**支持贴图**：`image_create`(从文件) / `image_create_from_data`(从数据) / `image_draw1`(sheet切片) / `image_draw_ex`(带旋转/缩放/翻转)，见 `joy/joy.h:1115-1121`。

## 资源放置

```
project_proxy2d/assets/project_proxy2d/scribble/dungeon_tileset.png   # 地砖 sheet
project_proxy2d/assets/proxy2d/scribble/<角色图>.png                   # 玩家/敌人帧
```
- 运行时路径统一为 `"project_proxy2d/scribble/<name>.png"`（结构与现有 `project_proxy2d/textures/logo.png` 一致）。
- `CMakeLists.txt` 用 `file(GLOB_RECURSE ... "${ASSETS_ROOT}/*")` 递归拷贝整个 `assets/`，**新 PNG 自动收录，无需改 CMake**；增量构建未拷贝时需重跑一次 CMake configure。

## 实施步骤

### 1. joy 侧补最近邻缩放 API（唯一必须改 joy 的点）
当前 `image_draw1` 底层是 `SDL_RenderTexture`（`joy/jr2d.c:589`），默认线性过滤，64→50 会轻微糊。为保像素感，在 `joy/jr2d.c`（及 `joy/joy.h:1115-1121` 附近）新增：
```c
JOY_API void image_set_scale_mode(image_p img, int nearest /*1=NEAREST*/);
// 内部调用 SDL_SetTextureScaleMode(tex, nearest ? SDL_SCALEMODE_NEAREST : SDL_SCALEMODE_LINEAR)
```
- 若不想改 joy，可暂接受线性过滤（黑线仍可读），作为降级项。

### 2. `asset_manager` 增加贴图获取方法
`project_proxy2d/asset_manager.h`（及 `.cpp`）新增加方法，复用现有 `tex_cache_`（`std::unordered_map<string, image_p>`）：
```cpp
image_p GetImage(const char* path);        // 查缓存→命中直接返回，未命中 image_create + 写缓存
image_p GetTileAtlas();                    // 加载 Scribble 地砖大图（单例 atlas 常驻）
```
- 在 `game_scene` on_load / `loading_scene` 阶段一次加载到缓存，切关不重载。

### 3. `map_def.h` 加瓦片映射层
- 保留现有 `map_tile_type(char)`（`map_def.h:170-176`）。
- 新增纯声明性映射，**不改碰撞/逻辑**：
```cpp
struct TileSprite { int src_x, src_y; };   // 从 sheet 切哪个 64×64 方块
static TileSprite tile_sprite_of(int type); // WALL/ROCK/WATER/GRASS/FLOWER/PATH/TREE/EMPTY → 占位
```
- 初始阶段只填 `WALL`（墙块）+ 空地画地板；TREE/ROCK/WATER 等装饰 P2 再铺。
- **可选（富地图）**：`map_load_from_tilemap`（`map_def.h:108-110`）当前把任意非0 GID→`'#'`，要丰富地面/装饰需扩展为"GID→char"映射（依赖用户提供 GID 表，见风险 6.2）。

### 4. `drawing_map_pass.h` 整体替换为贴图绘制
- 保留网格裁剪逻辑（`:20-34`）。
- 把 `switch` 内每种 tile 的矢量绘制（`:36-110`）替换为 `image_draw1(atlas, &(TileSprite 的 srcrect 64×64), &(dest 50×50))`。
- `MAP_TILE_EMPTY` 画地板（或跳过留白）。

### 5. `drawing_entity_pass.h` 实体贴图化
- 玩家/敌人的"旋转方块+方向线"（`:164-218`）替换为 `image_draw_ex(角色图, src, 中心, rotation=dir_rad, scale=角色大小)`。
- 血量环 `draw_hp_ring`（`:38-86`）保留，仅调色/尺寸适配；子弹（`:125-157`）可暂留矢量。

### 6. 雾/小地图
- `fog_pass_2d.h`：**无改动**（雾盖在贴图之上天然成立）。
- `drawing_minimap_pass.h`：保持现状（2px 格矢量块），可选调色对齐。

## 分阶段实施

- **Phase 1（最小闭环）**：joy 最近邻 API + asset_manager 加载 atlas + `tile_sprite_of` 填 WALL/GROUND + `drawing_map_pass` 整替换。→ 进入任一关看到 Scribble 地板与墙铺满、碰撞/出生照旧、重进不崩。
- **Phase 2（实体）**：`drawing_entity_pass` 玩家/敌人换 Scribble 角色帧，朝向旋转沿用现有 `dir→dir_rad`（`:169-175`）。
- **Phase 3（观感统一 + 富地图）**：小地图调色；TREE/ROCK/WATER 装饰接入；若需再扩展 `.lua` GID→char 铺地面细节。

## 验证方式

每完成一个 phase 构建运行 `project_proxy2d`（VS 启动目录 `build/<CONFIG>`）：
1. Phase 1：进任意关，确认 Scribble 墙/地板铺满、最近邻下线条清晰、碰撞与出生照旧、R 重进不崩。
2. Phase 2：玩家/敌人可辨识、朝向正确、死亡消失正常；`fog2d_entity_visible` 仍工作。
3. Phase 3：全流程单关"清怪→出口→过关"像素风一致，未崩。
- 帧同步 hash（`state_checksum`）与渲染无关，可跳过。

## 风险 & 待确认项（素材就位后需用户提供）

- **6.1 Scribble 瓦片清单（必须）**：`dungeon_tileset.png` 尺寸、64×64 每块**排列序号**（哪块是墙/地板/洞窟/水/草/花/树/石）；角色文件每帧每方向的 `srcrect` 布局。未给前 `tile_sprite_of`/角色取帧只填占位常量。
- **6.2 `.lua` 关卡 GID 规划**：当前关卡只产 `'#'`（GID=1）。要铺 Scribble 地面/装饰需用户在 Tiled 里按 Scribble 铺并给 GID→语义表；否则 Phase 1 只能"墙块贴 Scribble、空地留白"。
- **6.3 最近邻 API**：需 joy 侧新增（唯一越界点）；不想改可用线性过滤降级。
- **6.4 角色朝向/镜像**：Scribble 角色多单方向帧，用 `image_draw_ex` 负 scale 翻转做镜像；上下/斜向可能需独立 sheet，取决于 6.1。
- **6.5 比例感知**：Scribble 角色通常半格~1格，与现有视觉半径/血环尺寸需在 Phase 2 匹配调整。
- **6.6 出口/门表现**：求生之路"过关出口"是否要用 Scribble 门/梯表现，由用户定后纳入 P3。

## Critical Files
- `joy/joy/jr2d.c` 及 `joy/joy/joy.h`（新增最近邻缩放，`SDL_RenderTexture` 在 `jr2d.c:589`）
- `project_proxy2d/asset_manager.h` / `.cpp`
- `project_proxy2d/systems/map_def.h`
- `project_proxy2d/passes/drawing_map_pass.h`
- `project_proxy2d/passes/drawing_entity_pass.h`
- （不动）`fog_pass_2d.h`、`drawing_minimap_pass.h`、`main.cpp`、`CMakeLists.txt`