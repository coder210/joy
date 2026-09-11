#ifndef CONTEXT_H
#define CONTEXT_H
#include <map>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include "entt.hpp"
#include <joy/joy.h>
#include "protocol.h"
#include "components/action_state_component.h"
#include "components/id_component.h"
#include "components/ai_component.h"
#include "components/logic_rect_component.h"
#include "components/logic_position_component.h"
#include "components/logic_velocity_component.h"
#include "components/enemy_component.h"
#include "components/player_state_input.h"
#include "components/player_event_input.h"
#include "components/player_component.h"
#include "components/transform_component.h"
#include "components/net_component.h"
#include "components/local_only_component.h"
#include "components/global_state_component.h"
#include "systems/state_checksum.h"
#include "components/connection_component.h"

// 状态快照（用于预测回滚）
struct state_snapshot {
        fp_t position_x, position_y;
};

// 用户设置（自动保存到 settings.cfg，切换模式/进出场景时保留）
struct Settings {
        char  server_ip[64] = "192.168.2.28";
        int   server_port = 30000;
        float bgm_volume = 0.5f;   // 背景音乐音量 0~1
        float sfx_volume = 1.0f;   // 音效音量 0~1
        bool  fullscreen = false;
};

// 保存设置到 settings.cfg（key=value 文本格式）
static void settings_save(const Settings* s)
{
        FILE* fp = fopen("settings.cfg", "w");
        if (!fp) return;
        fprintf(fp, "server_ip=%s\n", s->server_ip);
        fprintf(fp, "server_port=%d\n", s->server_port);
        fprintf(fp, "bgm_volume=%f\n", s->bgm_volume);
        fprintf(fp, "sfx_volume=%f\n", s->sfx_volume);
        fprintf(fp, "fullscreen=%d\n", s->fullscreen ? 1 : 0);
        fclose(fp);
}

// 从 settings.cfg 加载设置（文件不存在时保留默认值）
static void settings_load(Settings* s)
{
        FILE* fp = fopen("settings.cfg", "r");
        if (!fp) return;
        char key[64], value[128];
        // 格式串前导空格跳过行首换行/空白，否则第二行起 key 会带上 '\n' 导致匹配失败
        while (fscanf(fp, " %63[^=]=%127s", key, value) == 2) {
                if (strcmp(key, "server_ip") == 0) {
                        snprintf(s->server_ip, sizeof(s->server_ip), "%s", value);
                } else if (strcmp(key, "server_port") == 0) {
                        s->server_port = atoi(value);
                } else if (strcmp(key, "bgm_volume") == 0) {
                        s->bgm_volume = (float)atof(value);
                } else if (strcmp(key, "sfx_volume") == 0) {
                        s->sfx_volume = (float)atof(value);
                } else if (strcmp(key, "fullscreen") == 0) {
                        s->fullscreen = (atoi(value) != 0);
                }
        }
        fclose(fp);
}

// 运行模式（单二进制，运行时通过 --mode 参数切换）
enum GameMode {
        MODE_SINGLE = 0,   // 单机：本地推逻辑，不连网络
        MODE_HOST,         // 房主：本地推逻辑 + 广播给其他玩家
        MODE_CLIENT,       // 客户端：连服务端/Host，消费命令 + 渲染
        MODE_SERVER,       // 独立服务端：只推逻辑 + 转发，无本地玩家
};

// 用户自定义事件类型
#define TRANSITION_EVENT (0x6)

// 跨场景过渡事件码
enum transition_event_code {
        TRANSITION_SPLASH_DONE = 1,
        TRANSITION_LOADING_DONE = 2,
};

// 通关事件（game_scene 判定全清后推送，main.cpp 切到胜利场景）
#define VICTORY_EVENT (0x8)
enum victory_event_code {
        VICTORY_EVENT_TRIGGERED = 1,
};

struct Context {
        GameMode mode = MODE_SINGLE;

