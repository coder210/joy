# 把"无限刷波"改为"4关单波清场 + 出口推进"

## Context（背景）
当前 `wave_system` 仍是"敌人全灭 → `gs.wave++` → 同地图再刷下一波"的无限刷波逻辑，
`gs.level` 永不推进，所以：
- 无论选关页选哪关，进入游戏后都只玩那一张固定图 + 无限波次（"怎么还是什么波"）；
- 4 张手绘地图从未被顺序打完（"4 个地图也不符"）。

目标（用户已确认）：
1. 每关单波：进入关卡一次性刷出本关敌人；
2. 清完本关敌人 → 弹出"通关"提示 → **走到出口点** 进入下一关；
3. 选关页选中的关卡作为**起始关**，清场后顺序打向第 4 关；
4. 第 4 关清场后显示通关/胜利并返回选关页。

关键约束（沿用帧同步确定性）：换关时障碍变了，双端（服务端权威 + 客户端）必须用同一个
确定性函数重建障碍与敌人 —— 即都调 `level_build_layout(new_level)`，不通过网络下发障碍数组。

## 复用的现有工具
- `level_maps.h`：`LEVEL_COUNT=4`、`LEVEL_SRCS`、`level_build_layout(level,...)`、`level_player_spawn(level,&fx,&fz)`（镜像它实现出口点）。
- `spawn_obstacles_system.h`：`spawn_obstacles_from_layout`（障碍 ID 从 10000 起）、`destroy_obstacles`。
- `wave_system.h`：`wave_prepare_enemies`、`wave_spawn_enemy_range`、`enemy_reset`、`ENEMY_SPAWNS`。
- `protocol.h`：`pack_s2c_map_reload`/`unpack_map_reload`（改造成传新关卡号）。
- `server.cpp:251-262`：已通用的 MAP_RELOAD 广播（消费 `ctx->map_reload_data`）。

## 改动文件与要点

### 1. `shared/systems/level_maps.h`
- 在 4 张地图 `LEVEL_SRCS` 各加**恰好一个 `X`**（出口，替代一个 `.`，不新增障碍 → 障碍 ID 顺序不变，确定性安全）。出口选在远离 `P` 且可达的空地，注意寻路 pad 外扩 0.6m（两墙通道需 >1 格宽）。
- `level_build_layout` 增加出参 `int& exit_cell`（解析 `X`，扫描中记录 `r*PF_GRID_N+c`；`X` 归为可通行走法）。
- 新增 `level_exit_position(level, fp_t* fx, fp_t* fz)`（镜像 `level_player_spawn`），供出口检测/小地图/提示用。

### 2. `shared/components/global_state_component.h`
- 在 `GlobalState` 增加 `bool level_cleared = false;`（仅权威端使用，标识"本关敌人已清空、出口待触发"）。
- 客户端不依赖它，由渲染层用存活敌人数本地推导（见第 7 条）。

### 3. `shared/systems/wave_system.h`（核心重写）
- `wave_prepare_enemies`：把 `count = wave_enemy_count(gs.wave)` 改为按关取 `LEVEL_ENEMY_COUNT[level-1]`（`spawn_def.h` 的 `wave_enemy_count` 不再被这里使用）。
- 新增确定性函数：
  ```cpp
  static void apply_level_change(Context* ctx, entt::registry& reg, int new_level)
  {
      destroy_enemies(reg);            // 真销毁敌人实体（替换原来 no-op）
      destroy_obstacles(reg);
      auto& gs = ...GlobalState;
      gs.level = new_level; gs.wave = 0; gs.level_cleared = false;
      gs.map_wave_generated = -1; gs.map_seed = new_level;
      // 重建障碍（10000+ ID）
      PmLayout layout; int pc=-1, ec[WAVE_MAX_ENEMIES], en=0;
      if (!level_build_layout(new_level,&layout,pc,ec,en)) pmap_generate(ctx->random,&layout);
      int oid=10000; spawn_obstacles_from_layout(reg, ctx, layout, oid);
      // 玩家全部重定位到新关出生点（level_player_spawn，双端一致）
      for (auto pe : reg.view<PlayerComponent,LogicPositionComponent,TransformComponent>()) { ...置新 P, 更新 SpawnPoint/Transform... }
      // 刷第一批敌（直接生成，非 WaveAdvance）
      int tiles[WAVE_MAX_ENEMIES], si[WAVE_MAX_ENEMIES];
      int got = wave_prepare_enemies(gs, new_level, tiles, si);
      wave_spawn_enemy_range(ctx, tiles, si, 0, got);
      gs.wave = 1;
      ctx->sfx_requests.push_back({SFX_WAVE});   // 开局/换关音效
      log_info(...);
  }
  ```
  > `apply_level_change` 在服务端（权威）和客户端（收到 MAP_RELOAD 时）各跑一次，输入只有 `new_level`，
  > 流程纯确定性 → 双端障碍/敌人/玩家位置逐位一致，符合帧同步。
