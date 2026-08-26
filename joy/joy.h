/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: joy.h
Description:
Author: ydlc
Version: 1.0
Date: 2021.3.28
History:
*************************************************/
#ifndef JOY_H
#define JOY_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "jconfig.h"
#include "jmath.h"
#include "SDL3/SDL_events.h"


#ifdef __cplusplus
extern "C" {
#endif


/* 事件类型常量（无需 SDL 头文件即可判断事件类型） */
#define JOY_EVENT_QUIT              0x100
#define JOY_EVENT_KEY_DOWN          0x300
#define JOY_EVENT_KEY_UP            0x301
#define JOY_EVENT_TEXT_INPUT        0x303
#define JOY_EVENT_MOUSE_MOTION      0x400
#define JOY_EVENT_MOUSE_BUTTON_DOWN 0x401
#define JOY_EVENT_MOUSE_BUTTON_UP   0x402
#define JOY_EVENT_WINDOW_RESIZED    0x701

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

#define POPUP_MAX_TITLE 128
#define POPUP_MAX_CONTENT 2048
#define TOOLTIP_MAX_TEXT 256

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

#define ACCUMULATE_IMPULSES 1
#define WARM_STARTING 1
#define POSITION_CORRECTION 1
#define MAX_POINTS 2


#define jVERTOR_INIT(name, type) \
typedef struct jvector_##name { \
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



        typedef struct delay {
                uint64_t time;
                short timeout;
        } delay_t;

        typedef struct random_num random_num_t, * random_num_p;

        // Mersenne Twister 内部状态大小（624 个 uint32 + 1 个 index）
        #define JOY_MT_N 624

        // 可重置随机数（Mersenne Twister），用于帧同步保证 server/client 随机序列一致
        JOY_API random_num_p utils_random_create(uint32_t seed);
        JOY_API void         utils_random_destroy(random_num_p rnum);
        JOY_API uint32_t     utils_random_next(random_num_p rnum);
        // 导出/恢复随机数内部状态（帧同步：客户端 join 时需用服务端当前状态恢复，
        // 仅同步 seed 会让已推进的 random 与初始状态不同步，导致 AI 随机序列分叉）
        JOY_API void         utils_random_get_state(random_num_p rnum, uint32_t* mt_out, int* index_out);
        JOY_API void         utils_random_set_state(random_num_p rnum, const uint32_t* mt_in, int index_in);

        typedef struct lcg_num lcg_num_t, * lcg_num_p;

        // 单状态线性同余随机数（LCG，64 位状态）。状态仅 1 个 uint64，
        // 帧同步时"每帧快照"只需同步 8 字节，比 Mersenne Twister（mt[624]+index≈2.5KB）
        // 小得多。用法与 utils_random_* 一致。
        JOY_API lcg_num_p   lcg_random_create(uint64_t seed);
        JOY_API void        lcg_random_destroy(lcg_num_p lcg);
        JOY_API uint32_t    lcg_random_next(lcg_num_p lcg);
        // 导出/恢复 LCG 当前状态（单值）。帧同步：客户端 join 时用服务端当前状态恢复，
        // 保证两端从同一推进位置继续，随机序列一致。
        JOY_API void        lcg_random_get_state(lcg_num_p lcg, uint64_t* state_out);
        JOY_API void        lcg_random_set_state(lcg_num_p lcg, uint64_t state);

        typedef void (*jtimer_callback)(int actual_interval, int interval, void* userdata);
        typedef struct jtimer jtimer_t, *jtimer_p;

        typedef union jwindow jwindow_t, * jwindow_p;

        typedef void (*jevent_keypressed_fn)(const char* key, int scancode, bool repeat, bool ctrl, bool shift, bool alt, void* arg);
        typedef void (*jevent_keyreleased_fn)(const char* key, int scancode, void* arg);
        typedef void (*jevent_mousepressed_fn)(float x, float y, int button, void* arg);
        typedef void (*jevent_mousereleased_fn)(float x, float y, int button, void* arg);
        typedef void (*jevent_mousemoved_fn)(float x, float y, float dx, float dy, void* arg);
        typedef void (*jevent_textinput_fn)(const char* text, void* arg);
        typedef void (*jevent_resize_fn)(int w, int h, void* arg);
        typedef void (*jevent_process_event_fn)(const void* event, void* arg);

        typedef struct jevent_callbacks {
                jevent_keypressed_fn on_keypressed;
                jevent_keyreleased_fn on_keyreleased;
                jevent_mousepressed_fn on_mousepressed;
                jevent_mousereleased_fn on_mousereleased;
                jevent_mousemoved_fn on_mousemoved;
                jevent_textinput_fn on_textinput;
                jevent_resize_fn on_resize;
                jevent_process_event_fn on_process_event;
        } jevent_callbacks_t;

        typedef struct env env_t, * env_p;
        typedef struct simple_fps_counter simple_fps_counter_t, * simple_fps_counter_p;
        typedef struct game_timer {
                int64_t last_time;          // 上一帧结束时的纳秒时间戳
                int64_t frame_start_time;   // 当前帧开始时的纳秒时间戳
                double delta_time;         // 未缩放的帧间隔时间（秒）
                double time_scale;         // 时间缩放因子
                int target_fps;            // 目标 FPS（0 表示不限制）
                double target_frame_time;  // 目标每帧时间（秒）
        } game_timer_t, * game_timer_p;

        typedef struct jrect
        {
                float x;
                float y;
                float w;
                float h;
        } jrect_t;

        typedef struct jcolor
        {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                uint8_t a;
        } jcolor_t;


        typedef struct iocp_server iocp_server_t, * iocp_server_p;

        typedef struct jfont font_t, * font_p;
        typedef union jr2d jr2d_t, * jr2d_p;
        typedef struct image image_t, * image_p;
        typedef struct jtext jtext_t, * jtext_p;
        typedef struct animation animation_t, * animation_p;
        typedef struct jr3d   jr3d_t, * jr3d_p;          /* 3D 渲染上下文 */
        typedef struct render_mesh render_mesh_t, * render_mesh_p;  /* 网格 */
        /* ---------- MVP 结构（无 SDL 依赖） ---------- */
        typedef struct render_mvp {
                float viewProj[16];
                float model[16];
        } render_mvp_t;

        /* ---------- 光照结构（与 GLSL std140 布局对齐, 同 main888） ---------- */
        #define JR3D_MAX_LIGHTS 4
        #define JR3D_LIGHT_DIRECTIONAL 0
        #define JR3D_LIGHT_POINT 1
        typedef struct jr3d_shader_light {
                int    enabled;      /* 0 */
                int    type;         /* 4 */
                int    _pad0[2];     /* 8 */
                float  position[3];  /* 16 */
                float  _pad1;        /* 28 */
                float  target[3];    /* 32 */
                float  _pad2;        /* 44 */
                float  color[4];     /* 48 */
        } jr3d_shader_light_t;       /* 64 字节 */
        typedef struct jr3d_light_block {
                jr3d_shader_light_t lights[JR3D_MAX_LIGHTS];
                float ambient[4];
                float viewPos[4];
        } jr3d_light_block_t;        /* 288 字节 */


        typedef struct ray2df {
                vec2f_t origin, direction;
        }ray2df_t;

        typedef struct ray2d_collisionf {
                bool hit;               // Did the ray hit something?
                fp_t distance;         // Distance to the nearest hit
                vec2f_t point;          // Point of the nearest hit
                vec2f_t normal;         // Surface normal of hit
        }ray2d_collisionf_t;


        /* 锚点约定: 矩形用"左上角(x,y) + 宽高"定位, 区别于 GJK BOX 的"中心 + 半边长" */
        typedef struct rectanglef {
                fp_t x; // rectangle top-left corner position x
                fp_t y; // rectangle top-left corner position y
                fp_t width;
                fp_t height;
        }rectanglef_t;

        typedef struct circlef {
                vec2f_t center;
                fp_t radius;
        }circlef_t;

        typedef struct polygonf {
                vec2f_t* vertices;
                int num_vertices;
        }polygonf_t;


        typedef struct contact2df {
                vec2f_t sp; // starting point
                vec2f_t ep; // ending point
                vec2f_t normal;
                fp_t depth;
        }contact2df_t;


        typedef struct ray3df {
                vec3f_t position, direction;
        }ray3df_t;

        typedef struct ray3d_collisionf {
                bool hit;               // Did the ray hit something?
                fp_t distance;         // Distance to the nearest hit
                vec3f_t point;          // Point of the nearest hit
                vec3f_t normal;         // Surface normal of hit
        }ray3d_collisionf_t;

        typedef struct spheref {
                vec3f_t position;
                fp_t radius;
        }spheref_t;

        typedef struct bounding_boxf {
                vec3f_t min;            // Minimum vertex box-corner
                vec3f_t max;            // Maximum vertex box-corner
        } bounding_boxf_t;

        typedef struct ray2d {
                vec2_t origin, direction;
        }ray2d_t;

        typedef struct ray2d_collision {
                bool hit;
                ft_t distance;
                vec2_t point;
                vec2_t normal;
        }ray2d_collision_t;

        typedef struct rectangle {
                ft_t x, y, width, height;
        }rectangle_t;

        typedef struct circle {
                vec2_t center;
                ft_t radius;
        }circle_t;

        typedef struct polygon {
                vec2_t* vertices;
                int num_vertices;
        }polygon_t;

        typedef struct contact2d {
                vec2_t sp;
                vec2_t ep;
                vec2_t normal;
                ft_t depth;
        }contact2d_t;

        typedef struct ray3d {
                vec3_t position, direction;
        }ray3d_t;

        typedef struct ray3d_collision {
                bool hit;
                ft_t distance;
                vec3_t point;
                vec3_t normal;
        }ray3d_collision_t;

        typedef struct sphere {
                vec3_t position;
                ft_t radius;
        }sphere_t;

        typedef struct bounding_box {
                vec3_t min;
                vec3_t max;
        } bounding_box_t;




        /* ---------- 碰撞体类型 ---------- */
        typedef enum gjk3df_ctype {
                GJK3DF_CTYPE_SPHERE,   /* 球体: position + radius */
                GJK3DF_CTYPE_BOX,      /* AABB盒: position + half_size */
                GJK3DF_CTYPE_CAPSULE,  /* 胶囊: position(中心) + radius + height + axis */
                GJK3DF_CTYPE_CYLINDER, /* 圆柱: position + radius + height */
                GJK3DF_CTYPE_CONE,     /* 圆锥: position + radius + height */
                GJK3DF_CTYPE_MESH,     /* 自定义网格: vertices数组 */
                GJK3DF_CTYPE_OBB       /* 旋转盒: position + half_size + rotation(欧拉角) */
        } gjk3df_ctype_k;

        /* ---------- 3D碰撞体 ---------- */
        /* 注意: BOX/OBB 的 position 是中心(half_size 为半边长), 与 rectanglef_t 的"左上角+宽高"语义不同 */
        typedef struct gjk3df_collider {
                gjk3df_ctype_k type;
                vec3f_t position;      /* 中心位置 */
                fp_t radius;           /* 球体/胶囊/圆柱/圆锥半径 */
                vec3f_t half_size;     /* BOX/OBB半边长 */
                fp_t height;           /* 胶囊/圆柱/圆锥高度 */
                vec3f_t axis;          /* 胶囊朝向(单位向量) */
                vec3f_t rotation;      /* OBB欧拉角(绕 X/Y/Z 弧度) */
                vec3f_t* vertices;     /* MESH顶点数组(借用的指针,不负责释放) */
                int vertices_num;
        } gjk3df_collider_t, * gjk3df_collider_p;

        /* ---------- 接触信息(含EPA穿透深度) ---------- */
        typedef struct gjk3df_contact {
                vec3f_t normal;
                fp_t depth;
        } gjk3df_contact_t, * gjk3df_contact_p;

        /* ======================================================================== */
        /*                        GJK 3D 浮点版 (ft_t/vec3_t)                        */
        /* ======================================================================== */
        /* ---------- 碰撞体类型 ---------- */
        typedef enum gjk3d_ctype {
                GJK3D_CTYPE_SPHERE,   /* 球体: position + radius */
                GJK3D_CTYPE_BOX,      /* AABB盒: position + half_size */
                GJK3D_CTYPE_CAPSULE,  /* 胶囊: position(中心) + radius + height + axis */
                GJK3D_CTYPE_CYLINDER, /* 圆柱: position + radius + height */
                GJK3D_CTYPE_CONE,     /* 圆锥: position + radius + height */
                GJK3D_CTYPE_MESH      /* 自定义网格: vertices数组 */
        } gjk3d_ctype_k;

        /* ---------- 3D碰撞体(浮点) ---------- */
        typedef struct gjk3d_collider {
                gjk3d_ctype_k type;
                vec3_t position;       /* 中心位置 */
                ft_t radius;           /* 球体/胶囊/圆柱/圆锥半径 */
                vec3_t half_size;      /* BOX半边长 */
                ft_t height;           /* 胶囊/圆柱/圆锥高度 */
                vec3_t axis;           /* 胶囊朝向(单位向量) */
                vec3_t* vertices;      /* MESH顶点数组(借用的指针,不负责释放) */
                int vertices_num;
        } gjk3d_collider_t, * gjk3d_collider_p;

        /* ---------- 接触信息(含EPA穿透深度) ---------- */
        typedef struct gjk3d_contact {
                vec3_t normal;
                ft_t depth;
        } gjk3d_contact_t, * gjk3d_contact_p;

      

        /* ======================================================================== */
        /*                             GJK 2D 碰撞检测                                */
        /* ======================================================================== */

        typedef uint8_t gjk2d_ctype_k;
        #define GJK2D_CTYPE_CIRCLE   0  /* 圆: position + radius */
        #define GJK2D_CTYPE_BOX      1  /* AABB盒: position + half_size */
        #define GJK2D_CTYPE_CAPSULE  2  /* 胶囊: position(中心) + radius + height + axis */
        #define GJK2D_CTYPE_POLYGON  3  /* 凸多边形: vertices数组(预变换到世界坐标) */
        #define GJK2D_CTYPE_PIE      4  /* 扇形: position + radius + sweep(弧度) */

        /* 注意: BOX 的 position 是中心(half_size 为半边长), 与 rectanglef_t 的"左上角+宽高"语义不同 */
        typedef struct gjk2d_collider {
                gjk2d_ctype_k type;
                vec2f_t position;       /* 中心位置 */
                fp_t radius;            /* 圆/胶囊/扇形半径 */
                vec2f_t half_size;      /* BOX半边长 */
                fp_t height;            /* 胶囊线段长度 */
                vec2f_t axis;           /* 胶囊朝向(单位向量) */
                vec2f_t* vertices;      /* POLYGON顶点数组(借用的指针,不释放) */
                int vertices_num;
                fp_t sweep;             /* 扇形张角(弧度) */
        } gjk2d_collider_t, * gjk2d_collider_p;

        typedef struct gjk2d_contact {
                vec2f_t normal;
                fp_t depth;
        } gjk2d_contact_t, * gjk2d_contact_p;




        /* ========== Types ========== */
        typedef struct janimator janimator_t, * janimator_p;

        /* Parameter types */
        typedef enum {
                JANIM_PARAM_FLOAT,
                JANIM_PARAM_INT,
                JANIM_PARAM_BOOL,
                JANIM_PARAM_TRIGGER,
        } janim_param_type_t;

        /* Condition modes */
        typedef enum {
                JANIM_COND_GREATER,     /* param > threshold */
                JANIM_COND_LESS,        /* param < threshold */
                JANIM_COND_EQUALS,      /* param == threshold (float with epsilon) */
                JANIM_COND_NOT_EQUALS,  /* param != threshold */
                JANIM_COND_IF_TRUE,     /* bool param == true */
                JANIM_COND_IF_FALSE,    /* bool param == false */
                JANIM_COND_TRIGGER,     /* trigger was set */
        } janim_cond_mode_t;

        /* State enter/exit callback */
        typedef void (*janim_state_cb)(const char* state_name, void* userdata);

        typedef struct label label_t, * label_p;
        typedef struct button button_t, * button_p;
        typedef struct combobox_item combobox_item_t, * combobox_item_p;
        typedef struct combobox combobox_t, * combobox_p;
        typedef struct combobox_input combobox_input_t, * combobox_input_p;
        typedef struct datagrid datagrid_t, * datagrid_p;
        typedef struct radio radio_t, * radio_p;
        typedef enum checkbox_state {
                JOY_CHECKBOX_UNCHECKED,
                JOY_CHECKBOX_CHECKED,
                JOY_CHECKBOX_INDETERMINATE
        }checkbox_state_t;
        typedef struct checkbox checkbox_t, * checkbox_p;
        typedef struct joystick joystick_t, * joystick_p;
        typedef struct inputbox inputbox_t, * inputbox_p;
        typedef struct slider slider_t, * slider_p;
        typedef struct popup popup_t, * popup_p;
        typedef struct tooltip tooltip_t, * tooltip_p;


        /* ---------- 对象（碰撞体/实体） ---------- */
        typedef struct jtilemap_object {
            int    id;
            char* name;
            char* type;         /* 用户自定义类型 */
            int    shape;        /* JTILEMAP_SHAPE_* */
            float  x, y;
            float  width, height; /* 矩形有效 */
            float  rotation;
            bool   visible;
        }jtilemap_object_t, * jtilemap_object_p;
        typedef struct jtilemap_gid_resolve jtilemap_gid_resolve_t;
        typedef struct jtilemap jtilemap_t, * jtilemap_p;
        typedef struct jtilemap_tileset jtilemap_tileset_t, * jtilemap_tileset_p;
        typedef struct jtilemap_tile_layer jtilemap_tile_layer_t, * jtilemap_tile_layer_p;
        typedef struct jtilemap_object_layer jtilemap_object_layer_t, * jtilemap_object_layer_p;

        /* ---------- 路径映射回调 ---------- */
        typedef const char* (*jtilemap_path_remap_t)(const char* original_path);
        /* ---------- 渲染实例（不透明句柄，可重入） ---------- */
        typedef struct jtilemap_render_t jtilemap_render_t, * jtilemap_render_p;


        typedef enum {
                NET_TYPE_CONNECTED = 0x1,
                NET_TYPE_DISCONNECTED = 0x2,
                NET_TYPE_MESSAGE = 0x3,
        } net_event_type_k;

        typedef struct net_message {
                net_event_type_k type;
                int conv;
                char* data;
                int len;
        }net_message_t, * net_message_p;

        typedef void (*net_callback)(net_message_p msg, void* userdata);
        typedef struct kcpserver kcpserver_t, * kcpserver_p;
        typedef struct kcpclient kcpclient_t, * kcpclient_p;
        typedef struct tcpserver tcpserver_t, * tcpserver_p;
        typedef struct tcpclient tcpclient_t, * tcpclient_p;
        typedef struct wsnetserver wsnetserver_t, * wsnetserver_p;
        typedef struct wsnetclient wsnetclient_t, * wsnetclient_p;
        typedef enum {
                NET_CLIENT_TCP,
                NET_CLIENT_KCP,
                NET_CLIENT_WEBSOCKET, // WebSocket: emscripten用原生, 非emscripten用mongoose
        } net_client_type;
        typedef struct netclient netclient_t, * netclient_p;
        typedef enum {
                NET_SERVER_KCP,
                NET_SERVER_TCP,
                NET_SERVER_WEBSOCKET,
        } net_server_type;
        typedef struct netserver netserver_t, * netserver_p;


        typedef struct scene_node scene_node_t, * scene_node_p;
        typedef struct scene scene_t, * scene_p;
        typedef struct scene_manager scene_manager_t, * scene_manager_p;

        typedef void (*scene_node_update_fn)(scene_node_p, float);
        typedef void (*scene_node_render_fn)(scene_node_p, const void*);
        typedef void (*scene_node_destroy_fn)(scene_node_p);
        typedef void (*scene_node_event_fn)(scene_node_p, const void*);

        typedef void (*scene_load_fn)(scene_p);
        typedef void (*scene_update_fn)(scene_p, float);
        typedef void (*scene_render_fn)(scene_p);
        typedef void (*scene_destroy_fn)(scene_p);
        typedef void (*scene_event_fn)(scene_p, const void*);



        /* ---------- 粒子发射器配置 ---------- */
        typedef struct jparticle_cfg {
                float x, y;                /* 发射位置 */
                float spawn_radius;        /* 生成半径(粒子在此范围内随机生成) */
                float direction;           /* 基础方向(度) */
                float spread;              /* 扩散角(度, 0=无扩散, 360=全方向) */
                float speed_min, speed_max;
                float lifetime_min, lifetime_max;
                float size_start, size_end;/* 起始/结束大小(线性插值) */
                float r, g, b, a;         /* 颜色 */
                float fadeout;             /* 淡出比例 0~1(0.8=最后20%生命周期淡出) */
                int   burst;               /* 每次 emit 产生的粒子数 */
                float rate;                /* 持续发射: 每秒粒子数(<=0 只爆发不持续) */
                bool  gravity;             /* 是否受重力影响 */
                float gravity_y;           /* 重力值(正数向下, 如 50.0f) */
        } jparticle_cfg_t;

        typedef struct jparticle jparticle_t, * jparticle_p;

        /* ---------- 光源 ---------- */
        typedef struct jlight {
                float x, y;          /* 位置(像素坐标) */
                float radius;        /* 光照半径 */
                float r, g, b;       /* 颜色(0-255) */
                float intensity;     /* 强度 0~1 */
        } jlight_t;

        typedef struct jlight_sys jlight_sys_t, * jlight_sys_p;


        typedef int jecs_id_t;
        typedef struct jecs_world jecs_world_t, * jecs_world_p;
        //VECTOR_INIT(kecs, jecs_id_t);

        typedef struct jecs_id_array {
                jecs_id_t ids[512];
                int num;
        }jecs_id_array_t, * jecs_id_array_p;

        typedef struct jecs_system {
                void* arg;
                void (*cb)(jecs_world_p, void*, float);
        } jecs_system_t, * jecs_system_p;


        /* ===== 行为树状态 ===== */
        typedef enum {
                BT_SUCCESS,
                BT_FAILURE,
                BT_RUNNING,
        } bt_status_t;

        /* ===== 前向声明 ===== */
        typedef struct bt_node bt_node_t;

        /* ===== 用户回调函数类型 ===== */
        typedef bt_status_t(*bt_action_fn)(float dt);
        typedef bool        (*bt_condition_fn)(void);

        /* ===== 节点生命周期回调 ===== */
        typedef void     (*bt_enter_fn)(bt_node_t* self);
        typedef bt_status_t(*bt_tick_fn)(bt_node_t* self, float dt);
        typedef void     (*bt_exit_fn)(bt_node_t* self);
        typedef void     (*bt_destroy_fn)(bt_node_t* self);

        /* ===== 基础节点结构 ===== */
        struct bt_node {
                bt_status_t   status;      /* 当前运行状态 */
                bt_enter_fn   enter;       /* 进入回调 */
                bt_tick_fn    tick;        /* 更新回调 */
                bt_exit_fn    exit;        /* 退出回调 */
                bt_destroy_fn destroy;     /* 销毁回调 */
        };

        /* ===== 内嵌状态机节点 ===== */
        typedef struct bt_state_slot {
                const char* name;                    /* 状态名称（不可为 NULL） */
                bt_enter_fn  state_enter;             /* 状态 enter（可为 NULL） */
                const char* (*state_update)(bt_node_t* node, float dt); /* 返回下一状态名，NULL 表示不切换 */
                bt_exit_fn   state_exit;              /* 状态 exit（可为 NULL） */
        } bt_state_slot_t;



        typedef struct audio_shot audio_shot_t, * audio_shot_p;
        typedef struct audio_bgm audio_bgm_t, * audio_bgm_p;


        typedef void* (*module_create_t)(void);
        typedef int (*module_init_t)(void* inst, void* app, const char* param);
        typedef int (*module_event_t)(void* inst, void* event);
        typedef int (*module_update_t)(void* inst, float delta_time);
        typedef void (*module_release_t)(void* inst);

        typedef struct module {
                char name[512];
                void* handle;
                module_create_t create;
                module_init_t init;
                module_event_t event_handler;
                module_update_t update;
                module_release_t release;
        }module_t, * module_p;

        typedef struct modules modules_t, * modules_p;




        typedef struct box2d_body_t {
                int id;
                vec2f_t position;
                fp_t rotation;
                vec2f_t velocity;
                fp_t angular_velocity;
                vec2f_t force;
                fp_t torque;
                vec2f_t bounds;
                fp_t friction;
                fp_t mass, inv_mass;
                fp_t I, invI;
        } box2d_body_t;


        typedef struct box2d_joint_t {
                mat22f_t M;
                vec2f_t local_anchor1, local_anchor2;
                vec2f_t r1, r2;
                vec2f_t bias;
                vec2f_t p;		// accumulated impulse
                int id1, id2;
                fp_t biasfactor;
                fp_t softness;
        } box2d_joint_t;


        typedef struct box2d_manifold_t {
                vec2f_t position;
                vec2f_t normal;
                fp_t separation;
                int feature;
                fp_t pn;	// accumulated normal impulse
                fp_t pt;	// accumulated tangent impulse
                vec2f_t r1, r2;
                fp_t mass_normal, mass_tangent;
                fp_t bias;
        } box2d_manifold_t;


        typedef union box2d_arbiter_key_t {
                struct id_pair {
                        int id1, id2;
                } id_pair;
                int64_t value;
        } box2d_arbiter_key_t;


        typedef struct box2d_arbiter_t {
                box2d_manifold_t manifolds[MAX_POINTS];
                int num_manifolds, id1, id2;
                fp_t friction; // combined friction
        } box2d_arbiter_t;


        typedef struct box2d_world_t box2d_world_t;



        /* ===== 装箱回调类型 ===== */
        typedef int (*jmsgpack_write_fn)(void* data, const char* buf, size_t len);

        /* ===== 类型别名 ===== */
        typedef struct jmsgpack_packer {
                void* data;
                jmsgpack_write_fn callback;
        } jmsgpack_packer_t;

        typedef struct jmsgpack_zone jmsgpack_zone_t;
        typedef struct jmsgpack_object jmsgpack_object_t;

        typedef enum {
                JMSGPACK_OBJECT_NIL = 0x00,
                JMSGPACK_OBJECT_BOOLEAN = 0x01,
                JMSGPACK_OBJECT_POSITIVE_INTEGER = 0x02,
                JMSGPACK_OBJECT_NEGATIVE_INTEGER = 0x03,
                JMSGPACK_OBJECT_FLOAT64 = 0x04,
                JMSGPACK_OBJECT_STR = 0x05,
                JMSGPACK_OBJECT_ARRAY = 0x06,
                JMSGPACK_OBJECT_MAP = 0x07,
        } jmsgpack_object_type_t;

        typedef struct {
                uint32_t size;
                const jmsgpack_object_t* ptr;
        } jmsgpack_object_array_t;

        typedef struct {
                uint32_t size;
                const char* ptr;
        } jmsgpack_object_str_t;

        typedef struct jmsgpack_object {
                jmsgpack_object_type_t type;
                union {
                        bool boolean;
                        uint64_t u64;
                        int64_t i64;
                        double f64;
                        jmsgpack_object_array_t array;
                        jmsgpack_object_str_t str;
                } via;
        } jmsgpack_object_t;

        typedef struct {
                jmsgpack_zone_t* zone;
                jmsgpack_object_t data;
        } jmsgpack_unpacked_t;


        typedef struct luax luax_t, * luax_p;
        typedef struct jsx jsx_t, * jsx_p;
        typedef struct csx csx_t, * csx_p;


        ///////////////////////////////////////////////////////////////////////
        ////////////////////////////  utils  //////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        JOY_API char* utils_strdup(const char* str, size_t* out_sz);
        JOY_API bool utils_get_current_time(uint64_t* ms_ticks);
        JOY_API void utils_current_datetime(const char* fmt, char* buf, size_t len);
        JOY_API char* utils_read_file(const char* filename, size_t* sz);
        JOY_API bool utils_check_file(const char* filename);
        JOY_API void utils_append_file(const char* filename, const char* data);
        JOY_API char* utils_read_script(const char* file, size_t* data_size);
        JOY_API short utils_bit2short(uint8_t* buf);
        JOY_API void  utils_short2bit(uint8_t* buf, short value);
        /* c# = BitConverter.FromUInt32 */
        JOY_API int utils_bit2int(uint8_t* buf);
        /* c# = BitConverter.ToUInt32 */
        JOY_API void utils_int2bit(uint8_t* buf, int value);
        JOY_API bool utils_wait_delay(delay_t* delay, uint64_t current_time);
        JOY_API short utils_md5x(const char* input, size_t len, char* output);
        JOY_API char* utils_des_crypt(const char* key, const char* text, size_t textsz, size_t* out_len);
        JOY_API char* utils_des_decrypt(const char* key, const char* text, size_t textsz, size_t* out_len);

        JOY_API unsigned long sdbm_hash(const char* str, int size);
        JOY_API unsigned long fnv1a_hash(const char* str, int size);
        JOY_API unsigned long jenkins_hash(const char* str, int size);
        JOY_API unsigned long murmur3_hash(const char* str);
        JOY_API unsigned long kr_hash(const char* str, int size);

        /**
         * 解码 UTF-8 字符串的第一个字符
         * @param s    输入字符串
         * @param len  字符串长度（字节数）
         * @param code 输出参数，存储解码后的 Unicode 码点 (int32_t)
         * @return     成功时返回消耗的字节数（1~4），失败返回 0
         */
        JOY_API size_t utf8_decode(const char* s, size_t len, int32_t* code);

        /**
         * 迭代遍历 UTF-8 字符串
         * @param s    输入字符串
         * @param len  字符串长度
         * @param pos  当前位置指针（输入/输出）
         * @param code 输出码点 (int32_t)
         * @return     成功返回 1，结束或错误返回 0
         */
        JOY_API int utf8_next(const char* s, size_t len,
                size_t* pos, int32_t* code);

        /**
         * 将整个 UTF-8 字符串解码为码点数组（调用者提供缓冲区）
         * @param s            输入字符串
         * @param len          字符串长度（字节数）
         * @param out_codes    输出缓冲区（接收码点数组），可为 NULL 以查询所需长度
         * @param out_capacity 输出缓冲区的容量（最多能存放的码点个数）
         * @return             成功时返回实际写入的码点个数（若 out_codes==NULL，则返回所需个数）；
         *                     若字符串无效或缓冲区容量不足，返回 0
         */
        JOY_API size_t utf8_decode_all(const char* s, size_t len,
                int32_t* out_codes, size_t out_capacity);
        JOY_API int pack_int8(char* buf, int8_t value, int offset);
        JOY_API int pack_int16(char* buf, int16_t value, int offset);
        JOY_API int pack_int32(char* buf, int32_t value, int offset);
        JOY_API int pack_uint32(char* buf, uint32_t value, int offset);
        JOY_API int pack_int64(char* buf, int64_t value, int offset);
        JOY_API int unpack_int8(const char* buf, int8_t* value, int offset);
        JOY_API int unpack_int16(const char* buf, int16_t* value, int offset);
        JOY_API int unpack_int32(const char* buf, int32_t* value, int offset);
        JOY_API int unpack_uint32(const char* buf, uint32_t* value, int offset);
        JOY_API int unpack_int64(const char* buf, int64_t* value, int offset);
        JOY_API int pack_string(char* buf, const char* value, int type_size, int offset);
        JOY_API int unpack_string(const char* buf, char* value, int type_size, int offset);
       
        JOY_API jtimer_p jtimer_create(int interval, jtimer_callback cb, void* userdata);
        JOY_API void jtimer_destroy(jtimer_p timer);
        JOY_API void jtimer_trigger(jtimer_p timer);
        JOY_API void jtimer_set_interval(jtimer_p timer, int interval);
        JOY_API void jtimer_set_active(jtimer_p timer, bool active);
        JOY_API void jtimer_reset(jtimer_p timer);
        JOY_API uint64_t jtimer_get_elapsed(jtimer_p timer);

        ///////////////////////////////////////////////////////////////////////
        ////////////////////////////  core  ///////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        JOY_API bool init_env();
        JOY_API void quit_env(void);
        JOY_API const char* get_error(void);
        JOY_API uint64_t get_ticks(void);

        /** 从鼠标事件中提取坐标 */
        JOY_API void get_mouse_pos(const void* event, float* out_x, float* out_y);
        /** 从鼠标事件中提取按钮（1=左, 2=中, 3=右） */
        JOY_API int  get_mouse_button(const void* event);

        JOY_API jwindow_p create_window(const char* title, int w, int h);
        JOY_API void destroy_window(jwindow_p win);
        JOY_API void* get_raw_window(jwindow_p win);
        JOY_API void window_set_fullscreen(jwindow_p win, bool fullscreen);
        JOY_API bool window_get_fullscreen(jwindow_p win);
        JOY_API void window_set_icon(jwindow_p win, const char* png_path);

        JOY_API void dispatch_event(jevent_callbacks_t cbs,
                const void* event, void* arg);
        JOY_API int  get_event_type(const void* event);
        JOY_API int  get_event_code(const void* event);
        JOY_API bool is_user_event(const void* event, int type);
        JOY_API bool push_user_event(int type, int code);
        JOY_API bool push_quit_event(void);
        JOY_API bool should_close(void* ev);

        JOY_API int log_info(const char* format, ...);
        JOY_API int log_debug(const char* format, ...);
        JOY_API int log_error(const char* format, ...);
        JOY_API void log_title(int type);

        JOY_API env_p env_create(void);
        JOY_API const char* env_get(env_p env, const char* key);
        JOY_API void env_set(env_p env, const char* key, const char* value);
        JOY_API void env_destroy(env_p env);

        JOY_API simple_fps_counter_p simple_fps_create();
        JOY_API void simple_fps_destory(simple_fps_counter_p fps);
        JOY_API void simple_fps_update(simple_fps_counter_p fps);
        JOY_API int simple_fps_value(simple_fps_counter_p fps);

        JOY_API void game_timer_init(game_timer_p clock);
        JOY_API void game_timer_update(game_timer_p clock);
        JOY_API float game_timer_get_delta_time(const game_timer_p clock);
        JOY_API float game_timer_get_unscaled_delta_time(const game_timer_p clock);
        JOY_API void game_timer_set_time_scale(game_timer_p clock, float scale);
        JOY_API float game_timer_get_time_scale(const game_timer_p clock);
        JOY_API void game_timer_set_target_fps(game_timer_p clock, int fps);
        JOY_API int game_timer_get_target_fps(const game_timer_p clock);

        /** 获取剪贴板文本（调用者负责释放返回的字符串） */
        JOY_API char* get_clipboard_text(void);
        /** 设置剪贴板文本 */
        JOY_API void set_clipboard_text(const char* text);


        JOY_API bool sys_init_netenv(void);
        JOY_API bool sys_release_netenv(void);
        JOY_API int get_local_ips(char ips[][JOY_MAX_IP], int max_count);
        JOY_API int sys_anyaddr(void);
        JOY_API bool sys_closesocket(int64_t sockfd);
        JOY_API int sys_set_sock_sndtimeo(int64_t sockfd, int ms);
        JOY_API int sys_set_sock_rcvtimeo(int64_t sockfd, int ms);
        JOY_API int sys_set_sock_accpettimeo(int64_t sockfd, int ms);
        JOY_API int64_t sys_tcp();
        JOY_API int64_t sys_udp();
        JOY_API bool sys_bind(int64_t sockfd, const char* ip, int port);
        JOY_API bool sys_connect(int64_t sockfd, const char* ip, int port);
        JOY_API bool sys_listen(int64_t sockfd);
        JOY_API int64_t sys_accept(int64_t sockfd, char* ip, int* port);
        JOY_API int sys_send(int64_t sockfd, const char* buf, int len);
        JOY_API int sys_recv(int64_t sockfd, char* buf);
        JOY_API int sys_recvfrom(int64_t sockfd, char* buf, char* ip, int* port);
        JOY_API int sys_sendto(int64_t sockfd, const char* buf,
                int len, const char* ip, int port);
        JOY_API int64_t sys_current_time();
        JOY_API void sys_sleep(int ms);
        JOY_API void sys_install_crash_handlers();

        JOY_API iocp_server_p iocp_server_create(const char* bind_ip, int bind_port);
        JOY_API void iocp_server_destroy(iocp_server_p server);
        JOY_API void iocp_process_events(iocp_server_p server, int timeout_ms);


        ///////////////////////////////////////////////////////////////////////
        ////////////////////////////  font ///////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        JOY_API font_p font_create(const char* filename, int fontsize);
        JOY_API void font_destroy(font_p font);
        JOY_API float font_get_scale(font_p font);
        JOY_API int font_get_size(font_p font);
        JOY_API int font_get_ascent(font_p font);


        ///////////////////////////////////////////////////////////////////////
        ////////////////////////////  j2rd ///////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        JOY_API jr2d_p r2d_create(jwindow_p win);
        JOY_API jr2d_p r2d_wrap(void* sdl_renderer);
        JOY_API void r2d_destroy(jr2d_p r2d);
        JOY_API void* r2d_unwrap(jr2d_p r2d);
        JOY_API vec2_t r2d_coordinates_from_window(jr2d_p r2d, float x, float y);

        /* 颜色/状态 */
        JOY_API bool r2d_get_color(jr2d_p r2d, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
        JOY_API bool r2d_set_color(jr2d_p r2d, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        JOY_API void r2d_begin(jr2d_p r2d);
        JOY_API void r2d_end(jr2d_p r2d);
        JOY_API bool r2d_set_logical_presentation(jr2d_p r2d, int w, int h);
        JOY_API bool r2d_get_logical_presentation(jr2d_p r2d, int* w, int* h, int* mode);
        JOY_API void r2d_get_size(jr2d_p r2d, int* w, int* h);
        JOY_API bool r2d_set_vsync(jr2d_p r2d, int vsync);
        JOY_API bool r2d_set_scale(jr2d_p r2d, float sx, float sy);
        JOY_API void r2d_set_colorf(jr2d_p r2d, float r, float g, float b, float a);
        JOY_API void r2d_get_colorf(jr2d_p r2d, float* r, float* g, float* b, float* a);
        JOY_API bool r2d_set_blend_mode(jr2d_p r2d, int mode);
        JOY_API bool r2d_draw_point(jr2d_p r2d, float x, float y);
        JOY_API bool r2d_draw_points(jr2d_p r2d, const vec2_t* points, int count);

        /* 形状绘制 */
        JOY_API bool shape_draw_line(jr2d_p r2d, float x1, float y1, float x2, float y2);
        JOY_API bool r2d_draw_lines(jr2d_p r2d, const vec2_t* points, int count);
        JOY_API bool shape_draw_rectangle(jr2d_p r2d, const char* mode, jrect_t rect);
        JOY_API bool shape_draw_polygon(jr2d_p r2d, const char* mode, vec2_t* points, int point_count, jcolor_t color);
        JOY_API bool shape_draw_circle(jr2d_p r2d, const char* mode, vec2_t center, float radius, int segments);
        JOY_API bool shape_draw_grid(jr2d_p r2d, vec2_t start, vec2_t end, float grid_size);
        JOY_API bool shape_draw_gridx(jr2d_p r2d, vec2_t position, int rows, int cols, float grid_size);

        /* 图片操作 */
        JOY_API image_p image_create(jr2d_p r2d, const char* filename);
        JOY_API image_p image_create_from_data(jr2d_p r2d, const void* rgba_pixels, int w, int h);
        JOY_API image_p image_create_empty(jr2d_p r2d, int w, int h);
        JOY_API void  image_destroy(image_p image);
        JOY_API void  image_draw(image_p image, vec2_t position);
        JOY_API void  image_draw1(image_p image, const jrect_t* srcrect, const jrect_t* destrect);
        JOY_API void  image_draw_ex(image_p image, const jrect_t* srcrect, vec2_t position, float rotation, vec2_t scale, vec2_t origin);
        JOY_API void  image_set_alpha(image_p image, uint8_t alpha);
        JOY_API void  image_set_blend_mode(image_p image, int mode);
        JOY_API bool  image_get_size(image_p image, float* w, float* h);
        JOY_API void  image_update(image_p image, const jrect_t* rect, const void* pixels, int pitch);

        /* 文字 */
        JOY_API jtext_p text_createx(jr2d_p r2d, font_p font, const char* str, int len, jcolor_t color);
        JOY_API jtext_p text_create(jr2d_p r2d, font_p font, const int* codepoints, int num_codepoints, jcolor_t color);
        JOY_API void text_updatex(jtext_p text, font_p font, const char* str, int len, jcolor_t color);
        JOY_API void text_update(jtext_p text, font_p font, const int* codepoints, int num_codepoints, jcolor_t color);
        JOY_API void text_destroy(jtext_p jtext_tex);
        JOY_API void jtext_print(jr2d_p r2d, jtext_p jtext_tex, float x, float y);
        JOY_API void jtext_printx(jr2d_p r2d, font_p font, const char* str, int len, jcolor_t color, float x, float y);
        JOY_API int text_get_width(jtext_p jtext_tex);
        JOY_API int text_get_height(jtext_p jtext_tex);

        /* 简单动画 */
        JOY_API animation_p animation_create(jr2d_p r2d);
        JOY_API void animation_destroy(animation_p animation);
        JOY_API void animation_reset(animation_p animation);
        JOY_API void animation_add_clip(animation_p animation, const char* image_path, float duration, jrect_t src_rect);
        JOY_API bool animation_is_finished(animation_p animation);
        JOY_API void animation_set_position(animation_p animation, float x, float y);
        JOY_API void animation_set_scale(animation_p animation, float x, float y);
        JOY_API void animation_set_rotation(animation_p animation, float rotation);
        JOY_API void animation_update(animation_p animation, float dt);
        JOY_API void animation_draw(animation_p animation, jrect_t* camera);


        ///////////////////////////////////////////////////////////////////////
        ////////////////////////////  j3rd ///////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        JOY_API void* r3d_load_shader(void* device, const char* filename,
                int stage, int sampler_num, int uniform_buffer_num);
        JOY_API void* r3d_create_pipeline(void* device,
                void* vertex_shader, void* fragment_shader,
                int swapchain_format, int depth_format);
        JOY_API void* r3d_upload_buffer(void* device, const void* data, size_t size);
        JOY_API void* r3d_create_solid_texture(void* device,
                unsigned char r, unsigned char g, unsigned char b);
        JOY_API void* r3d_create_depth_texture(void* device, int width, int height);
        JOY_API void* r3d_create_sampler(void* device);
        JOY_API void  r3d_mat44_to_float16(const mat44_t* m, float out[16]);

        /* ================================================================
         * 高级 API (v3.0) — 可重入，无 SDL3 类型暴露
         * ================================================================ */

         /* ---------- 生命周期 ---------- */
        JOY_API jr3d_p r3d_create(jwindow_p win, const char* shader_dir);
        JOY_API void   r3d_destroy(jr3d_p r);
        /* 返回内部 SDL_GPUDevice*, 供调用方用 SDL3 API 自传纹理(如字体图集) */
        JOY_API void*  r3d_get_device(jr3d_p r);

        /* ---------- 帧管理 ---------- */
        JOY_API bool r3d_begin(jr3d_p r);
        JOY_API void r3d_end(jr3d_p r);

        /* ---------- 相机 ---------- */
        JOY_API void r3d_camera_lookat(jr3d_p r, vec3_t eye, vec3_t target, vec3_t up, float fovy);
        JOY_API void r3d_camera_fps(jr3d_p r, vec3_t pos, float yaw, float pitch, float fovy);

        /* ---------- 网格 ---------- */
        JOY_API render_mesh_p r3d_mesh_cube(jr3d_p r, float w, float h, float d);
        JOY_API render_mesh_p r3d_mesh_sphere(jr3d_p r, float radius, int slices, int stacks);
        JOY_API render_mesh_p r3d_mesh_plane(jr3d_p r, float w, float d, int segs);
        JOY_API render_mesh_p r3d_mesh_ramp(jr3d_p r, float w, float l, float h);
        JOY_API render_mesh_p r3d_mesh_custom(jr3d_p r, const void* verts, int vertex_count);
        /* 单位平面 quad: 4 顶点(FLOAT3 pos + FLOAT2 uv) + 6 索引(0,1,2,0,3,2), 范围 x/y ∈ [-0.5,0.5] */
        JOY_API render_mesh_p r3d_mesh_quad(jr3d_p r);
        JOY_API void         r3d_mesh_destroy(jr3d_p r, render_mesh_p m);

        /* ---------- 纹理 ---------- */
        /* 通用 RGBA32 纹理创建(自动处理 upload 行对齐, flip=true 时上下翻转, 与 stbi_set_flip_vertically_on_load 对应) */
        JOY_API void* r3d_create_texture(jr3d_p r, int w, int h, const void* rgba32, int flip);
        /* 从 PNG 文件加载并创建 GPU 纹理(SDL_LoadPNG -> RGBA32 -> r3d_create_texture, flip=1)
           成功返回 SDL_GPUTexture*(void*), 失败返回 NULL 并通过 SDL_Log 暴露原因 */
        JOY_API void* r3d_load_texture_png(jr3d_p r, const char* path, int* outW, int* outH);
        JOY_API void* r3d_gen_checkerboard(jr3d_p r, int tile_size, int tile_count);

        /* ---------- 字体渲染 (stb_truetype) ---------- */
        /* 初始化字体(读取 TTF, 由引擎持有缓冲, 程序退出前不释放) */
        JOY_API bool r3d_font_init(jr3d_p r, const char* ttf_path, int pixel_height);
        /* 在 3D 空间以 billboard 绘制文字: topLeft 为左上角锚点(世界坐标),
           yaw/pitch 为屏幕基向量方向(与相机一致), size 为文字世界高度。
           right/up 由 yaw/pitch 推得, 保证文字直立不斜切。 */
        JOY_API bool r3d_font_draw_3d(jr3d_p r, render_mesh_p m, const char* text,
                                       vec3_t topLeft, float yaw, float pitch, float size);
        /* 固定朝向绘制文字(不随相机变化): 始终水平直立、正对 -Z。
           topLeft 为左上角锚点(世界坐标), size 为文字世界高度。 */
        JOY_API bool r3d_font_draw_fixed(jr3d_p r, render_mesh_p m, const char* text,
                                         vec3_t topLeft, float size);
        /* 屏幕 2D HUD 文字: 固定在窗口像素坐标(pixelX,pixelY 左上, Y向下), 不随相机变化。
           fontSize 为像素高度, winW/winH 为窗口像素尺寸。 */
        JOY_API bool r3d_font_draw_2d(jr3d_p r, render_mesh_p m, const char* text,
                                      float pixelX, float pixelY, float fontSize,
                                      float winW, float winH);
        /* 取当前渲染尺寸(像素, 与视口一致), 用于 2D HUD 投影。可传 NULL 忽略某一项 */
        JOY_API void r3d_get_window_size(jr3d_p r, int* outW, int* outH);

        /* ---------- 2D UI: button (仿 jr2d, 用屏幕正交投影渲染, 不随相机变化) ---------- */
        /* 按钮状态 */
        typedef enum {
            JR3D_BTN_NORMAL  = 0,
            JR3D_BTN_HOVER   = 1,
            JR3D_BTN_PRESSED = 2
        } jr3d_button_state_t;

        /* 按钮对象(屏幕像素坐标, 原点左上) */
        typedef struct jr3d_button jr3d_button_t, * jr3d_button_p;
        struct jr3d_button {
            float x, y, w, h;          /* 位置与尺寸(像素) */
            char  text[128];           /* 按钮文字 */
            jr3d_button_state_t state; /* 当前状态 */
            bool  pressedInside;       /* 按下时是否在按钮内(用于判定 click) */
            void (*onClick)(jr3d_button_p);  /* 点击回调(按下+松开均在按钮内触发) */
        };

        /* 创建一个按钮(屏幕像素坐标, 原点左上) */
        JOY_API jr3d_button_p r3d_button_create(float x, float y, float w, float h, const char* text);
        /* 绘制按钮(需先 r3d_font_init; m 为单位 quad)。按钮会自己处理 normal/hover/pressed 配色 */
        JOY_API void r3d_button_draw(jr3d_p r, render_mesh_p m, jr3d_button_p btn, float winW, float winH);
        /* 命中测试: 屏幕像素坐标 (mx,my) 是否在按钮内 */
        JOY_API bool r3d_button_is_hovered(const jr3d_button_p btn, float mx, float my);
        /* 按下测试: 鼠标在按钮内且 mouse_down 为 true 时返回 true(并置为 pressed 状态) */
        JOY_API bool r3d_button_is_pressed(jr3d_button_p btn, bool mouse_down, float mx, float my);
        /* 设置点击回调(按下并松开均在按钮内时触发) */
        JOY_API void r3d_button_set_onclick(jr3d_button_p btn, void (*cb)(jr3d_button_p));
        /* 释放按钮: 更新状态为 hover/normal */
        JOY_API void r3d_button_release(jr3d_button_p btn, float mx, float my);
        /* 销毁按钮 */
        JOY_API void r3d_button_destroy(jr3d_button_p btn);

        /* ---------- 2D UI: label (仿 jr2d, 纯文字 2D HUD, 固定屏幕位置) ---------- */
        typedef struct jr3d_label jr3d_label_t, * jr3d_label_p;
        struct jr3d_label {
            float x, y;          /* 左上角位置(像素) */
            float fontSize;      /* 字号(像素) */
            uint8_t r, g, b;     /* 文字颜色 */
            char  text[256];     /* 文字内容 */
        };

        /* 创建 label(屏幕像素坐标, 原点左上) */
        JOY_API jr3d_label_p r3d_label_create(float x, float y, float fontSize, uint8_t cr, uint8_t cg, uint8_t cb, const char* text);
        /* 更新文字内容 */
        JOY_API void r3d_label_set_text(jr3d_label_p l, const char* text);
        /* 绘制 label */
        JOY_API void r3d_label_draw(jr3d_p r, render_mesh_p m, jr3d_label_p l, float winW, float winH);
        /* 销毁 label */
        JOY_API void r3d_label_destroy(jr3d_label_p l);

        /* ---------- 2D UI: input (仿 jr2d inputbox, 屏幕像素坐标, 不随相机变化) ---------- */
        /* 输入框对象(屏幕像素坐标, 原点左上) */
        typedef struct jr3d_input jr3d_input_t, * jr3d_input_p;
        struct jr3d_input {
            float x, y, w, h;       /* 位置与尺寸(像素) */
            float fontSize;         /* 字号(像素) */
            uint8_t r, g, b;        /* 文字颜色 */
            char  text[256];        /* 文本缓冲区 */
            int   cursor_pos;       /* 光标位置(字节偏移, UTF-8 安全) */
            int   scroll;           /* 文本水平滚动偏移(像素) */
            bool  focused;          /* 是否聚焦 */
            uint64_t cursor_timer;  /* 光标闪烁计时 */
            bool  cursor_visible;   /* 光标是否可见 */
            int   sel_start;        /* 选中起点(字节偏移); <0 表示无选中 */
            int   sel_end;          /* 选中终点(字节偏移) */
            void (*on_submit)(jr3d_input_p);  /* 回车提交回调 */
        };

        /* 创建输入框(屏幕像素坐标, 原点左上) */
        JOY_API jr3d_input_p r3d_input_create(float x, float y, float w, float h, float fontSize,
                                              uint8_t cr, uint8_t cg, uint8_t cb, const char* text);
        /* 设置/获取文本 */
        JOY_API void   r3d_input_set_text(jr3d_input_p inp, const char* text);
        JOY_API const char* r3d_input_get_text(jr3d_input_p inp);
        /* 命中测试: 屏幕像素坐标 (mx,my) 是否在输入框内 */
        JOY_API bool   r3d_input_is_hovered(const jr3d_input_p inp, float mx, float my);
        /* 聚焦/失焦 */
        JOY_API void   r3d_input_focus(jr3d_input_p inp);
        JOY_API void   r3d_input_defocus(jr3d_input_p inp);
        JOY_API bool   r3d_input_is_focused(const jr3d_input_p inp);
        /* 在光标处插入字符串(支持中文等多字节) */
        JOY_API void   r3d_input_insert(jr3d_input_p inp, const char* s);
        /* 删除光标前/后的 UTF-8 字符(退格/Delete) */
        JOY_API void   r3d_input_backspace(jr3d_input_p inp);
        JOY_API void   r3d_input_delete(jr3d_input_p inp);
        /* 光标移动(UTF-8 安全): 左/右/行首/行尾 */
        JOY_API void   r3d_input_cursor_left(jr3d_input_p inp);
        JOY_API void   r3d_input_cursor_right(jr3d_input_p inp);
        JOY_API void   r3d_input_cursor_home(jr3d_input_p inp);
        JOY_API void   r3d_input_cursor_end(jr3d_input_p inp);
        /* 鼠标点击定位光标: 屏幕像素坐标 (mx,my) -> cursor_pos(并聚焦) */
        JOY_API void   r3d_input_click(jr3d_p r, jr3d_input_p inp, float mx, float my);
        /* 统一事件处理(对齐 jr2d inputbox_process_event): 内部处理
           鼠标聚焦/定位、文本输入、键盘(退格/删除/方向/Home/End/回车)。
           返回 true 表示事件已被输入框消费(调用方应停止后续处理, 如相机旋转)。 */
        JOY_API bool   r3d_input_handle_event(jr3d_p r, jr3d_input_p inp, SDL_Event* event);
        /* 设置回车提交回调 */
        JOY_API void   r3d_input_set_onsubmit(jr3d_input_p inp, void (*cb)(jr3d_input_p));
        /* 触发提交(调用 on_submit); 供回车事件调用 */
        JOY_API void   r3d_input_submit(jr3d_input_p inp);
        /* 绘制输入框(需先 r3d_font_init; m 为单位 quad) */
        JOY_API void   r3d_input_draw(jr3d_p r, render_mesh_p m, jr3d_input_p inp, float winW, float winH);
        /* 销毁输入框 */
        JOY_API void   r3d_input_destroy(jr3d_input_p inp);

        /* ---------- 绘制 ---------- */
        JOY_API void r3d_draw_mesh(jr3d_p r, render_mesh_p m, vec3_t pos, vec3_t scale, jcolor_t color);
        JOY_API void r3d_draw_mesh_tex(jr3d_p r, render_mesh_p m, vec3_t pos, vec3_t scale, void* tex);
        /* billboard 绘制: 用 center + right + up 基向量(无需归一化, 内部按 size 缩放) 把单位 quad 朝向相机 */
        JOY_API void r3d_draw_mesh_tex_basis(jr3d_p r, render_mesh_p m,
                vec3_t center, vec3_t right, vec3_t up, float size, void* tex);
        JOY_API void r3d_draw_grid(jr3d_p r, int slices, float spacing, float line_width);

        /* ---------- 光照渲染（顶点格式 pos+normal+uv, pitch=32） ---------- */
        /* 创建光照管线: 顶点 uniform slot0 = render_mvp_t(MVP),
           片元 uniform slot0 = jr3d_light_block_t, 片元采样器 slot0 = 纹理 */
        JOY_API void* r3d_create_lighting_pipeline(void* device,
                void* vertex_shader, void* fragment_shader,
                int swapchain_format, int depth_format);
        /* 带法线的网格生成（pos+normal+uv, 8 float/顶点） */
        JOY_API render_mesh_p r3d_mesh_cube_lit(jr3d_p r, float w, float h, float d);
        JOY_API render_mesh_p r3d_mesh_sphere_lit(jr3d_p r, float radius, int slices, int stacks);
        JOY_API render_mesh_p r3d_mesh_plane_lit(jr3d_p r, float w, float d, int segs);
        JOY_API render_mesh_p r3d_mesh_ramp_lit(jr3d_p r, float w, float l, float h);
        JOY_API render_mesh_p r3d_mesh_custom_lit(jr3d_p r, const void* verts, int vertex_count);
        /* 设置光照数据; 相机更新时 viewPos 会自动覆盖为当前相机位置 */
        JOY_API void r3d_set_light_block(jr3d_p r, const jr3d_light_block_t* block);
        /* 用光照管线绘制网格（需在 r3d_begin/r3d_end 之间调用） */
        JOY_API void r3d_draw_mesh_lit(jr3d_p r, render_mesh_p m, const mat44_t* model, void* tex);
        JOY_API void r3d_draw_mesh_lit_color(jr3d_p r, render_mesh_p m, const mat44_t* model, jcolor_t color);

        ///////////////////////////////////////////////////////////////////////
       ////////////////////////////  collision ///////////////////////////////
       ///////////////////////////////////////////////////////////////////////


        JOY_API ray2d_collision_t collision2d_get_ray_circle(ray2d_t ray, circle_t circle);
        JOY_API ray2d_collision_t collision2d_get_ray_rectangle(ray2d_t ray, rectangle_t rect);
        JOY_API ray2d_collision_t collision2d_get_ray_rectanglex(ray2d_t ray, rectangle_t rect, ft_t angle);
        JOY_API ray2d_collision_t collision2d_get_ray_polygon(ray2d_t ray, polygon_t polygon);
        JOY_API bool collision2d_check_point_rectangle(vec2_t point, rectangle_t rect);
        JOY_API bool collision2d_check_point_circle(vec2_t point, circle_t circle);
        JOY_API ft_t collision2d_get_squared_distance_point_circle(vec2_t point, circle_t circle);
        JOY_API bool collision2d_check_point_triangle(vec2_t point, vec2_t p1, vec2_t p2, vec2_t p3);
        JOY_API bool collision2d_check_lines(vec2_t a1, vec2_t b1, vec2_t a2, vec2_t b2, vec2_t* cp);
        JOY_API bool collision2d_check_point_line(vec2_t point, vec2_t p1, vec2_t p2, ft_t threshold);
        JOY_API bool collision2d_check_circles(circle_t a, circle_t b);
        JOY_API bool collision2d_check_rectangles(rectangle_t a, rectangle_t b);
        JOY_API bool collision2d_check_polygons(polygon_t a, polygon_t b);
        JOY_API bool collision2d_check_circle_rectangle(circle_t a, rectangle_t b);
        JOY_API bool collision2d_check_circle_polygon(circle_t a, polygon_t b);
        JOY_API int collision2d_get_circles(circle_t a, circle_t b, contact2d_t* contact);
        JOY_API int collision2d_get_rectangles(rectangle_t a, ft_t a_angle, rectangle_t b, ft_t b_angle, contact2d_t* contact);
        JOY_API int collision2d_get_polygons(polygon_t a, polygon_t b, contact2d_t* contact);
        JOY_API ray3d_collision_t collision3d_get_ray_triangle(ray3d_t ray, vec3_t p1, vec3_t p2, vec3_t p3);
        JOY_API ray3d_collision_t collision3d_get_ray_quad(ray3d_t ray, vec3_t p1, vec3_t p2, vec3_t p3, vec3_t p4);
        JOY_API ray3d_collision_t collision3d_get_ray_sphere(ray3d_t ray, sphere_t sphere);
        JOY_API ray3d_collision_t collision3d_get_ray_box(ray3d_t ray, bounding_box_t box);
        JOY_API bool collision3d_check_spheres(sphere_t a, sphere_t b);
        JOY_API bool collision3d_check_boxes(bounding_box_t a, bounding_box_t b);
        JOY_API bool collision3d_check_box_sphere(bounding_box_t a, sphere_t b);

        JOY_API ray2d_collisionf_t collision2df_get_ray_circle(ray2df_t ray,
                circlef_t circle);
        JOY_API ray2d_collisionf_t collision2df_get_ray_rectangle(ray2df_t ray,
                rectanglef_t rect);
        JOY_API ray2d_collisionf_t collision2df_get_ray_rectanglex(ray2df_t ray,
                rectanglef_t rect, fp_t angle /* 旋转角, 单位: 度 */);
        JOY_API ray2d_collisionf_t collision2df_get_ray_polygon(ray2df_t ray,
                polygonf_t polygon);
        JOY_API bool collision2df_check_point_rectangle(vec2f_t point,
                rectanglef_t rect);
        JOY_API bool collision2df_check_point_circle(vec2f_t point, circlef_t circle);
        JOY_API fp_t collision2df_get_squared_distance_point_circle(vec2f_t point, circlef_t circle);
        JOY_API bool collision2df_check_point_triangle(vec2f_t point,
                vec2f_t p1, vec2f_t p2, vec2f_t p3);
        JOY_API bool collision2df_check_lines(vec2f_t a1, vec2f_t b1,
                vec2f_t a2, vec2f_t b2, vec2f_t* cp);
        JOY_API bool collision2df_check_point_line(vec2f_t point,
                vec2f_t p1, vec2f_t p2, fp_t threshold);
        JOY_API bool collision2df_check_circles(circlef_t a, circlef_t b);
        JOY_API bool collision2df_check_rectangles(rectanglef_t a, rectanglef_t b);
        JOY_API bool collision2df_check_polygons(polygonf_t a, polygonf_t b);
        JOY_API bool collision2df_check_circle_rectangle(circlef_t a, rectanglef_t b);
        JOY_API bool collision2df_check_circle_polygon(circlef_t a, polygonf_t b);
        JOY_API int collision2df_get_circles(circlef_t a, circlef_t b,
                contact2df_t* contact);
        JOY_API int collision2df_get_rectangles(rectanglef_t a, fp_t a_angle /* 度 */,
                rectanglef_t b, fp_t b_angle /* 度 */,
                contact2df_t* contact);
        JOY_API fp_t find_min_separation(polygonf_t a, polygonf_t b,
                vec2f_t* axis, vec2f_t* point);
        JOY_API int collision2df_get_polygons(polygonf_t a, polygonf_t b,
                contact2df_t* contact);
        JOY_API ray3d_collisionf_t collision3df_get_ray_triangle(ray3df_t ray,
                vec3f_t p1, vec3f_t p2, vec3f_t p3);
        JOY_API ray3d_collisionf_t collision3df_get_ray_quad(ray3df_t ray,
                vec3f_t p1, vec3f_t p2, vec3f_t p3, vec3f_t p4);
        JOY_API ray3d_collisionf_t collision3df_get_ray_sphere(ray3df_t ray,
                spheref_t sphere);
        JOY_API ray3d_collisionf_t collision3df_get_ray_box(ray3df_t ray,
                bounding_boxf_t box);
        JOY_API bool collision3df_check_spheres(spheref_t a, spheref_t b);
        JOY_API bool collision3df_check_boxes(bounding_boxf_t a,
                bounding_boxf_t b);
        JOY_API bool collision3df_check_box_sphere(bounding_boxf_t a,
                spheref_t b);


        /* 主碰撞检测: GJK + EPA, 返回是否碰撞, contact填充法线和深度 */
        JOY_API bool gjk2d_collide(gjk2d_collider_p c1, gjk2d_collider_p c2,
                vec2f_t init_dir, gjk2d_contact_p contact);
        JOY_API void gjk2d_init_circle(gjk2d_collider_t* c, vec2f_t pos, fp_t radius);
        JOY_API void gjk2d_init_box(gjk2d_collider_t* c, vec2f_t pos, vec2f_t half_size);
        JOY_API void gjk2d_init_capsule(gjk2d_collider_t* c, vec2f_t pos, vec2f_t axis, fp_t height, fp_t radius);
        JOY_API void gjk2d_init_polygon(gjk2d_collider_t* c, vec2f_t* vertices, int num);
        JOY_API void gjk2d_init_pie(gjk2d_collider_t* c, vec2f_t pos, fp_t radius, fp_t sweep);
        JOY_API bool gjk3df_collide(gjk3df_collider_p c1, gjk3df_collider_p c2,
                vec3f_t init_dir, gjk3df_contact_p contact);
        /* 快捷初始化函数(仅设置参数,无内存分配) */
        JOY_API void gjk3df_init_sphere(gjk3df_collider_t* c, vec3f_t pos, fp_t radius);
        JOY_API void gjk3df_init_box(gjk3df_collider_t* c, vec3f_t pos, vec3f_t half_size);
        JOY_API void gjk3df_init_obb(gjk3df_collider_t* c, vec3f_t pos, vec3f_t half_size, vec3f_t rotation);
        JOY_API void gjk3df_init_capsule(gjk3df_collider_t* c, vec3f_t pos, fp_t radius, fp_t height, vec3f_t axis);
        JOY_API void gjk3df_init_cylinder(gjk3df_collider_t* c, vec3f_t pos, fp_t radius, fp_t height);
        JOY_API void gjk3df_init_cone(gjk3df_collider_t* c, vec3f_t pos, fp_t radius, fp_t height);
        JOY_API void gjk3df_init_mesh(gjk3df_collider_t* c, vec3f_t* vertices, int num);

        /* ============ GJK 3D 浮点版 (ft_t/vec3_t) ============ */
        /* 主碰撞检测: GJK + EPA, 返回是否碰撞, contact填充法线和深度 */
        JOY_API bool gjk3d_collide(gjk3d_collider_p c1, gjk3d_collider_p c2,
                vec3_t init_dir, gjk3d_contact_p contact);
        /* 快捷初始化函数(仅设置参数,无内存分配) */
        JOY_API void gjk3d_init_sphere(gjk3d_collider_t* c, vec3_t pos, ft_t radius);
        JOY_API void gjk3d_init_box(gjk3d_collider_t* c, vec3_t pos, vec3_t half_size);
        JOY_API void gjk3d_init_capsule(gjk3d_collider_t* c, vec3_t pos, ft_t radius, ft_t height, vec3_t axis);
        JOY_API void gjk3d_init_cylinder(gjk3d_collider_t* c, vec3_t pos, ft_t radius, ft_t height);
        JOY_API void gjk3d_init_cone(gjk3d_collider_t* c, vec3_t pos, ft_t radius, ft_t height);
        JOY_API void gjk3d_init_mesh(gjk3d_collider_t* c, vec3_t* vertices, int num);

        JOY_API janimator_p janimator_create(jr2d_p r2d);
        JOY_API void janimator_destroy(janimator_p anim);
        JOY_API void janimator_set_float(janimator_p anim, const char* name, float v);
        JOY_API void janimator_set_int(janimator_p anim, const char* name, int v);
        JOY_API void janimator_set_bool(janimator_p anim, const char* name, bool v);
        JOY_API void janimator_set_trigger(janimator_p anim, const char* name);
        JOY_API float janimator_get_float(janimator_p anim, const char* name);
        JOY_API int janimator_get_int(janimator_p anim, const char* name);
        JOY_API bool janimator_get_bool(janimator_p anim, const char* name);
        JOY_API void janimator_add_state(janimator_p anim, const char* name);
        JOY_API void janimator_set_default_state(janimator_p anim, const char* name);
        JOY_API void janimator_state_add_clip(janimator_p anim, const char* state_name,
                const char* image_path, float duration, jrect_t src_rect);
        JOY_API void janimator_state_add_color(janimator_p anim, const char* state_name,
                float duration, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        JOY_API void janimator_state_set_loop(janimator_p anim, const char* state_name, bool loop);
        JOY_API void janimator_state_set_speed(janimator_p anim, const char* state_name, float speed);
        JOY_API void janimator_state_set_blend(janimator_p anim, const char* state_name,
                const char* param_name);
        JOY_API void janimator_state_add_motion(janimator_p anim, const char* state_name, const char* image_path, float duration, jrect_t src_rect, float threshold);
        JOY_API int janimator_add_transition(janimator_p anim, const char* from, const char* to);
        JOY_API int janimator_add_any_transition(janimator_p anim, const char* to);
        JOY_API void janimator_transition_add_cond(janimator_p anim, int trans_idx,
                const char* param_name, int mode, float threshold);
        JOY_API void janimator_transition_set_exit_time(janimator_p anim, int trans_idx, float seconds);
        JOY_API void janimator_update(janimator_p anim, float dt);
        JOY_API void janimator_draw(janimator_p anim, float x, float y);
        JOY_API void janimator_set_flip_h(janimator_p anim, bool flip);
        JOY_API const char* janimator_current_state(janimator_p anim);
        JOY_API void janimator_set_on_enter(janimator_p anim, janim_state_cb cb, void* userdata);
        JOY_API void janimator_set_on_exit(janimator_p anim, janim_state_cb cb, void* userdata);


        JOY_API label_p label_create(jr2d_p r2d, font_p fontinfo, const int* codepoints, int len);
        JOY_API void label_destroy(label_p label);
        JOY_API void label_set_text(label_p label, font_p fontinfo, const int* codepoints, int len, jcolor_t color);
        JOY_API void label_draw(label_p label);
        JOY_API void label_process_event(label_p label, const void* event);
        JOY_API button_p button_create(jr2d_p r2d, float width, float height);
        JOY_API void button_destroy(button_p btn);
        JOY_API void button_set_position(button_p btn, float x, float y);
        JOY_API vec2_t button_get_position(button_p btn);
        JOY_API void button_set_normal_color(button_p btn, jcolor_t color);
        JOY_API void button_set_hover_color(button_p btn, jcolor_t color);
        JOY_API void button_set_pressed_color(button_p btn, jcolor_t color);
        JOY_API void button_set_image(button_p btn, image_p image);
        JOY_API void button_set_pressed_image(button_p btn, image_p image);
        JOY_API void button_set_textx(button_p btn, font_p fontinfo, const char* str, int len, jcolor_t color);
        JOY_API void button_set_text(button_p btn, font_p fontinfo, const int* codepoints, int len, jcolor_t color);
        JOY_API bool button_is_hovered(button_p btn, float x, float y);
        JOY_API bool button_is_pressed(button_p btn, float x, float y);
        JOY_API void button_set_hovered(button_p btn, bool v);
        JOY_API void button_set_pressed(button_p btn, bool v);
        JOY_API void button_set_text_offset(button_p btn, float x, float y);
        JOY_API void button_draw(button_p btn);
        JOY_API void button_process_event(button_p btn, const void* event);
        JOY_API combobox_p combobox_create(jr2d_p r2d, jrect_t rect);
        JOY_API void combobox_destroy(combobox_p combobox);
        JOY_API void combobox_set_text(combobox_p combobox, font_p font,
                const int* codepoints, int len, jcolor_t color);
        JOY_API void combobox_add_item(combobox_p combobox, font_p font,
                const char* str, int len, jcolor_t color);
        JOY_API void combobox_add_itemx(combobox_p combobox, font_p font,
                const int* codepoints, int len, jcolor_t color);

        JOY_API bool combobox_is_pressed(combobox_p combobox, float x, float y);
        JOY_API bool combobox_is_open(combobox_p combobox);
        JOY_API void combobox_set_open(combobox_p combobox, bool v);
        JOY_API int combobox_hit_item(combobox_p combobox, float x, float y);
        JOY_API void combobox_set_selected(combobox_p combobox, int index);
        JOY_API int combobox_get_selected(combobox_p combobox);
        JOY_API void combobox_draw(combobox_p combobox);
        JOY_API void combobox_process_event(combobox_p cb, const void* event);

        // 可输入下拉列表框
        JOY_API combobox_input_p combobox_input_create(jr2d_p r2d, jrect_t rect, font_p font, int buf_len);
        JOY_API void combobox_input_destroy(combobox_input_p ci);
        JOY_API void combobox_input_add_item(combobox_input_p ci, const char* str, int len);
        JOY_API void combobox_input_clear_items(combobox_input_p ci);
        JOY_API const char* combobox_input_get_text(combobox_input_p ci);
        JOY_API void combobox_input_set_text(combobox_input_p ci, const char* str);
        JOY_API void combobox_input_draw(combobox_input_p ci);
        JOY_API void combobox_input_draw_dropdown(combobox_input_p ci);
        JOY_API bool combobox_input_is_open(combobox_input_p ci);
        JOY_API void combobox_input_set_popup_node(combobox_input_p ci, scene_node_p node);
        JOY_API void combobox_input_process_event(combobox_input_p ci, const void* ev);
        JOY_API int combobox_input_hit_item(combobox_input_p ci, float x, float y);

        JOY_API datagrid_p datagrid_create(jr2d_p r2d, jrect_t rect, int col_count, bool has_header, void* font);
        JOY_API void datagrid_setheaders(datagrid_p grid, const char* headers);
        JOY_API void datagrid_setheader(datagrid_p grid, int index, const char* header);
        JOY_API void datagrid_addrow(datagrid_p grid, const char** row_data);
        JOY_API void datagrid_setcell(datagrid_p grid, int row_index, int col_index, const char* data);
        JOY_API void datagrid_destroy(datagrid_p grid);
        JOY_API void datagrid_draw(datagrid_p grid, jr2d_p r2d);
        JOY_API int datagrid_hit_row(datagrid_p grid, float x, float y);
        JOY_API void datagrid_scroll(datagrid_p grid, int lines);
        JOY_API void datagrid_set_selected_row(datagrid_p grid, int row);
        JOY_API void datagrid_process_event(datagrid_p dg, const void* event);
        JOY_API radio_p radio_create(int x, int y, int size, const char* label, int group_id);
        JOY_API void radio_destroy(radio_p radio);
        JOY_API bool radio_is_pressed(radio_p radio, float x, float y);
        JOY_API void radio_set_hovered(radio_p radio, bool v);
        JOY_API void radio_process_event(radio_p rb, const void* event);
        JOY_API checkbox_p checkbox_create(int x, int y, int size, const char* label);
        JOY_API void checkbox_destroy(checkbox_p checkbox);
        JOY_API void checkbox_set_state(checkbox_p checkbox, checkbox_state_t state);
        JOY_API checkbox_state_t checkbox_get_state(checkbox_p checkbox);
        JOY_API void checkbox_set_colors(checkbox_p checkbox, jcolor_t box, jcolor_t check, jcolor_t hover);
        JOY_API void checkbox_set_r2d(checkbox_p checkbox, jr2d_p r2d);
        JOY_API void checkbox_toggle(checkbox_p checkbox);
        JOY_API bool checkbox_is_pressed(checkbox_p cb, float x, float y);
        JOY_API void checkbox_set_hovered(checkbox_p cb, bool v);
        JOY_API void checkbox_draw(checkbox_p cb, jr2d_p r2d, void* font);
        JOY_API void checkbox_process_event(checkbox_p cb, const void* event);
        JOY_API joystick_p joystick_create(jr2d_p r2d, float x, float y, float radius);
        JOY_API void joystick_destroy(joystick_p joystick);
        JOY_API void joystick_draw(joystick_p joystick);
        JOY_API void joystick_reset(joystick_p joystick);
        JOY_API void joystick_set_position(joystick_p joystick, float x, float y);
        JOY_API bool joystick_is_dragging(joystick_p joystick);
        JOY_API void joystick_touch_down(joystick_p joystick, float x, float y);
        JOY_API void joystick_touch_move(joystick_p joystick, float x, float y);
        JOY_API void joystick_touch_up(joystick_p joystick);
        JOY_API vec2_t joystick_get_direction(joystick_p joystick);
        JOY_API float joystick_get_magnitude(joystick_p joystick);
        JOY_API void joystick_process_event(joystick_p js, const void* event);
        JOY_API inputbox_p inputbox_create(jr2d_p r2d, jrect_t rect, font_p font);
        JOY_API void inputbox_destroy(inputbox_p ti);
        JOY_API void inputbox_set_text(inputbox_p ti, const char* text);
        JOY_API const char* inputbox_get_text(inputbox_p ti);
        JOY_API void inputbox_set_callback(inputbox_p ti, void (*submit)(inputbox_p, void*), void (*change)(inputbox_p, void*), void* userdata);
        JOY_API void inputbox_draw(inputbox_p ti);
        JOY_API bool inputbox_is_pressed(inputbox_p ti, float x, float y);
        JOY_API bool inputbox_is_hovered(inputbox_p ti, float x, float y);
        JOY_API void inputbox_focus(inputbox_p ti);
        JOY_API void inputbox_defocus(inputbox_p ti);
        JOY_API void inputbox_insert(inputbox_p ti, const char* s, int len);
        JOY_API void inputbox_backspace(inputbox_p ti);
        JOY_API void inputbox_delete(inputbox_p ti);
        JOY_API void inputbox_cursor_left(inputbox_p ti);
        JOY_API void inputbox_cursor_right(inputbox_p ti);
        JOY_API void inputbox_cursor_home(inputbox_p ti);
        JOY_API void inputbox_cursor_end(inputbox_p ti);
        JOY_API void inputbox_select_left(inputbox_p ti);
        JOY_API void inputbox_select_right(inputbox_p ti);
        JOY_API void inputbox_select_home(inputbox_p ti);
        JOY_API void inputbox_select_end(inputbox_p ti);
        JOY_API void inputbox_select_all(inputbox_p ti);
        JOY_API void inputbox_paste(inputbox_p ti);
        JOY_API void inputbox_copy(inputbox_p ti);
        JOY_API void inputbox_cut(inputbox_p ti);
        JOY_API bool inputbox_is_focused(inputbox_p ti);
        JOY_API void inputbox_process_event(inputbox_p ti, const void* event);
        JOY_API slider_p slider_create(jr2d_p r2d, float x, float y, float w, float h, float min, float max, float initial);
        JOY_API void slider_destroy(slider_p s);
        JOY_API void slider_set_value(slider_p s, float v);
        JOY_API float slider_get_value(slider_p s);
        JOY_API void slider_set_colors(slider_p s, jcolor_t track, jcolor_t fill, jcolor_t thumb, jcolor_t thumb_hover);
        JOY_API void slider_set_range(slider_p s, float min, float max);
        JOY_API void slider_draw(slider_p s);
        JOY_API bool slider_is_pressed(slider_p s, float x, float y);
        JOY_API void slider_set_pressed(slider_p s, bool v);
        JOY_API void slider_drag_move(slider_p s, float x, float y);
        JOY_API bool slider_is_hovered(slider_p s, float x, float y);
        JOY_API void slider_set_hovered(slider_p s, bool v);
        JOY_API void slider_process_event(slider_p s, const void* event);
        JOY_API popup_p popup_create(jr2d_p r2d, float w, float h);
        JOY_API void popup_destroy(popup_p p);
        JOY_API void popup_set_title(popup_p p, const char* text, int len);
        JOY_API void popup_set_content(popup_p p, const char* text, int len);
        JOY_API void popup_set_position(popup_p p, float x, float y);
        JOY_API void popup_set_visible(popup_p p, bool v);
        JOY_API void popup_set_closeable(popup_p p, bool v);
        JOY_API void popup_draw(popup_p p, font_p title_font, font_p content_font);
        JOY_API bool popup_is_close_clicked(popup_p p, float mx, float my);
        JOY_API bool popup_is_title_clicked(popup_p p, float mx, float my);
        JOY_API void popup_drag_move(popup_p p, float mx, float my);
        JOY_API bool popup_is_inside(popup_p p, float mx, float my);
        JOY_API void popup_process_event(popup_p p, const void* event);
        JOY_API tooltip_p tooltip_create(jr2d_p r2d, const char* text, int len);
        JOY_API void tooltip_destroy(tooltip_p t);
        JOY_API void tooltip_set_text(tooltip_p t, const char* text, int len);
        JOY_API void tooltip_set_position(tooltip_p t, float x, float y);
        JOY_API void tooltip_set_visible(tooltip_p t, bool v);
        JOY_API void tooltip_draw(tooltip_p t, font_p font);
        JOY_API void tooltip_process_event(tooltip_p t, const void* event);
        JOY_API void jui_process_event(void** widgets, int count, const void* event);

        JOY_API jtilemap_p jtilemap_load_file(const char* filename);
        JOY_API void jtilemap_destroy(jtilemap_p tm);
        JOY_API int jtilemap_get_width(const jtilemap_p tm);
        JOY_API int jtilemap_get_height(const jtilemap_p tm);
        JOY_API jtilemap_gid_resolve_t jtilemap_resolve_gid(const jtilemap_p tm, int gid);
        JOY_API const char* jtilemap_get_tile_image(const jtilemap_tileset_p ts, int local_id);
        JOY_API bool jtilemap_get_tile_def_size(const jtilemap_tileset_p ts, int local_id, int* out_w, int* out_h);
        JOY_API bool jtilemap_get_tile_uv(const jtilemap_p tm, int gid,
                uint32_t time_ms, float out_rect[4]);
        JOY_API int jtilemap_get_anim_tileid(const jtilemap_p tm, int gid,
                uint32_t time_ms);
        JOY_API jtilemap_tile_layer_p jtilemap_find_tile_layer(const jtilemap_p tm,
                const char* name);
        JOY_API jtilemap_object_layer_p jtilemap_find_object_layer(const jtilemap_p tm, const char* name);
        JOY_API void jtilemap_object_layer_foreach(jtilemap_object_layer_p ol, void (*callback)(jtilemap_object_p obj, void*arg), void* arg);
        JOY_API int jtilemap_get_tile(const jtilemap_tile_layer_p layer,
                int tx, int ty);
        JOY_API jtilemap_render_p jtilemap_render_create(jr2d_p renderer,
                const char* asset_base_path,
                jtilemap_path_remap_t remap);
        JOY_API void jtilemap_render_all(jtilemap_render_p r,
                jtilemap_p tm,
                float camera_x, float camera_y,
                uint32_t time_ms);
        JOY_API bool jtilemap_render_layer(jtilemap_render_p r,
                jtilemap_p tm,
                const char* layer_name,
                float camera_x, float camera_y,
                uint32_t time_ms);
        JOY_API void jtilemap_render_destroy(jtilemap_render_p r);

        // ==============================
        // KCP Server & Client
        // ==============================
        JOY_API kcpserver_p kcpserver_create(const char* ip, int port);
        JOY_API int kcpserver_destroy(kcpserver_p ks);
        JOY_API void kcpserver_send(kcpserver_p ks, int conv, const char* data, int len);
        JOY_API void kcpserver_broadcast(kcpserver_p ks, const char* data, int len);
        JOY_API void kcpserver_offline(kcpserver_p ks, int conv);
        JOY_API void kcpserver_update(kcpserver_p ks);
        JOY_API int kcpserver_connection_count(kcpserver_p ks);
        JOY_API bool kcpserver_poll_message(kcpserver_p ks, net_message_p msg);
        JOY_API void kcpserver_set_callback(kcpserver_p ks, net_callback cb, void* userdata);
        JOY_API kcpclient_p kcpclient_create(const char* ip, int port);
        JOY_API void kcpclient_destroy(kcpclient_p kc);
        JOY_API int kcpclient_send(kcpclient_p kc, const char* data, int len);
        JOY_API void kcpclient_update(kcpclient_p kc);
        JOY_API bool kcpclient_poll_message(kcpclient_p kc, net_message_p msg);
        JOY_API void kcpclient_set_callback(kcpclient_p kc, net_callback cb, void* userdata);
        JOY_API tcpserver_p tcpserver_create(const char* ip, int port);
        JOY_API int tcpserver_destroy(tcpserver_p tcpserver);
        JOY_API void tcpserver_send(tcpserver_p tcpserver, int conv, const char* data, int len);
        JOY_API void tcpserver_broadcast(tcpserver_p tcpserver, const char* data, int len);
        JOY_API void tcpserver_offline(tcpserver_p tcpserver, int conv);
        JOY_API void tcpserver_update(tcpserver_p tcpserver);
        JOY_API int tcpserver_connection_count(tcpserver_p tcpserver);
        JOY_API bool tcpserver_poll_message(tcpserver_p tcpserver, net_message_p msg);
        JOY_API tcpclient_p tcpclient_create(const char* ip, int port);
        JOY_API void tcpclient_destroy(tcpclient_p tcpclient);
        JOY_API int tcpclient_send(tcpclient_p tcpclient, const char* data, int len);
        JOY_API void tcpclient_update(tcpclient_p tcpclient);
        JOY_API bool tcpclient_poll_message(tcpclient_p tcpclient, net_message_p msg);
        JOY_API wsnetserver_p wsnetserver_create(const char* ip, int port);
        JOY_API int wsnetserver_destroy(wsnetserver_p ws);
        JOY_API void wsnetserver_send(wsnetserver_p ws, int conv, const char* data, int len);
        JOY_API void wsnetserver_broadcast(wsnetserver_p ws, const char* data, int len);
        JOY_API void wsnetserver_offline(wsnetserver_p ws, int conv);
        JOY_API void wsnetserver_update(wsnetserver_p ws);
        JOY_API int wsnetserver_connection_count(wsnetserver_p ws);
        JOY_API bool wsnetserver_poll_message(wsnetserver_p ws, net_message_p msg);
        JOY_API void wsnetserver_set_callback(wsnetserver_p ws, net_callback cb, void* userdata);
        JOY_API wsnetclient_p wsnetclient_create(const char* url);
        JOY_API void wsnetclient_destroy(wsnetclient_p wc);
        JOY_API int wsnetclient_send(wsnetclient_p wc, const char* data, int len);
        JOY_API void wsnetclient_update(wsnetclient_p wc);
        JOY_API bool wsnetclient_poll_message(wsnetclient_p wc, net_message_p msg);
        JOY_API void wsnetclient_set_callback(wsnetclient_p wc, net_callback cb, void* userdata);
        JOY_API netclient_p netclient_create(net_client_type type, const char* host, int port);
        JOY_API void netclient_destroy(netclient_p nc);
        JOY_API int netclient_send(netclient_p nc, const char* data, int len);
        JOY_API void netclient_update(netclient_p nc);
        JOY_API bool netclient_poll_message(netclient_p nc, net_message_p msg);
        JOY_API void netclient_set_callback(netclient_p nc, net_callback cb, void* userdata);
        JOY_API net_client_type netclient_get_type(netclient_p nc);
        JOY_API netserver_p netserver_create(net_server_type type, const char* ip, int port);
        JOY_API void netserver_destroy(netserver_p ns);
        JOY_API void netserver_send(netserver_p ns, int conv, const char* data, int len);
        JOY_API void netserver_broadcast(netserver_p ns, const char* data, int len);
        JOY_API void netserver_offline(netserver_p ns, int conv);
        JOY_API void netserver_update(netserver_p ns);
        JOY_API int netserver_connection_count(netserver_p ns);
        JOY_API bool netserver_poll_message(netserver_p ns, net_message_p msg);
        JOY_API void netserver_set_callback(netserver_p ns, net_callback cb, void* userdata);
        JOY_API net_server_type netserver_get_type(netserver_p ns);
        JOY_API void netmessage_free_data(net_message_p msg);


        JOY_API scene_node_p scene_node_create(void);
        JOY_API void scene_node_destroy(scene_node_p node);
        JOY_API void scene_node_add_child(scene_node_p parent, scene_node_p child);
        JOY_API void scene_node_remove_child(scene_node_p parent, scene_node_p child);
        JOY_API scene_node_p scene_node_get_parent(scene_node_p node);
        JOY_API scene_node_p scene_node_get_first_child(scene_node_p node);
        JOY_API scene_node_p scene_node_get_next_sibling(scene_node_p node);
        JOY_API int scene_node_get_child_count(scene_node_p node);
        JOY_API void scene_node_set_position(scene_node_p node, float x, float y);
        JOY_API void scene_node_get_position(scene_node_p node, float* out_x, float* out_y);
        JOY_API void scene_node_set_world_position(scene_node_p node, float x, float y);
        JOY_API void scene_node_get_world_position(scene_node_p node, float* out_x, float* out_y);
        JOY_API void scene_node_set_rotation(scene_node_p node, float radians);
        JOY_API float scene_node_get_rotation(scene_node_p node);
        JOY_API void scene_node_set_scale(scene_node_p node, float scale_x, float scale_y);
        JOY_API void scene_node_get_scale(scene_node_p node, float* out_scale_x, float* out_scale_y);
        JOY_API void scene_node_set_zorder(scene_node_p node, int zorder);
        JOY_API int scene_node_get_zorder(scene_node_p node);
        JOY_API void scene_node_set_userdata(scene_node_p node, void* userdata);
        JOY_API void* scene_node_get_userdata(scene_node_p node);
        JOY_API void scene_node_set_name(scene_node_p node, const char* name);
        JOY_API const char* scene_node_get_name(scene_node_p node);
        JOY_API void scene_node_set_tag(scene_node_p node, const char* tag);
        JOY_API const char* scene_node_get_tag(scene_node_p node);
        JOY_API scene_node_p scene_find_node_by_name(scene_node_p root, const char* name);
        JOY_API scene_node_p scene_find_node_by_name_deep(scene_p scene, const char* name);
        JOY_API int  scene_find_nodes_by_tag(scene_node_p root, const char* tag, scene_node_p* out, int max);
        JOY_API int  scene_find_nodes_by_tag_deep(scene_p scene, const char* tag, scene_node_p* out, int max);
        JOY_API void scene_node_set_update_callback(scene_node_p node, scene_node_update_fn cb);
        JOY_API void scene_node_set_render_callback(scene_node_p node, scene_node_render_fn cb);
        JOY_API void scene_node_set_destroy_callback(scene_node_p node, scene_node_destroy_fn cb);
        JOY_API void scene_node_set_event_callback(scene_node_p node, scene_node_event_fn cb);
        JOY_API void scene_node_handle_event(scene_node_p node, const void* e);
        JOY_API void scene_node_update(scene_node_p node, float delta_time);
        JOY_API void scene_node_render(scene_node_p node, const void* arg);
        JOY_API scene_p scene_create(const char* name);
        JOY_API void scene_destroy(scene_p scene);
        JOY_API void scene_add_root_node(scene_p scene, scene_node_p root);
        JOY_API void scene_remove_root_node(scene_p scene, scene_node_p root);
        JOY_API int scene_get_root_count(scene_p scene);
        JOY_API scene_node_p scene_get_root_at(scene_p scene, int index);
        JOY_API void scene_update(scene_p scene, float delta_time);
        JOY_API void scene_render(scene_p scene, const void* arg);
        JOY_API void scene_handle_event(scene_p scene, const void* e);
        JOY_API void scene_set_load_callback(scene_p scene, scene_load_fn cb);
        JOY_API void scene_set_update_callback(scene_p scene, scene_update_fn cb);
        JOY_API void scene_set_render_callback(scene_p scene, scene_render_fn cb);
        JOY_API void scene_set_destroy_callback(scene_p scene, scene_destroy_fn cb);
        JOY_API void scene_set_event_callback(scene_p scene, scene_event_fn cb);
        JOY_API void scene_set_popup_node(scene_p scene, scene_node_p node);
        JOY_API scene_node_p scene_get_popup_node(scene_p scene);
        JOY_API void scene_set_userdata(scene_p scene, void* ud);
        JOY_API void* scene_get_userdata(scene_p scene);
        JOY_API const char* scene_get_name(scene_p scene);
        JOY_API scene_manager_p scene_manager_create(void);
        JOY_API void scene_manager_destroy(scene_manager_p mgr);
        JOY_API void scene_manager_push(scene_manager_p mgr, scene_p scene);
        JOY_API void scene_manager_replace(scene_manager_p mgr, scene_p scene);
        JOY_API void scene_manager_pop(scene_manager_p mgr);
        JOY_API void scene_manager_update(scene_manager_p mgr, float delta_time);
        JOY_API void scene_manager_render(scene_manager_p mgr, const void* arg);
        JOY_API void scene_manager_handle_event(scene_manager_p mgr, const void* e);
        JOY_API scene_p scene_manager_get_current(scene_manager_p mgr);
        JOY_API int scene_manager_get_count(scene_manager_p mgr);


        JOY_API jparticle_cfg_t jparticle_cfg_default(void);
        JOY_API jparticle_p jparticle_create(jparticle_cfg_t cfg);
        JOY_API void jparticle_destroy(jparticle_p p);
        JOY_API void jparticle_set_position(jparticle_p p, float x, float y);
        JOY_API void jparticle_emit(jparticle_p p);            /* 爆发一次 */
        JOY_API void jparticle_clear(jparticle_p p);            /* 清除所有粒子 */
        JOY_API void jparticle_update(jparticle_p p, float dt);
        JOY_API void jparticle_render(jparticle_p p, jr2d_p r2d);
        JOY_API int  jparticle_count(jparticle_p p);
        JOY_API bool jparticle_is_active(jparticle_p p);


        JOY_API jlight_sys_p jlight_sys_create(int width, int height);
        JOY_API void jlight_sys_destroy(jlight_sys_p sys);
        JOY_API void jlight_sys_resize(jlight_sys_p sys, int width, int height);
        JOY_API int  jlight_add(jlight_sys_p sys, jlight_t light);
        JOY_API void jlight_remove(jlight_sys_p sys, int id);
        JOY_API void jlight_set(jlight_sys_p sys, int id, jlight_t light);
        JOY_API void jlight_clear(jlight_sys_p sys);
        JOY_API int jlight_add_occluder(jlight_sys_p sys, float x1, float y1, float x2, float y2);
        JOY_API void jlight_clear_occluders(jlight_sys_p sys);
        JOY_API void jlight_set_ambient(jlight_sys_p sys, uint8_t r, uint8_t g, uint8_t b);
        JOY_API void jlight_render(jlight_sys_p sys, jr2d_p r2d);



        JOY_API jecs_world_p jecs_create();
        JOY_API void jecs_destroy(jecs_world_p world);
        JOY_API void jecs_process(jecs_world_p world, float dt);
        JOY_API jecs_id_t jecs_spawn(jecs_world_p world);
        JOY_API void jecs_kill(jecs_world_p world, jecs_id_t id);
        JOY_API void jecs_define(jecs_world_p world, const char* name, size_t size);
        JOY_API void jecs_set(jecs_world_p world, jecs_id_t id, const char* name, const void* ptr, size_t size);
        JOY_API size_t jecs_get(jecs_world_p world, jecs_id_t id, const char* name, void** data);
        JOY_API void jecs_remove(jecs_world_p world, jecs_id_t id, const char* name);
        JOY_API jecs_id_array_t jecs_query(jecs_world_p world, int arg_cnt, ...);
        JOY_API jecs_id_array_t jecs_queryx(jecs_world_p world, int arg_cnt, char** args);
        JOY_API void jecs_register(jecs_world_p world, jecs_system_t system);
        JOY_API int jecs_test();


        /* ===== 运行与生命周期管理 ===== */
        bt_status_t bt_node_run(bt_node_t* node, float dt);
        void        bt_node_reset(bt_node_t* node);
        void        bt_node_destroy(bt_node_t* node);
        /* Action: 执行动作，返回 SUCCESS / FAILURE / RUNNING */
        bt_node_t* bt_action_create(bt_action_fn fn);
        bt_node_t* bt_action_createv(bt_action_fn fn, void* userdata);
        void bt_action_set_userdata(bt_node_t* node, void* userdata);
        void* bt_action_get_userdata(bt_node_t* node);
        /* Condition: 条件判断，true → SUCCESS, false → FAILURE */
        bt_node_t* bt_condition_create(bt_condition_fn pred);
        /* Wait: 等待指定秒数 */
        bt_node_t* bt_wait_create(float seconds);
        /* ===== 组合节点 ===== */
        /* Sequence: 顺序执行全部子节点，全部成功才返回 SUCCESS */
        bt_node_t* bt_sequence_create(bt_node_t** children, int count);
        /* Selector: 顺序尝试子节点，任一成功即返回 SUCCESS */
        bt_node_t* bt_selector_create(bt_node_t** children, int count);
        /* Parallel: 并行执行，success_count 个成功即成功 */
        bt_node_t* bt_parallel_create(bt_node_t** children, int count, int success_count);
        /* Inverter: 反转子节点结果 */
        bt_node_t* bt_inverter_create(bt_node_t* child);
        /* Succeeder: 总是返回 SUCCESS */
        bt_node_t* bt_succeeder_create(bt_node_t* child);
        /* Repeater: 重复子节点 times 次（-1 无限） */
        bt_node_t* bt_repeater_create(bt_node_t* child, int times);
        bt_node_t* bt_statemachine_create(bt_state_slot_t* states, int count, const char* initial);
        void       bt_statemachine_transition(bt_node_t* node, const char* state_name);


        JOY_API uint32_t audio_open_device();
        JOY_API void audio_close_device(uint32_t device_id);
        JOY_API bool audio_create_wav(const char* filepath, uint32_t device_id);
        JOY_API bool audio_create_mp3(const char* filepath, uint32_t device_id);
        JOY_API bool audio_create_flac(const char* filepath, uint32_t device_id);
        JOY_API bool audio_create_ogg(const char* filepath, uint32_t device_id);
        JOY_API audio_shot_p audio_shot_create(const char* wav_path, uint32_t device_id);
        JOY_API audio_shot_p audio_shot_create_mp3(const char* path, uint32_t device_id);
        JOY_API audio_shot_p audio_shot_create_flac(const char* path, uint32_t device_id);
        JOY_API audio_shot_p audio_shot_create_ogg(const char* path, uint32_t device_id);
        JOY_API void audio_shot_play(audio_shot_p shot);
        JOY_API void audio_shot_set_gain(audio_shot_p shot, float gain);
        JOY_API void audio_shot_destroy(audio_shot_p shot);
        JOY_API audio_bgm_p audio_bgm_create(const char* wav_path, uint32_t device_id);
        JOY_API audio_bgm_p audio_bgm_create_mp3(const char* path, uint32_t device_id);
        JOY_API audio_bgm_p audio_bgm_create_flac(const char* path, uint32_t device_id);
        JOY_API audio_bgm_p audio_bgm_create_ogg(const char* path, uint32_t device_id);
        JOY_API void audio_bgm_stop(audio_bgm_p bgm);
        JOY_API void audio_bgm_set_gain(audio_bgm_p bgm, float gain);
        JOY_API void audio_bgm_destroy(audio_bgm_p bgm);


        JOY_API modules_p modules_create(const char* path);
        JOY_API void modules_destroy(modules_p modules);
        JOY_API bool modules_query(modules_p modules, const char* name, module_p* mod);
        JOY_API void modules_register(modules_p modules, const char* name,
                module_create_t create,
                module_init_t init,
                module_event_t event_handler,
                module_update_t update,
                module_release_t release);
        JOY_API void modules_require(modules_p modules, const char* name);
        JOY_API bool moudles_retire(modules_p modules, const char* name, module_p* mod);
        JOY_API void* module_instance_create(module_p mod);
        JOY_API void module_instance_init(module_p mod, void* inst, void* ctx, const char* param);
        JOY_API void module_instance_event(module_p mod, void* inst, void* event);
        JOY_API void module_instance_update(module_p mod, void* inst, float delta_time);
        JOY_API void module_instance_release(module_p mod, void* inst);


        JOY_API box2d_world_t* box2d_create_world(vec2f_t gravity, int iterations);
        JOY_API void box2d_clear_world(box2d_world_t* world);
        JOY_API void box2d_destroy_world(box2d_world_t* world);
        JOY_API void box2d_step(box2d_world_t* world, fp_t dt);
        JOY_API box2d_body_t* box2d_create_body(box2d_world_t* world, fp_t mass, fp_t bx, fp_t by);
        JOY_API void box2d_destroy_body(box2d_world_t* world, int id);
        JOY_API box2d_joint_t* box2d_create_joint(box2d_world_t* world, int id1, int id2, vec2f_t anchor);
        JOY_API void box2d_destroy_joint(box2d_world_t* world, box2d_joint_t* joint);
        JOY_API void box2d_foreach_bodies(box2d_world_t* world, void (*iterator)(box2d_body_t*));
        JOY_API void box2d_foreach_joints(box2d_world_t* world, void (*iterator)(box2d_joint_t*, box2d_body_t*, box2d_body_t*));
        JOY_API void box2d_foreach_arbiters(box2d_world_t* world, void (*iterator)(box2d_arbiter_t*));

        JOY_API void jmsgpack_packer_init(jmsgpack_packer_t* pk, void* data, jmsgpack_write_fn cb);
        JOY_API int  jmsgpack_pack_int(jmsgpack_packer_t* pk, int d);
        JOY_API int  jmsgpack_pack_int64(jmsgpack_packer_t* pk, int64_t d);
        JOY_API int  jmsgpack_pack_uint(jmsgpack_packer_t* pk, unsigned int d);
        JOY_API int  jmsgpack_pack_uint64(jmsgpack_packer_t* pk, uint64_t d);
        JOY_API int  jmsgpack_pack_nil(jmsgpack_packer_t* pk);
        JOY_API int  jmsgpack_pack_array(jmsgpack_packer_t* pk, size_t n);
        JOY_API int  jmsgpack_pack_str(jmsgpack_packer_t* pk, size_t l);
        JOY_API int  jmsgpack_pack_str_body(jmsgpack_packer_t* pk, const void* b, size_t l);
        JOY_API void jmsgpack_unpacked_init(jmsgpack_unpacked_t* result);
        JOY_API void jmsgpack_unpacked_destroy(jmsgpack_unpacked_t* result);
        /* 返回值: >0 成功, 0 需要更多数据, <0 错误 */
        JOY_API int  jmsgpack_unpack_next(jmsgpack_unpacked_t* result, const char* data, size_t len, size_t* off);


        JOY_API luax_p luax_create(void);
        JOY_API bool luax_dofile(luax_p luax, const char* filename);
        JOY_API void luax_release(luax_p luax);
        void* luax_get_state(luax_p luax);
        JOY_API bool luax_reload(luax_p luax, const char* filename);
        JOY_API bool luax_call_joy(luax_p luax, const char* func, int argc, float dt);
        JOY_API void luax_dispatch_event(luax_p luax, const void* event);
        JOY_API void luax_set_watch(luax_p luax, const char* filename);
        JOY_API bool luax_poll_reload(luax_p luax);
        JOY_API const char* luax_get_watch(luax_p luax);
        JOY_API void luax_set_args(luax_p luax, int argc, char* argv[]);


        JOY_API jsx_p jsx_create(void);
        JOY_API bool jsx_dofile(jsx_p jsx, const char* filename);
        JOY_API void jsx_release(jsx_p jsx);
        JOY_API void jsx_dispatch_event(jsx_p jsx, const void* event);
        JOY_API bool jsx_call_joy(jsx_p jsx, const char* func, int argc, float dt);
        JOY_API bool jsx_reload(jsx_p jsx, const char* filename);
        JOY_API void jsx_set_watch(jsx_p jsx, const char* filename);
        JOY_API bool jsx_poll_reload(jsx_p jsx);
        JOY_API const char* jsx_get_watch(jsx_p jsx);
        JOY_API void jsx_set_args(jsx_p jsx, int argc, char* argv[]);

#ifdef ENABLED_CSHARP_SCRIPT
        JOY_API csx_p csx_create(void);
        JOY_API bool csx_load_assembly(csx_p csx, const char* filename);
        JOY_API MonoClass* csx_find_class(csx_p csx, const char* namespace_name, const char* class_name);
        JOY_API MonoObject* csx_invoke_static_method(csx_p csx, const char* namespace_name, const char* class_name, const char* method_name, void** args);
        JOY_API void csx_register_internal_call(const char* name, const void* method);
        JOY_API void csx_release(csx_p csx);
#endif // ENABLED_CSHARP_SCRIPT
#ifdef __cplusplus
}
#endif


#endif // !JOY_H