        Settings settings;   // 用户设置（切换模式/进出场景时保留）

        bool running;
        game_timer_t game_timer;
        jwindow_p window;
        jr2d_p r2d;
        scene_manager_p mgr;  // 场景管理器（由 main.cpp 创建后赋值）
        scene_p pending_scene;  // 过渡场景（由 splash/loading 场景设置，事件处理中执行替换）
        netclient_p netclient;
        netserver_p net_server;
        entt::registry ecs;
        entt::entity singleton_net = entt::null;

        // ---- 音频 ----
        uint32_t            audio_dev = 0;
        audio_shot_p        step_sound = NULL;
        audio_bgm_p         bgm = NULL;
        int local_dir = 0; // 本地玩家方向
        // 远程玩家方向追踪
        //std::map<entt::entity, int>  remote_dirs;

        std::map<int, int> server_inputs;
        bool ready = false;
        int server_frameid = 0;

        // 固定逻辑步
        uint64_t lastTime = get_ticks();

        int current_input_mask = 0;
        bool attack_triggered = false;          // 按钮 tap 近战，发送一次后自动复位
        bool bullet_triggered = false;          // "射"按钮 tap，发送一次后自动复位
        bool ray_triggered = false;             // "线"按钮 tap，发送一次后自动复位
        float heartbeatTimer = 0.0f;

        std::queue<std::string> command_queue;

        // ========== 预测回滚相关 ==========
        uint32_t local_conv = 0;                     // 本地玩家的 conv（连接标识）
        std::map<int, std::map<int, state_snapshot>> snapshots;        // 按 frame_id 递增存储的历史快照
        std::vector<int> pending_inputs;     // 已发送但未确认的输入

        // ---- 固定步长逻辑 ----
        float accumulator = 0.0f;
        float FIXED_TIMESTEP = 1.0f / 20.0f;

        // ========== 帧同步校验记录缓冲（性能优化：不实时打印，退出时落盘）==========
        // 每帧一条：帧号、服务端下发 checksum、本地校验 checksum、本帧各玩家输入、本帧实体快照。
        // 正常帧静默追加，仅 MISMATCH 帧实时 log_error 告警；全部攒在内存，退出时写文件。
        // 结构定义见 state_checksum.h（client/server 共用同一 ChecksumRecord）。
        std::vector<ChecksumRecord> checksum_records;

        // 视觉时间（仅渲染用，不参与帧同步）：
        // 驱动"浑天仪式"轨道环的自旋/变形相位
        float visual_time = 0.0f;

        // system 注册
        std::vector<struct SystemEntry> systems;

        // ========== 服务端 / Host 字段（命令收集与转发）==========
        std::vector<S2CPlayerJoin> player_joins;
        std::vector<S2CPlayerLeave> player_leaves;
        std::vector<S2CPlayerInput> player_inputs;

        std::map<int, std::string> commands;          // 以帧号为键的命令帧缓冲（补发用）
        std::map<int, std::string> world_snapshots;   // 以帧号为键的世界快照（中途加入用）

        S2CEntity entities[512];
        int num_entities = 0;

        // ========== 角色/关卡选择（跨场景保留，loading 由 reset_context_for_mode 不清除）==========
        int  selected_char_id  = 0;                    // 选中角色（CHAR_WARRIOR 默认）
        int  selected_level    = 1;                    // 选中关卡（默认第 1 关）
        char selected_nickname[32] = "玩家";            // 玩家昵称
};

// ========== System 调度基础设施 ==========
enum Phase { OnStart, PreUpdate, OnUpdate, PostUpdate };

struct SystemEntry {
        std::string name;
        Phase phase;
        std::function<void(entt::registry&, Context*, float)> fn;
};

inline void add_system(Context* ctx, Phase phase, const char* name,
        void (*fn)(entt::registry&, Context*, float))
{
        ctx->systems.push_back({ name, phase, fn });
}