- 重写 `wave_system` 状态机（服务端权威，注册位置不变）：
  ```
  if (!ctx->is_authority || !ctx->random) return;
  gs = ...;
  if (gs.wave == 0 && !gs.level_cleared) { apply_level_change(gs.level); /*首帧开局*/ return; }
  alive = 统计存活敌人;  if (alive > 0) return;
  if (!gs.level_cleared) { gs.level_cleared = true; return; }   // 清场完成，等待走向出口
  // 已清场：检测玩家到达出口
  if (player_at_exit(ctx, gs)) {          // 距离出口 < 1.5m（用定点 fp）
      if (gs.level < LEVEL_COUNT) {
          int next = gs.level + 1;
          apply_level_change(next);
          if (ctx->net_server) ctx->map_reload_data = pack_s2c_map_reload(frame_id, next);
      } // 最后一关：清场即胜利，UI 在渲染层提示并返回选关（不再推进）
  }
  ```
- 删除原 `gs.wave++` / `level_index_for_wave` 依赖的无限推进与 `WaveAdvanceComponent` 生成分支。

### 4. `shared/protocol.h`
- `pack_s2c_map_reload(frame_id, level)`：第二项从 `wave` 改成 `level`（语义=换关到第 N 关）。
- `unpack_map_reload(...)`：第二项解到 `level`。

### 5. `context.h`
- `PendingMapReload{ int level; }`（把 `wave` 字段换成 `level`）。

### 6. `client.cpp`
- `handle_map_reload(ctx, level)`：调 `apply_level_change(ctx, ctx->ecs, level)` 重建障碍+敌人+重定位玩家（确定性，与权威端一致）；删掉原先创建 `WaveAdvanceComponent` 的分支。
- `handle_loading`：删掉 `wave_delay` 创建 `WaveAdvanceComponent` 的块（新模型无波次延迟）。
- `client_fixed_update` 里 `rit->second.wave` → `rit->second.level`，并把 `handle_map_reload` 调用参数同步改。

### 7. HUD / 小地图：`layers/debug_layer.cpp`
- 战况大字从 `"Wave %d"` 改为 `"第 %d 关"`（`gs.level`）。
- 本地推导"清场"：`bool cleared = (alive==0 && gs.wave>0);`
  - `cleared && gs.level<LEVEL_COUNT` → 中央上方提示 `"通关！前往出口"`（墨色大字，可复用 `draw_wave_hint` 的绘制样式）。
  - `cleared && gs.level==LEVEL_COUNT` → 居中大标题 `"通关成功"` + 小字 `"按任意键返回选关"`（胜利画面）。
- `draw_minimap`：按 `level_exit_position(gs.level)` 画出口标记（方形）：未激活灰、清场后激活金/绿色。

### 8. `scenes/game_scene.cpp`
- 胜利返回：`on_event` 中，当本地推导"通关成功"（`gs.level==LEVEL_COUNT && 存活敌人==0`）时，任意键 → `scene_manager_replace(ctx->mgr, level_select_scene_get_scene(level_select_scene_create(ctx, ctx->mode)))`（新增 include level_select_scene.h）。开始时 `selected_level` 已有当前起始关，返回选关页能直接看到进度。
  > 场景切换属 UI 层，不参与模拟，不影响双端确定性。返回后再进入游戏会经 `loading_scene` 调 `reset_context_for_mode` 重建 ECS（现有机制，安全）。

### 9. `server.cpp`
- MAP_RELOAD 广播（`:251-262`）保持不变（已是通用消费 `ctx->map_reload_data`），只是数据里现在是新关卡号。

## 不做的事（避免越界）
- 不重画 4 张地图的墙/刷怪布局，仅新增出口 `X`。
- 不加敌人掉落/货币/商店；通关奖励维持"过关注入下一关 + 轻微加成"的范围（本版不动数值）。
- 不新增音效资源；通关/清场用现有 `SFX_WAVE` 触发。

## 确定性/帧同步注意点
- 换关重建障碍用固定 `oid=10000` 起、敌人用 `get_entity_id` 递增；两端执行同一 `apply_level_change` 且删除/新建实体次数一致 → ID 逐位对齐，不会破坏 checksum。
- `X` 用 `.` 替换、`X` 归为可通行，不改变障碍实体数 → 首波障碍 ID 不变，旧客户端快照仍兼容。
- 客户端"清场/通关"提示是渲染层按存活敌人数本地推导，不落进 `GlobalState.level_cleared`（该字段仅权威端驱动流转），避免非权威端误改模拟状态。

## 验证
1. `joy_editor` 无需改动；重新编译 `project_proxy3d`（单机）。
2. 单机：选第 1 关进入 → 应刷出第 1 关敌人（数量按 `LEVEL_ENEMY_COUNT[0]`），HUD 显示"第 1 关"；
   清完 → 屏幕提示"通关！前往出口"，小地图出口点亮；走进出口 → 切到"第 2 关"地图（障碍/刷怪变化、玩家落在新 P）。
3. 顺序打到第 4 关清场 → 显示"通关成功"，按任意键回到选关页。
4. 选第 2 关作为起始：应从第 2 关打到第 4 关。
5. 联机（Host+Client 双端）：换关时客户端经 MAP_RELOAD 在同一逻辑帧重建，`checksum` 两端一致（退出后 diff `checksum_log*.txt` 无 MISMATCH）。

（胜利后的去向当前按"提示后返回选关页"实现；若你希望回主菜单或循环无尽模式，改动只在第 7/8 条，稍后可调。）