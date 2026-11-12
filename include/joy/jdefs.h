/* 公共定义：常量、枚举、内联工具与基础 typedef
 * 由 joy.h 按行段拆出，聚合入口见 joy.h；改动请在对应分域头进行。 */
#ifndef JOY_DEFS_H
#define JOY_DEFS_H

/* 系统头与 jmath.h 必须在 extern "C" 之外：C++ 下它们各自带链接声明，
 * 与拆分前 joy.h 的 include 顺序保持一致。 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <joy/jconfig.h>
#include <joy/jmath.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 公共头不再包含任何 SDL 头：事件/输入统一走 joy_input_* 标量状态快照，
 * SDL_Event 只在 src/ 的平台适配代码里出现。原先这里放的是从 SDL3 枚举
 * 抄来的 JOY_EVENT_* 数值常量，全仓零使用，已移除；适配层若需要判断事件
 * 类型，应在对应 .c 内包含 <SDL3/SDL_events.h> 并直接使用 SDL_EVENT_*。
 * 这里保留与原来相同的行数，避免打乱 joy.idl 既有条目的 source 行号。
 *
 * 公开头改用标准整数类型（uint8_t 等），不再借用 SDL 的 Uint8 别名。
 *
 */

/* ---------- 混合模式 ---------- */
#define JR2D_BLEND_NONE  0
#define JR2D_BLEND_BLEND 1
#define JR2D_BLEND_ADD   2
#define JR2D_BLEND_MOD   3

/* ---------- 翻转模式 ---------- */
#define JR2D_FLIP_NONE 0
#define JR2D_FLIP_H    1
#define JR2D_FLIP_V    2
#define JR2D_FLIP_HV   3

/* ===================== 单位换算（米 ↔ 像素） ===================== */
/* 1 米 = PIXELS_PER_METER 屏幕像素；渲染坐标 = 世界坐标(米) × PIXELS_PER_METER。
 * 作为唯一真源放入 joy，project_proxy2d / adventure2d 共用，避免重复定义。
 *
 * 允许单个项目在编译期覆盖（默认 50，不覆盖时行为与以前完全一致）：
 *   逻辑分辨率提高后，若要保持"可视世界范围 + 角色显示大小"不变，
 *   需让 PPM 同比例提高 —— 例如 480x320 → 960x640 时配 -DPIXELS_PER_METER=100。 */
#ifndef PIXELS_PER_METER
enum { PIXELS_PER_METER = 50 };
#endif

/* 逻辑分辨率呈现模式（对标 SDL_RendererLogicalPresentation）。
 * 项目层用这些常量，避免直接依赖 SDL render 头。 */
#define JOY_LOGICAL_PRESENTATION_DISABLED       0
#define JOY_LOGICAL_PRESENTATION_STRETCH        1
#define JOY_LOGICAL_PRESENTATION_LETTERBOX      2
#define JOY_LOGICAL_PRESENTATION_INTEGER_SCALE  3

/* 世界坐标(米) ↔ 屏幕像素 互换；参数为浮点，避免整数截断。 */
static inline float meters_to_pixels(float meters) { return meters * (float)PIXELS_PER_METER; }
static inline float pixels_to_meters(float pixels) { return pixels / (float)PIXELS_PER_METER; }

#define JOY_POPUP_MAX_TITLE 128
#define JOY_POPUP_MAX_CONTENT 2048
#define JOY_TOOLTIP_MAX_TEXT 256

/* ---------- 分层类型常量 ---------- */
#define JTILEMAP_LAYER_TILE     0
#define JTILEMAP_LAYER_OBJECT   1
#define JTILEMAP_LAYER_IMAGE    2

/* ---------- 形状常量 ---------- */
#define JTILEMAP_SHAPE_RECT     0
#define JTILEMAP_SHAPE_POLYGON  1
#define JTILEMAP_SHAPE_POLYLINE 2
#define JTILEMAP_SHAPE_ELLIPSE  3
#define JTILEMAP_SHAPE_POINT    4

/* ---------- GID 翻转标志（Tiled 规范，编码在 GID 高位） ---------- */
#define JTILEMAP_FLIP_H       0x80000000
#define JTILEMAP_FLIP_V       0x40000000
#define JTILEMAP_FLIP_D       0x20000000
#define JTILEMAP_GID_MASK     0x1FFFFFFF

#define JOY_ACCUMULATE_IMPULSES 1
#define JOY_WARM_STARTING 1
#define JOY_POSITION_CORRECTION 1
#define JOY_MAX_POINTS 2