inline void run_phase(Context* ctx, Phase phase, float dt)
{
    // ---- 临时剖析：仅对 OnUpdate 逐个系统计时，秒级打印（不影响帧同步逻辑）----
    static double   s_ms[128] = {0};
    static long     s_n[128]  = {0};
    static uint32_t s_t0 = 0;
    static int      s_idx_rp = 0;
    if (phase == OnUpdate) s_idx_rp = 0;
    for (auto& sys : ctx->systems) {
        if (sys.phase != phase) continue;
        auto t0 = std::chrono::high_resolution_clock::now();
        sys.fn(ctx->ecs, ctx, dt);
        auto t1 = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        if (phase == OnUpdate && s_idx_rp < 128) {
            s_ms[s_idx_rp] += ms; s_n[s_idx_rp]++; s_idx_rp++;
        }
    }
    if (phase == OnUpdate) {
        uint32_t now = get_ticks();
        if (s_t0 == 0) s_t0 = now;
        if (now - s_t0 >= 1000) {
            int i = 0;
            for (auto& sys : ctx->systems) {
                if (sys.phase != OnUpdate) continue;
                if (i < 128 && s_n[i] > 0) {
                    fprintf(stderr, "[prof] %-14s calls=%ld total=%.1fms avg=%.3fms\n",
                        sys.name.c_str(), s_n[i], s_ms[i], s_ms[i] / s_n[i]);
                }
                i++;
            }
            for (int k = 0; k < 128; k++) { s_ms[k] = 0; s_n[k] = 0; }
            s_t0 = now; s_idx_rp = 0;
        }
    }
}


// 全局实体 id 分配：统一从 GlobalState.next_entity_id 取。
// 服务端/Host/单机为权威分配；客户端在 handle_loading 时把 next_entity_id 设为
// 服务端下发的 world_eid（已跳过敌人占用的 1..N 区间），保证两端 id 序列对齐。
// 注意：两端必须在每个 tick 以相同顺序、相同次数调用 get_entity_id（玩家 join、子弹发射等），
// 否则 id 序列会错位导致帧同步 mismatch——这是帧同步的固有前提，不是 id 起点问题。
static int get_entity_id(Context* ctx)
{
    auto& gs = ctx->ecs.ctx().get<GlobalState>();
    return gs.next_entity_id++;
}

// 运行时切换模式：重置所有瞬时状态（ECS/系统注册/网络/命令缓冲），
// 保证可反复进出游戏场景而不重复注册系统或重复 emplace 单例组件。
static void reset_context_for_mode(Context* ctx, GameMode mode)
{
    if (ctx->netclient) { netclient_destroy(ctx->netclient); ctx->netclient = NULL; }
    if (ctx->net_server) { netserver_destroy(ctx->net_server); ctx->net_server = NULL; }

    ctx->ecs = entt::registry();   // 全新 registry（含 ctx() 上下文，避免重复 emplace 报错）
    ctx->systems.clear();

    ctx->command_queue = std::queue<std::string>();
    ctx->commands.clear();
    ctx->world_snapshots.clear();
    ctx->player_joins.clear();
    ctx->player_leaves.clear();
    ctx->player_inputs.clear();
    ctx->server_inputs.clear();
    ctx->snapshots.clear();
    ctx->pending_inputs.clear();
    ctx->checksum_records.clear();

    ctx->ready = false;
    ctx->server_frameid = 0;
    ctx->local_conv = 0;
    ctx->num_entities = 0;
    ctx->accumulator = 0.0f;
    ctx->current_input_mask = 0;
    ctx->attack_triggered = false;
    ctx->bullet_triggered = false;
    ctx->ray_triggered = false;
    ctx->local_dir = 0;
    ctx->heartbeatTimer = 0.0f;
    ctx->visual_time = 0.0f;

    ctx->mode = mode;
}


#endif
