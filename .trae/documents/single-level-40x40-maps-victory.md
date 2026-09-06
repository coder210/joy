# 单关大地图 + 通关界面 改造方案

## Context（背景）
当前循环是「多关自动推进」：单机每关清场后走出口即自动切到下一关，直到 4 关全部通关。用户要改成：
- **每场游戏只有一关，通关后禁止自动切关**；
- 地图要**更大更复杂**（现在只有 20×20 / 20 米）；
- 通关后进入一个**通关界面**（结算/返回）。

用户已确认：**地图扩到 40×40（40 米）**，**保留 4 张可选大地图**（选关页仍可选一张，清完即结算，不自动切）。

## 现状约束（已探明）
- 寻路网格常量集中在 `shared/components/pathfinding_component.h`：`PF_GRID_N=20`、`PF_GRID_MIN=-10`、`PF_GRID_OFFSET=10`、`PF_CELLS=400`。
- 世界坐标映射 `pf_grid_to_world(g) = g - OFFSET + 0.5`、`pf_world_to_grid(v) = floor(v)+OFFSET`，改 `OFFSET` 即整体平移/放大世界。
- 相机（`passes/camera_pass.cpp`）完全跟随玩家，扩图不破坏视角；`CAM_DIST=18` 可视范围偏小。
- 需联动改的硬编码（已逐一 grep 定位）：
  - floor 尺寸 20.0：`client.cpp` L337/338、L575/576；`server.cpp` L285/286、L420/421。
  - 障碍世界坐标 `x10 = c*10 - 95` / `z10 = r*10 - 95`：`shared/systems/level_maps.h` L175-176。
  - 敌人出生点 `gx - 10` / `gz - 10`：`shared/systems/wave_system.h` L138-139。
  - 小地图世界范围 `world = 20.0f`：`layers/debug_layer.cpp` L20。
  - A* 工作区 `AstarWork::MAX_CELLS = 400`：`shared/components/global_state_component.h` L31（扩到 1600 会溢出，必改）。
  - 障碍容量 `PM_MAX_OBS = 160`：`shared/systems/procedural_map.h` L23（大图墙体数易超，必增大）。
- 敌人数容量 `WAVE_MAX_ENEMIES = 64`（`shared/components/wave_advance_component.h`），每关敌人数表 `LEVEL_ENEMY_COUNT`（`level_maps.h` L121）。
- 场景切换用 `scene_manager_replace(ctx->mgr, scene)`；`reset_context_for_mode()` 不清空 `ctx->selected_level`，胜利后回选关页可保持选择。
- CMake 用 `file(GLOB "scenes/*.cpp")` 收集源文件，加新 .cpp 后需**重新 configure**（GLOB 在 configure 时才展开）。

## 改动清单

### A. 网格扩容 20 → 40
1. `shared/components/pathfinding_component.h`
   - `PF_GRID_N = 40`、`PF_GRID_MIN = -20`、`PF_GRID_OFFSET = 20`（`PF_CELLS` 自动变 1600）。
2. `shared/components/global_state_component.h`
   - `AstarWork::MAX_CELLS`：改为 `PF_CELLS`（经 `pathfinding_component.h`）。当前该文件未 include，补 `#include "pathfinding_component.h"`（无循环依赖，其仅依赖 jmath）。
3. `shared/components/pathfinding_component.h`（同属 A）
   - `PathComponent::path[256]` → `path[512]`：40×40 长路径可能超原数组（pathfinding_system 用 `path_len` 读取，无别的 256 硬编码边界则可直接放大数组）。
4. `shared/systems/procedural_map.h`
   - `PM_MAX_OBS 160 → 1024`（手绘大图墙容量；手绘关不走 jpm 引擎，仅 `PmLayout.obs[]` 需要足够容量）。
   - `PM_OFFSET10 100 → 200`（保持与 20m 偏移一致；仅程序化兜底用，顺手同步）。
5. 硬编码偏移修正：
   - `shared/systems/level_maps.h` L175-176：`o.x10 = c*10 - 95` → `c*10 - (PF_GRID_OFFSET*10 - 5)`；`o.z10` 同理用行 r。注释同步更新。
   - `shared/systems/wave_system.h` L138-139：`gx - 10` → `gx - PF_GRID_OFFSET`、`gz - 10` → `gz - PF_GRID_OFFSET`（需该文件可见 `PF_GRID_OFFSET`，其已含 `spawn_def.h`/`procedural_map.h`，若编译不过则补 include `pathfinding_component.h`）。
   - `layers/debug_layer.cpp` L20：`world = 20.0f` → `(float)PF_GRID_N`（该文件已含 `level_maps.h` → 含 `procedural_map.h` → 含 `pathfinding_component.h`，可直接用）。
   - `client.cpp`（L337-338、L575-576）与 `server.cpp`（L285-286、L420-421）：`r3d_mesh_plane(..., 20.0f, 20.0f, ...)` / `mesh_plane_wire` 的 `20.0f` → `(float)PF_GRID_N`（两文件均经 wave_system.h → level_maps.h 可见常量）。