#define jVERTOR_INIT(name, type) \
typedef struct joy_vector_##name { \
        type *a; \
        int m; \
        int n; \
}jvector_##name##_t, *jvector_##name##_p; \
\
static void jvector_init_##name(jvector_##name##_p vec) \
{\
        vec->n = 0;\
        vec->m = 32;\
        vec->a = (type *)malloc(sizeof(type) * vec->m);\
}\
\
static void jvector_push_##name(jvector_##name##_p vec, type id) \
{\
        if (vec->n >= vec->m) {\
                vec->a = (type *)realloc(vec->a, vec->m * 2);\
        }\
        vec->a[vec->n++] = id;\
}\
\
static type jvector_a_##name(jvector_##name##_p vec, int i)\
{\
        if (vec->n > i) {\
                return vec->a[i];\
        }\
        else {\
                return 0;\
        }\
}\
\
static int jvector_size_##name(jvector_##name##_p vec) \
{\
        return vec->n;\
}\
\
static void jvector_clear_##name(jvector_##name##_p vec) \
{\
        vec->n = 0;\
}\
\
static void jvector_destroy_##name(jvector_##name##_p vec) \
{\
        vec->n = vec->m = 0;\
        free(vec->a);\
}\

#define jvector_t(name) jvector_##name##_t
#define jvector_p(name) jvector_##name##_p
#define jvector_init(name, vec) jvector_init_##name(vec)
#define jvector_push(name, vec, id) jvector_push_##name(vec, id)
#define jvector_a(name, vec, i) jvector_a_##name(vec, i)
#define jvector_size(name, vec) jvector_size_##name(vec)
#define jvector_clear(name, vec) jvector_clear_##name(vec)
#define jvector_destroy(name, vec) jvector_destroy_##name(vec)



        typedef struct joy_delay {
                uint64_t time;
                short timeout;
        } joy_delay_t;

        typedef struct joy_random_num joy_random_num_t, * joy_random_num_p;

        // Mersenne Twister 内部状态大小（624 个 uint32 + 1 个 index）
        #define JOY_MT_N 624

        // 可重置随机数（Mersenne Twister），用于帧同步保证 server/client 随机序列一致
        JOY_API joy_random_num_p joy_core_utils_random_create(uint32_t seed);
        JOY_API void         joy_core_utils_random_destroy(joy_random_num_p rnum);
        JOY_API uint32_t     joy_core_utils_random_next(joy_random_num_p rnum);
        // 导出/恢复随机数内部状态（帧同步：客户端 join 时需用服务端当前状态恢复，
        // 仅同步 seed 会让已推进的 random 与初始状态不同步，导致 AI 随机序列分叉）
        JOY_API void         joy_core_utils_random_get_state(joy_random_num_p rnum, uint32_t* mt_out, int* index_out);
        JOY_API void         joy_core_utils_random_set_state(joy_random_num_p rnum, const uint32_t* mt_in, int index_in);

        typedef struct joy_lcg_num joy_lcg_num_t, * joy_lcg_num_p;

        // 单状态线性同余随机数（LCG，64 位状态）。状态仅 1 个 uint64，
        // 帧同步时"每帧快照"只需同步 8 字节，比 Mersenne Twister（mt[624]+index≈2.5KB）
        // 小得多。用法与 utils_random_* 一致。
        JOY_API joy_lcg_num_p   joy_core_lcg_random_create(uint64_t seed);
        JOY_API void        joy_core_lcg_random_destroy(joy_lcg_num_p lcg);
        JOY_API uint32_t    joy_core_lcg_random_next(joy_lcg_num_p lcg);
        // 导出/恢复 LCG 当前状态（单值）。帧同步：客户端 join 时用服务端当前状态恢复，
        // 保证两端从同一推进位置继续，随机序列一致。
        JOY_API void        joy_core_lcg_random_get_state(joy_lcg_num_p lcg, uint64_t* state_out);
        JOY_API void        joy_core_lcg_random_set_state(joy_lcg_num_p lcg, uint64_t state);

        // ============ 泛洪聚团程序化地图生成（jprocmap） ============
        // 程序化生成"带连片掩体的开放竞技场"地图：若干锚点（开放且远离出生安全区的格）
        // 各自做有限泛洪，随机成长成 5~10 格连片的掩体群，中间留开放通道；生成后从玩家
        // 主区做多源 BFS 连通性修复，把封死通道的整块障碍移除，保证所有开放格可达。
        // 只由 seed 决定 → 完全确定性，可用作帧同步依据。所有坐标用"十分之一米"整数。
        //
        // 参数全部可配置（网格尺寸、出生点/安全区、障碍密度/高度、簇大小范围）。

        // 网格/容量上限（固定数组，避免动态分配；实际尺寸由 joy_pm_config_t.grid_n 决定）
        #define JPM_MAX_GRID_N 128         // 网格边长上限（格，对齐 128×128 手绘大图容量）
        #define JPM_MAX_CELLS  (JPM_MAX_GRID_N*JPM_MAX_GRID_N)
        #define JPM_MAX_OBS    160         // 障碍物数量上限
        #define JPM_MAX_SPAWN  8           // 出生安全区数量上限

        // 单个障碍实体（世界坐标用十分之一米整数）
        typedef struct joy_pm_obstacle {
                int x10, z10;    // 中心坐标（十分之一米，格子对齐）
                int hx10, hz10;  // XZ 半尺寸（十分之一米，单格 = 5）
                int sy10;        // 高度（十分之一米）
                int gx0, gz0;    // 占用格子（左/下角格）
                int gw, gh;      // 占用格子数（宽/深，恒 1）
        } joy_pm_obstacle_t, * joy_pm_obstacle_p;

        // 出生安全区：以世界坐标（十分之一米）为圆心、radius10 为半径的保护区
        typedef struct joy_pm_spawn {
                int cx10, cz10;  // 安全区圆心（世界坐标，十分之一米）
                int radius10;    // 安全区半径（十分之一米）
        } joy_pm_spawn_t;

        // 生成配置（可配置参数集中于此）。BSP 模式复用 density 作为房间分块粒度的参考
        typedef struct joy_pm_config {
                int grid_n;        // 网格边长（格），默认 20
                int offset10;      // 世界坐标 → 网格坐标平移（十分之一米），默认 100（10m）
                int pad10;         // 障碍外扩边距（十分之一米），默认 6（0.6m）
                int density;       // 目标障碍密度（%），默认 25
                int max_obs;       // 障碍物数量上限，默认 160
                int height_min10;  // 障碍最低高度（十分之一米），默认 5（0.5m）
                int height_max10;  // 障碍最高高度（十分之一米），默认 18（1.8m）
                int cluster_min;   // 每簇最小格数，默认 5
                int cluster_max;   // 每簇最大格数，默认 10
                int spawn_count;               // 出生安全区数量
                joy_pm_spawn_t spawns[JPM_MAX_SPAWN];  // 出生安全区列表（spawns[0] = 玩家区）
        } joy_pm_config_t, * joy_pm_config_p;

        // 生成结果布局
        typedef struct joy_pm_layout {
                joy_pm_obstacle_t obs[JPM_MAX_OBS];
                int  count;                     // 实际障碍数
                int  seed;                      // 实际使用的生成 seed（调试用）
                bool walkable[JPM_MAX_CELLS];   // 生成后的可通行栅格（含 pad 外扩）
        } joy_pm_layout_t, * joy_pm_layout_p;

        // 默认配置：填充一组与游戏一致的默认参数（20×20、玩家(0,0)+敌人(10m,10m) 安全区等）
        JOY_API void joy_scene_procmap_config_default(joy_pm_config_t* cfg);
        // 追加一个出生安全区（世界坐标圆心 + 半径，十分之一米）
        JOY_API void joy_scene_procmap_config_add_spawn(joy_pm_config_p cfg, int cx10, int cz10, int radius10);
        // 波次换图 seed：由初始 seed 与波次号确定性派生（纯函数，不依赖随机流）
        JOY_API uint32_t joy_scene_procmap_wave_seed(uint32_t base_seed, int wave);
        // 由给定 seed 确定性生成布局
        JOY_API void joy_scene_procmap_generate_seed(const joy_pm_config_t* cfg, uint32_t seed, joy_pm_layout_t* out);
        // 从上层随机流抽取 1 个 seed 生成布局（初始生成用）
        JOY_API void joy_scene_procmap_generate(const joy_pm_config_t* cfg, joy_random_num_p rng, joy_pm_layout_t* out);
        // 整数栅格化：按配置从障碍实体重建可通行栅格（含 pad 外扩）
        JOY_API void joy_scene_procmap_rasterize(const joy_pm_config_t* cfg, bool walkable[JPM_MAX_CELLS],
                                   const joy_pm_obstacle_t* obs, int n);
        // 4 方向 BFS：返回从起点可达的格子总数（用于连通性校验）
        JOY_API int joy_scene_procmap_bfs(const bool walkable[JPM_MAX_CELLS], int grid_n, int sx, int sz);
        // 从布局开放格中随机采样 count 个格子（洗牌取前 count），返回实际采样数；
        // min_dist10 限制距玩家区（spawns[0] 圆心）的最小距离（十分之一米）。
        JOY_API int joy_scene_procmap_sample_open_tiles(joy_random_num_p rng, const joy_pm_config_t* cfg,
                                          const joy_pm_layout_t* L, int count, int min_dist10,
                                          int* out_tiles);

        typedef void (*jtimer_callback)(int actual_interval, int interval, void* userdata);
        typedef struct joy_timer joy_timer_t, *joy_timer_p;

        typedef union jwindow joy_window_t, * joy_window_p;

        typedef void (*jevent_keypressed_fn)(const char* key, int scancode, bool repeat, bool ctrl, bool shift, bool alt, void* arg);
        typedef void (*jevent_keyreleased_fn)(const char* key, int scancode, void* arg);
        typedef void (*jevent_mousepressed_fn)(float x, float y, int button, void* arg);
        typedef void (*jevent_mousereleased_fn)(float x, float y, int button, void* arg);
        typedef void (*jevent_mousemoved_fn)(float x, float y, float dx, float dy, void* arg);
        typedef void (*jevent_textinput_fn)(const char* text, void* arg);
        typedef void (*jevent_resize_fn)(int w, int h, void* arg);
        typedef void (*jevent_process_event_fn)(const void* event, void* arg);

        typedef struct joy_event_callbacks {
                jevent_keypressed_fn on_keypressed;
                jevent_keyreleased_fn on_keyreleased;
                jevent_mousepressed_fn on_mousepressed;
                jevent_mousereleased_fn on_mousereleased;
                jevent_mousemoved_fn on_mousemoved;
                jevent_textinput_fn on_textinput;
                jevent_resize_fn on_resize;
                jevent_process_event_fn on_process_event;
        } joy_event_callbacks_t;

        typedef struct joy_env joy_env_t, * joy_env_p;
        typedef struct joy_simple_fps_counter joy_simple_fps_counter_t, * joy_simple_fps_counter_p;
        typedef struct joy_game_timer {
                int64_t last_time;          // 上一帧结束时的纳秒时间戳
                int64_t frame_start_time;   // 当前帧开始时的纳秒时间戳
                double delta_time;         // 未缩放的帧间隔时间（秒）
                double time_scale;         // 时间缩放因子
                int target_fps;            // 目标 FPS（0 表示不限制）
                double target_frame_time;  // 目标每帧时间（秒）
        } joy_game_timer_t, * joy_game_timer_p;

        typedef struct joy_rect
        {
                float x;
                float y;
                float w;
                float h;
        } joy_rect_t;

        typedef struct joy_color
        {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                uint8_t a;
        } joy_color_t;


        typedef struct joy_iocp_server joy_iocp_server_t, * joy_iocp_server_p;

        typedef struct joy_font joy_font_t, * joy_font_p;
        typedef struct joy_r2d joy_r2d_t, * joy_r2d_p;
        typedef struct joy_image joy_image_t, * joy_image_p;
        typedef struct joy_text joy_text_t, * joy_text_p;
        typedef struct joy_animation joy_animation_t, * joy_animation_p;
        typedef struct joy_layout_grid joy_layout_grid_t;   // 栅格布局（见下方 jlayout_*）
        typedef struct joy_r3d   joy_r3d_t, * joy_r3d_p;          /* 3D 渲染上下文 */
        typedef struct joy_render_mesh joy_render_mesh_t, * joy_render_mesh_p;  /* 网格 */

        /* jr2d 内部几何绘制原语，供 jlight.c 复用（替代原先的跨文件 extern 前向声明）。
         * 非 joy_ 公开 API，不进入 IDL / 生成链。 */
        bool joy_r2d_render_geometry(joy_r2d_p r2d, void* texture,
                                 const float* vert_data, int vertex_count,
                                 const int* indices, int index_count);

        /* jr2d 内部按字体清理文字缓存, 供 jfont.c 销毁字体时调用;
         * 非 joy_ 公开 API, 不进入 IDL / 生成链。 */
        void joy_r2d_jtext_cache_clear_for_font(joy_font_p font);

#ifdef __cplusplus
}
#endif

#endif /* JOY_DEFS_H */
