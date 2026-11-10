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
        typedef struct scrollview scrollview_t, * scrollview_p;


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
        typedef struct jtilemap jtilemap_t, * jtilemap_p;
        typedef struct jtilemap_tileset jtilemap_tileset_t, * jtilemap_tileset_p;
        typedef struct jtilemap_tile_layer jtilemap_tile_layer_t, * jtilemap_tile_layer_p;
        typedef struct jtilemap_object_layer jtilemap_object_layer_t, * jtilemap_object_layer_p;

        /* GID 解析结果：在此给出完整定义，使 C++ TU（模板 .cpp）也能看到完整类型，
           从而消除 joy_scene_tilemap_resolve_gid 的 -Wreturn-type-c-linkage 告警 */
        typedef struct jtilemap_gid_resolve {
                jtilemap_tileset_p tileset;
                int local_id;               /* tileset 内局部 ID */
                bool has_anim;
                int  anim_idx;              /* tileset->anims[] 下标，仅 has_anim 时有效 */
        } jtilemap_gid_resolve_t;

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
        typedef void (*scene_fixed_update_fn)(scene_p, float); /* 固定步长更新(Unity FixedUpdate 对应) */



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

        /* jmsgpack_object_t 为不透明句柄（opaque）。
         * 外部不再直接访问其字段，一律通过下方 jmsgpack_object_* 访问器读取；
         * 结构体定义与到 msgpack_object 的转换全部封装在 jmsgpack.c 内部。
         * 这样彻底消除"手写镜像 + 强转"带来的跨平台布局耦合（32/64 位尺寸差异静默出错）。
         * 历史的 bug：联盟体缺 ext 成员导致 wasm32 下 jmsgpack_object 比 msgpack_object 小 8 字节，
         * ptr[1] 按错误步进读取 → 嵌套解包失败（仅在 wasm 上暴露）。改成不透明后此类问题不可能再现。 */

        typedef struct {
                jmsgpack_zone_t* zone;
                const jmsgpack_object_t* data;   /* 指向 zone 内的根对象（不透明） */
        } jmsgpack_unpacked_t;

#ifdef __cplusplus
}
#endif

#endif /* JOY_TYPES_H */