### B. 去掉自动切关（核心：每场单关）
文件 `shared/systems/wave_system.h`：`wave_system` 末尾段（L253-263）当前会「最后一关 return / 否则判定到出口后 `apply_level_change(next)` 并广播 MAP_RELOAD」。改为：
- 删除 `if (gs.level >= LEVEL_COUNT) return;`、`player_at_exit` 判定、`apply_level_change(ctx, registry, next)`、以及 `net_server` 广播 MAP_RELOAD 整个段落。
- 保留：开局 `if (gs.wave==0 && !level_cleared) apply_level_change(..., gs.level)`（用于建图刷第一批敌）、清场置 `gs.level_cleared = true` 的逻辑。
- `player_at_exit` 与 `apply_level_change`（多关参数版）、`destroy_all_enemies`、`level_exit_position`：`apply_level_change` 仍被开局调用，**保留**；`player_at_exit` 不再被调用，**删除**（避免未用告警）。如确认别处无引用再删。
- 服务端 MAP_RELOAD 不再触发；协议封装 `pack/unpack_map_reload`、客户端 `handle_map_reload`/`PendingMapReload` 保留（代码无害，仅为不再触发），不做清理以免影响在线路径回归。

### C. 通关界面 victory_scene
1. 新建 `scenes/victory_scene.h/.cpp`，参照 `scenes/level_select_scene.cpp` 的 UI 结构（`r3d_button`、事件回调、绘制）：
   - 显示「通关！」大字、本关名/编号、击杀数（`gs.kills`）、出口达成提示。
   - 按钮：「再玩一次」（回到 loading 重进当前关）、「返回选关」（`scene_manager_replace(ctx->mgr, level_select_scene_create(ctx, ctx->mode))`）、「主菜单」（`main_menu_scene_create(ctx)`）。
   - 「再玩一次」复刻 level_select 的确认分支：`ctx->selected_level` 已保留当前关，走 `loading_scene_create(ctx, ctx->mode)` 重进。
2. `scenes/game_scene.h`：`game_scene` 增加 `float victory_timer = -1.0f;`（本局清场后累计延时再切）。
3. `scenes/game_scene.cpp` `on_update`：当本关已清场（`gs.level_cleared` 或本地推导「wave>0 且无存活敌人」）时累计 `victory_timer`，满 ~2.5s 后 `scene_manager_replace(ctx->mgr, victory_scene_create(ctx))`，让玩家看到最后击杀再结算。
   - 客户端本地用「存活敌人数==0 && 已开局」推导清场（与 `debug_layer::draw_minimap` 的解析一致），不依赖 `level_cleared`（它仅权威端置位），保证单机/在线体验一致。

### D. 大地图内容（工作量最大：内容创作）
文件 `shared/systems/level_maps.h`：
- `LEVEL_ROWS` / `LEVEL_COLS` 40 → 用 `PF_GRID_N`（或常量 40）。
- 重绘 4 张 **40×40** ASCII：每张含 1 个 `P`、出口 `X`（可通行）、若干 `E`（需 ≥ 该关敌人数）、`#` 墙（保证连通、边缘整圈封闭、密度适中、避免 1 格宽通道——寻路 pad 0.6m 会把贴墙缝隙判不可通行）。
- `LEVEL_ENEMY_COUNT`：大图"更复杂"，建议提升为 `{8, 12, 16, 18}`（均 ≤ 64，且各关 E 数 ≥ 对应值）。
- 选关页 `LEVEL_META`（`scenes/level_select_scene.cpp`）描述文案可微调以匹配大地图（可选）。

### E. 相机可视范围（可选但推荐）
`passes/camera_pass.cpp`：`CAM_DIST = 18` → `~28`，让 40 米图更"尽收眼底"（相机仍跟随玩家，不影响帧同步确定性）。

## 验证
1. 新文件加入后**重新 configure**：如 `cmake --build e:\c++\joy\build --target project_proxy3d --config Debug` 无法识别新 .cpp，先运行
   `cmake -S e:\c++\joy -B e:\c++\joy\build` 再 build（GLOB 重新展开）。
2. 运行 `build\Debug\project_proxy3d.exe`：
   - 选关页显示 4 关且敌人数正确；
   - 进第 1 关：地面铺满 40m、小地图映射正确、出生点/敌人/障碍位置正常、敌人能正常 A* 追玩家（验证扩网格无溢出/misalign）；
   - **清完本关敌人 → 约 2.5s 后进入通关界面**，**不会自动切到第 2 关**；
   - 通关界面「再玩一次 / 返回选关 / 主菜单」按钮可点击跳转。
3. 控制台确认无 `[MISMATCH]`、无数组越界告警。

## 备注
- 本方案以单机为主，victory 切换属流程层、各端本地推导清场后同步触发；在线路径不改协议，仅不再发 MAP_RELOAD（服务端已不触发切关）。
- 若在线端担心清场推导时机差，可在 client 端额外用服务端权威的 `level_cleared` 派生（随 checksum 统计字段），本次不引入。