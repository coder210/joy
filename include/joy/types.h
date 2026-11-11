/* 公共类型：结构体布局、回调类型与不透明句柄 typedef
 * 由 joy.h 按行段拆出，聚合入口见 joy.h；改动请在对应分域头进行。 */
#ifndef JOY_TYPES_H
#define JOY_TYPES_H

#include <joy/defs.h>

#ifdef __cplusplus
extern "C" {
#endif

        /* ---------- MVP 结构（无 SDL 依赖） ---------- */
        typedef struct render_mvp {
                float viewProj[16];
                float model[16];
        } joy_render_mvp_t;

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
        } joy_r3d_shader_light_t;       /* 64 字节 */
        typedef struct jr3d_light_block {
                joy_r3d_shader_light_t lights[JR3D_MAX_LIGHTS];
                float ambient[4];
                float viewPos[4];
        } joy_r3d_light_block_t;        /* 288 字节 */


        typedef struct ray2df {
                joy_vec2f_t origin, direction;
        }joy_ray2df_t;

        typedef struct ray2d_collisionf {
                bool hit;               // Did the ray hit something?
                joy_fp_t distance;         // Distance to the nearest hit
                joy_vec2f_t point;          // Point of the nearest hit
                joy_vec2f_t normal;         // Surface normal of hit
        }joy_ray2d_collisionf_t;


        /* 锚点约定: 矩形用"左上角(x,y) + 宽高"定位, 区别于 GJK BOX 的"中心 + 半边长" */
        typedef struct rectanglef {
                joy_fp_t x; // rectangle top-left corner position x
                joy_fp_t y; // rectangle top-left corner position y
                joy_fp_t width;
                joy_fp_t height;
        }joy_rectanglef_t;

        typedef struct circlef {
                joy_vec2f_t center;
                joy_fp_t radius;
        }joy_circlef_t;

        typedef struct polygonf {
                joy_vec2f_t* vertices;
                int num_vertices;
        }joy_polygonf_t;


        typedef struct contact2df {
                joy_vec2f_t sp; // starting point
                joy_vec2f_t ep; // ending point
                joy_vec2f_t normal;
                joy_fp_t depth;
        }joy_contact2df_t;


        typedef struct ray3df {
                joy_vec3f_t position, direction;
        }joy_ray3df_t;

        typedef struct ray3d_collisionf {
                bool hit;               // Did the ray hit something?
                joy_fp_t distance;         // Distance to the nearest hit
                joy_vec3f_t point;          // Point of the nearest hit
                joy_vec3f_t normal;         // Surface normal of hit
        }joy_ray3d_collisionf_t;

        typedef struct spheref {
                joy_vec3f_t position;
                joy_fp_t radius;
        }joy_spheref_t;

        typedef struct bounding_boxf {
                joy_vec3f_t min;            // Minimum vertex box-corner
                joy_vec3f_t max;            // Maximum vertex box-corner
        } joy_bounding_boxf_t;

        typedef struct ray2d {
                joy_vec2_t origin, direction;
        }joy_ray2d_t;

        typedef struct ray2d_collision {
                bool hit;
                joy_ft_t distance;
                joy_vec2_t point;
                joy_vec2_t normal;
        }joy_ray2d_collision_t;

        typedef struct rectangle {
                joy_ft_t x, y, width, height;
        }joy_rectangle_t;

        typedef struct circle {
                joy_vec2_t center;
                joy_ft_t radius;
        }joy_circle_t;

        typedef struct polygon {
                joy_vec2_t* vertices;
                int num_vertices;
        }joy_polygon_t;

        typedef struct contact2d {
                joy_vec2_t sp;
                joy_vec2_t ep;
                joy_vec2_t normal;
                joy_ft_t depth;
        }joy_contact2d_t;

        typedef struct ray3d {
                joy_vec3_t position, direction;
        }joy_ray3d_t;

        typedef struct ray3d_collision {
                bool hit;
                joy_ft_t distance;
                joy_vec3_t point;
                joy_vec3_t normal;
        }joy_ray3d_collision_t;

        typedef struct sphere {
                joy_vec3_t position;
                joy_ft_t radius;
        }joy_sphere_t;

        typedef struct bounding_box {
                joy_vec3_t min;
                joy_vec3_t max;
        } joy_bounding_box_t;




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
        /* 注意: BOX/OBB 的 position 是中心(half_size 为半边长), 与 joy_rectanglef_t 的"左上角+宽高"语义不同 */
        typedef struct gjk3df_collider {
                gjk3df_ctype_k type;
                joy_vec3f_t position;      /* 中心位置 */
                joy_fp_t radius;           /* 球体/胶囊/圆柱/圆锥半径 */
                joy_vec3f_t half_size;     /* BOX/OBB半边长 */
                joy_fp_t height;           /* 胶囊/圆柱/圆锥高度 */
                joy_vec3f_t axis;          /* 胶囊朝向(单位向量) */
                joy_vec3f_t rotation;      /* OBB欧拉角(绕 X/Y/Z 弧度) */
                joy_vec3f_t* vertices;     /* MESH顶点数组(借用的指针,不负责释放) */
                int vertices_num;
        } joy_gjk3df_collider_t, * joy_gjk3df_collider_p;

        /* ---------- 接触信息(含EPA穿透深度) ---------- */
        typedef struct gjk3df_contact {
                joy_vec3f_t normal;
                joy_fp_t depth;
        } joy_gjk3df_contact_t, * joy_gjk3df_contact_p;

        /* ======================================================================== */
        /*                        GJK 3D 浮点版 (joy_ft_t/joy_vec3_t)                        */
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
                joy_vec3_t position;       /* 中心位置 */
                joy_ft_t radius;           /* 球体/胶囊/圆柱/圆锥半径 */
                joy_vec3_t half_size;      /* BOX半边长 */
                joy_ft_t height;           /* 胶囊/圆柱/圆锥高度 */
                joy_vec3_t axis;           /* 胶囊朝向(单位向量) */
                joy_vec3_t* vertices;      /* MESH顶点数组(借用的指针,不负责释放) */
                int vertices_num;
        } joy_gjk3d_collider_t, * joy_gjk3d_collider_p;

        /* ---------- 接触信息(含EPA穿透深度) ---------- */
        typedef struct gjk3d_contact {
                joy_vec3_t normal;
                joy_ft_t depth;
        } joy_gjk3d_contact_t, * joy_gjk3d_contact_p;

      

        /* ======================================================================== */
        /*                             GJK 2D 碰撞检测                                */
        /* ======================================================================== */

        typedef uint8_t gjk2d_ctype_k;
        #define GJK2D_CTYPE_CIRCLE   0  /* 圆: position + radius */
        #define GJK2D_CTYPE_BOX      1  /* AABB盒: position + half_size */
        #define GJK2D_CTYPE_CAPSULE  2  /* 胶囊: position(中心) + radius + height + axis */
        #define GJK2D_CTYPE_POLYGON  3  /* 凸多边形: vertices数组(预变换到世界坐标) */
        #define GJK2D_CTYPE_PIE      4  /* 扇形: position + radius + sweep(弧度) */

        /* 注意: BOX 的 position 是中心(half_size 为半边长), 与 joy_rectanglef_t 的"左上角+宽高"语义不同 */
        typedef struct gjk2d_collider {
                gjk2d_ctype_k type;
                joy_vec2f_t position;       /* 中心位置 */
                joy_fp_t radius;            /* 圆/胶囊/扇形半径 */
                joy_vec2f_t half_size;      /* BOX半边长 */
                joy_fp_t height;            /* 胶囊线段长度 */
                joy_vec2f_t axis;           /* 胶囊朝向(单位向量) */
                joy_vec2f_t* vertices;      /* POLYGON顶点数组(借用的指针,不释放) */
                int vertices_num;
                joy_fp_t sweep;             /* 扇形张角(弧度) */
        } joy_gjk2d_collider_t, * joy_gjk2d_collider_p;

        typedef struct gjk2d_contact {
                joy_vec2f_t normal;
                joy_fp_t depth;
        } joy_gjk2d_contact_t, * joy_gjk2d_contact_p;




        /* ========== Types ========== */
        typedef struct janimator joy_animator_t, * joy_animator_p;

        /* Parameter types */
        typedef enum {
                JANIM_PARAM_FLOAT,
                JANIM_PARAM_INT,
                JANIM_PARAM_BOOL,
                JANIM_PARAM_TRIGGER,
        } joy_anim_param_type_t;

        /* Condition modes */
        typedef enum {
                JANIM_COND_GREATER,     /* param > threshold */
                JANIM_COND_LESS,        /* param < threshold */
                JANIM_COND_EQUALS,      /* param == threshold (float with epsilon) */
                JANIM_COND_NOT_EQUALS,  /* param != threshold */
                JANIM_COND_IF_TRUE,     /* bool param == true */
                JANIM_COND_IF_FALSE,    /* bool param == false */
                JANIM_COND_TRIGGER,     /* trigger was set */
        } joy_anim_cond_mode_t;

        /* State enter/exit callback */
        typedef void (*janim_state_cb)(const char* state_name, void* userdata);

        typedef struct label joy_label_t, * joy_label_p;
        typedef struct button joy_button_t, * joy_button_p;
        typedef struct combobox_item joy_combobox_item_t, * joy_combobox_item_p;
        typedef struct combobox joy_combobox_t, * joy_combobox_p;
        typedef struct combobox_input joy_combobox_input_t, * joy_combobox_input_p;
        typedef struct datagrid joy_datagrid_t, * joy_datagrid_p;
        typedef struct radio joy_radio_t, * joy_radio_p;
        typedef enum checkbox_state {
                JOY_CHECKBOX_UNCHECKED,
                JOY_CHECKBOX_CHECKED,
                JOY_CHECKBOX_INDETERMINATE
        }joy_checkbox_state_t;
        typedef struct checkbox joy_checkbox_t, * joy_checkbox_p;
        typedef struct joystick joy_joystick_t, * joy_joystick_p;
        typedef struct inputbox joy_inputbox_t, * joy_inputbox_p;
        typedef struct slider joy_slider_t, * joy_slider_p;
        typedef struct popup joy_popup_t, * joy_popup_p;
        typedef struct tooltip joy_tooltip_t, * joy_tooltip_p;
        typedef struct scrollview joy_scrollview_t, * joy_scrollview_p;


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
        }joy_tilemap_object_t, * joy_tilemap_object_p;
        typedef struct jtilemap joy_tilemap_t, * joy_tilemap_p;
        typedef struct jtilemap_tileset joy_tilemap_tileset_t, * joy_tilemap_tileset_p;
        typedef struct jtilemap_tile_layer joy_tilemap_tile_layer_t, * joy_tilemap_tile_layer_p;
        typedef struct jtilemap_object_layer joy_tilemap_object_layer_t, * joy_tilemap_object_layer_p;

        /* GID 解析结果：在此给出完整定义，使 C++ TU（模板 .cpp）也能看到完整类型，
           从而消除 joy_scene_tilemap_resolve_gid 的 -Wreturn-type-c-linkage 告警 */
        typedef struct jtilemap_gid_resolve {
                joy_tilemap_tileset_p tileset;
                int local_id;               /* tileset 内局部 ID */
                bool has_anim;
                int  anim_idx;              /* tileset->anims[] 下标，仅 has_anim 时有效 */
        } joy_tilemap_gid_resolve_t;

        /* ---------- 路径映射回调 ---------- */
        typedef const char* (*joy_tilemap_path_remap_t)(const char* original_path);
        /* ---------- 渲染实例（不透明句柄，可重入） ---------- */
        typedef struct joy_tilemap_render_t joy_tilemap_render_t, * joy_tilemap_render_p;


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
        }joy_net_message_t, * joy_net_message_p;

        typedef void (*net_callback)(joy_net_message_p msg, void* userdata);
        typedef struct kcpserver joy_kcpserver_t, * joy_kcpserver_p;
        typedef struct kcpclient joy_kcpclient_t, * joy_kcpclient_p;
        typedef struct tcpserver joy_tcpserver_t, * joy_tcpserver_p;
        typedef struct tcpclient joy_tcpclient_t, * joy_tcpclient_p;
        typedef struct wsnetserver joy_wsnetserver_t, * joy_wsnetserver_p;
        typedef struct wsnetclient joy_wsnetclient_t, * joy_wsnetclient_p;
        typedef enum {
                NET_CLIENT_TCP,
                NET_CLIENT_KCP,
                NET_CLIENT_WEBSOCKET, // WebSocket: emscripten用原生, 非emscripten用mongoose
        } net_client_type;
        typedef struct netclient joy_netclient_t, * joy_netclient_p;
        typedef enum {
                NET_SERVER_KCP,
                NET_SERVER_TCP,
                NET_SERVER_WEBSOCKET,
        } net_server_type;
        typedef struct netserver joy_netserver_t, * joy_netserver_p;


        typedef struct scene_node joy_scene_node_t, * joy_scene_node_p;
        typedef struct scene joy_scene_t, * joy_scene_p;
        typedef struct scene_manager joy_scene_manager_t, * joy_scene_manager_p;

        typedef void (*scene_node_update_fn)(joy_scene_node_p, float);
        typedef void (*scene_node_render_fn)(joy_scene_node_p, const void*);
        typedef void (*scene_node_destroy_fn)(joy_scene_node_p);
        typedef void (*scene_node_event_fn)(joy_scene_node_p, const void*);

        typedef void (*scene_load_fn)(joy_scene_p);
        typedef void (*scene_update_fn)(joy_scene_p, float);
        typedef void (*scene_render_fn)(joy_scene_p);
        typedef void (*scene_destroy_fn)(joy_scene_p);
        typedef void (*scene_event_fn)(joy_scene_p, const void*);
        typedef void (*scene_fixed_update_fn)(joy_scene_p, float); /* 固定步长更新(Unity FixedUpdate 对应) */



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
        } joy_particle_cfg_t;

        typedef struct jparticle joy_particle_t, * joy_particle_p;

        /* ---------- 光源 ---------- */
        typedef struct jlight {
                float x, y;          /* 位置(像素坐标) */
                float radius;        /* 光照半径 */
                float r, g, b;       /* 颜色(0-255) */
                float intensity;     /* 强度 0~1 */
        } joy_light_t;

        typedef struct jlight_sys joy_light_sys_t, * joy_light_sys_p;


        typedef int joy_ecs_id_t;
        typedef struct jecs_world joy_ecs_world_t, * joy_ecs_world_p;
        //VECTOR_INIT(kecs, joy_ecs_id_t);

        typedef struct jecs_id_array {
                joy_ecs_id_t ids[512];
                int num;
        }joy_ecs_id_array_t, * joy_ecs_id_array_p;

        typedef struct jecs_system {
                void* arg;
                void (*cb)(joy_ecs_world_p, void*, float);
        } joy_ecs_system_t, * joy_ecs_system_p;


        /* ===== 行为树状态 ===== */
        typedef enum {
                BT_SUCCESS,
                BT_FAILURE,
                BT_RUNNING,
        } joy_bt_status_t;

        /* ===== 前向声明 ===== */
        typedef struct bt_node joy_bt_node_t;

        /* ===== 用户回调函数类型 ===== */
        typedef joy_bt_status_t(*bt_action_fn)(float dt);
        typedef bool        (*bt_condition_fn)(void);

        /* ===== 节点生命周期回调 ===== */
        typedef void     (*bt_enter_fn)(joy_bt_node_t* self);
        typedef joy_bt_status_t(*bt_tick_fn)(joy_bt_node_t* self, float dt);
        typedef void     (*bt_exit_fn)(joy_bt_node_t* self);
        typedef void     (*bt_destroy_fn)(joy_bt_node_t* self);

        /* ===== 基础节点结构 ===== */
        struct bt_node {
                joy_bt_status_t   status;      /* 当前运行状态 */
                bt_enter_fn   enter;       /* 进入回调 */
                bt_tick_fn    tick;        /* 更新回调 */
                bt_exit_fn    exit;        /* 退出回调 */
                bt_destroy_fn destroy;     /* 销毁回调 */
        };

        /* ===== 内嵌状态机节点 ===== */
        typedef struct bt_state_slot {
                const char* name;                    /* 状态名称（不可为 NULL） */
                bt_enter_fn  state_enter;             /* 状态 enter（可为 NULL） */
                const char* (*state_update)(joy_bt_node_t* node, float dt); /* 返回下一状态名，NULL 表示不切换 */
                bt_exit_fn   state_exit;              /* 状态 exit（可为 NULL） */
        } joy_bt_state_slot_t;



        typedef struct audio_shot joy_audio_shot_t, * joy_audio_shot_p;
        typedef struct audio_bgm joy_audio_bgm_t, * joy_audio_bgm_p;


        typedef void* (*joy_module_create_t)(void);
        typedef int (*joy_module_init_t)(void* inst, void* app, const char* param);
        typedef int (*joy_module_event_t)(void* inst, void* event);
        typedef int (*joy_module_update_t)(void* inst, float delta_time);
        typedef void (*joy_module_release_t)(void* inst);

        typedef struct module {
                char name[512];
                void* handle;
                joy_module_create_t create;
                joy_module_init_t init;
                joy_module_event_t event_handler;
                joy_module_update_t update;
                joy_module_release_t release;
        }joy_module_t, * joy_module_p;

        typedef struct modules joy_modules_t, * joy_modules_p;




        typedef struct joy_box2d_body_t {
                int id;
                joy_vec2f_t position;
                joy_fp_t rotation;
                joy_vec2f_t velocity;
                joy_fp_t angular_velocity;
                joy_vec2f_t force;
                joy_fp_t torque;
                joy_vec2f_t bounds;
                joy_fp_t friction;
                joy_fp_t mass, inv_mass;
                joy_fp_t I, invI;
        } joy_box2d_body_t;


        typedef struct joy_box2d_joint_t {
                joy_mat22f_t M;
                joy_vec2f_t local_anchor1, local_anchor2;
                joy_vec2f_t r1, r2;
                joy_vec2f_t bias;
                joy_vec2f_t p;		// accumulated impulse
                int id1, id2;
                joy_fp_t biasfactor;
                joy_fp_t softness;
        } joy_box2d_joint_t;


        typedef struct joy_box2d_manifold_t {
                joy_vec2f_t position;
                joy_vec2f_t normal;
                joy_fp_t separation;
                int feature;
                joy_fp_t pn;	// accumulated normal impulse
                joy_fp_t pt;	// accumulated tangent impulse
                joy_vec2f_t r1, r2;
                joy_fp_t mass_normal, mass_tangent;
                joy_fp_t bias;
        } joy_box2d_manifold_t;


        typedef union joy_box2d_arbiter_key_t {
                struct id_pair {
                        int id1, id2;
                } id_pair;
                int64_t value;
        } joy_box2d_arbiter_key_t;


        typedef struct joy_box2d_arbiter_t {
                joy_box2d_manifold_t manifolds[JOY_MAX_POINTS];
                int num_manifolds, id1, id2;
                joy_fp_t friction; // combined friction
        } joy_box2d_arbiter_t;


        typedef struct joy_box2d_world_t joy_box2d_world_t;



        /* ===== 装箱回调类型 ===== */
        typedef int (*jmsgpack_write_fn)(void* data, const char* buf, size_t len);

        /* ===== 类型别名 ===== */
        typedef struct jmsgpack_packer {
                void* data;
                jmsgpack_write_fn callback;
        } joy_msgpack_packer_t;

        typedef struct jmsgpack_zone joy_msgpack_zone_t;
        typedef struct jmsgpack_object joy_msgpack_object_t;

        typedef enum {
                JMSGPACK_OBJECT_NIL = 0x00,
                JMSGPACK_OBJECT_BOOLEAN = 0x01,
                JMSGPACK_OBJECT_POSITIVE_INTEGER = 0x02,
                JMSGPACK_OBJECT_NEGATIVE_INTEGER = 0x03,
                JMSGPACK_OBJECT_FLOAT64 = 0x04,
                JMSGPACK_OBJECT_STR = 0x05,
                JMSGPACK_OBJECT_ARRAY = 0x06,
                JMSGPACK_OBJECT_MAP = 0x07,
        } joy_msgpack_object_type_t;

        /* joy_msgpack_object_t 为不透明句柄（opaque）。
         * 外部不再直接访问其字段，一律通过下方 jmsgpack_object_* 访问器读取；
         * 结构体定义与到 msgpack_object 的转换全部封装在 jmsgpack.c 内部。
         * 这样彻底消除"手写镜像 + 强转"带来的跨平台布局耦合（32/64 位尺寸差异静默出错）。
         * 历史的 bug：联盟体缺 ext 成员导致 wasm32 下 jmsgpack_object 比 msgpack_object 小 8 字节，
         * ptr[1] 按错误步进读取 → 嵌套解包失败（仅在 wasm 上暴露）。改成不透明后此类问题不可能再现。 */

        typedef struct {
                joy_msgpack_zone_t* zone;
                const joy_msgpack_object_t* data;   /* 指向 zone 内的根对象（不透明） */
        } joy_msgpack_unpacked_t;

#ifdef __cplusplus
}
#endif

#endif /* JOY_TYPES_H */
