/******************************************************************************
 * src/joy_scene.h —— scene 域声明头（joy_scene.c 的配套头）
 * 职责：场景 / 动画 / tilemap / ECS / 过程地图 / 行为树
 * 方向：本头是该域声明的归属地，由 include/joy/japi.h 聚合包含；
 *       本头只依赖基础类型头，不再 include 聚合门面 joy.h，避免循环包含。
 * 生成链：本头是 gen_bindings.js 的 DECL_SOURCES 之一，改动声明后必须跑 --scan。
 ******************************************************************************/
#ifndef JOY_SCENE_IMPL_H
#define JOY_SCENE_IMPL_H

#include <joy/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

        JOY_API joy_animator_p joy_scene_animator_create(joy_r2d_p r2d);
        JOY_API void joy_scene_animator_destroy(joy_animator_p anim);
        JOY_API void joy_scene_animator_set_float(joy_animator_p anim, const char* name, float v);
        JOY_API void joy_scene_animator_set_int(joy_animator_p anim, const char* name, int v);
        JOY_API void joy_scene_animator_set_bool(joy_animator_p anim, const char* name, bool v);
        JOY_API void joy_scene_animator_set_trigger(joy_animator_p anim, const char* name);
        JOY_API float joy_scene_animator_get_float(joy_animator_p anim, const char* name);
        JOY_API int joy_scene_animator_get_int(joy_animator_p anim, const char* name);
        JOY_API bool joy_scene_animator_get_bool(joy_animator_p anim, const char* name);
        JOY_API void joy_scene_animator_add_state(joy_animator_p anim, const char* name);
        JOY_API void joy_scene_animator_set_default_state(joy_animator_p anim, const char* name);
        JOY_API void joy_scene_animator_state_add_clip(joy_animator_p anim, const char* state_name,
                const char* image_path, float duration, joy_rect_t src_rect);
        JOY_API void joy_scene_animator_state_add_color(joy_animator_p anim, const char* state_name,
                float duration, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        JOY_API void joy_scene_animator_state_set_loop(joy_animator_p anim, const char* state_name, bool loop);
        JOY_API void joy_scene_animator_state_set_speed(joy_animator_p anim, const char* state_name, float speed);
        JOY_API void joy_scene_animator_state_set_blend(joy_animator_p anim, const char* state_name,
                const char* param_name);
        JOY_API void joy_scene_animator_state_add_motion(joy_animator_p anim, const char* state_name, const char* image_path, float duration, joy_rect_t src_rect, float threshold);
        JOY_API int joy_scene_animator_add_transition(joy_animator_p anim, const char* from, const char* to);
        JOY_API int joy_scene_animator_add_any_transition(joy_animator_p anim, const char* to);
        JOY_API void joy_scene_animator_transition_add_cond(joy_animator_p anim, int trans_idx,
                const char* param_name, int mode, float threshold);
        JOY_API void joy_scene_animator_transition_set_exit_time(joy_animator_p anim, int trans_idx, float seconds);
        JOY_API void joy_scene_animator_update(joy_animator_p anim, float dt);
        JOY_API void joy_scene_animator_draw(joy_animator_p anim, float x, float y);
        JOY_API void joy_scene_animator_set_flip_h(joy_animator_p anim, bool flip);
        JOY_API const char* joy_scene_animator_current_state(joy_animator_p anim);
        JOY_API void joy_scene_animator_set_on_enter(joy_animator_p anim, joy_anim_state_cb cb, void* userdata);
        JOY_API void joy_scene_animator_set_on_exit(joy_animator_p anim, joy_anim_state_cb cb, void* userdata);

        JOY_API joy_tilemap_p joy_scene_tilemap_load_file_json(const char* filename);
        JOY_API void joy_scene_tilemap_destroy(joy_tilemap_p tm);
        JOY_API int joy_scene_tilemap_get_width(const joy_tilemap_p tm);
        JOY_API int joy_scene_tilemap_get_height(const joy_tilemap_p tm);
        JOY_API int joy_scene_tilemap_get_tilewidth(const joy_tilemap_p tm);
        JOY_API int joy_scene_tilemap_get_tileheight(const joy_tilemap_p tm);
        JOY_API joy_tilemap_gid_resolve_t joy_scene_tilemap_resolve_gid(const joy_tilemap_p tm, int gid);
        JOY_API const char* joy_scene_tilemap_get_tile_image(const joy_tilemap_tileset_p ts, int local_id);
        JOY_API bool joy_scene_tilemap_get_tile_def_size(const joy_tilemap_tileset_p ts, int local_id, int* out_w, int* out_h);
        JOY_API bool joy_scene_tilemap_get_tile_uv(const joy_tilemap_p tm, int gid,
                uint32_t time_ms, float out_rect[4]);
        JOY_API int joy_scene_tilemap_get_anim_tileid(const joy_tilemap_p tm, int gid,
                uint32_t time_ms);
        JOY_API joy_tilemap_tile_layer_p joy_scene_tilemap_find_tile_layer(const joy_tilemap_p tm,
                const char* name);
        JOY_API joy_tilemap_object_layer_p joy_scene_tilemap_find_object_layer(const joy_tilemap_p tm, const char* name);
        JOY_API void joy_scene_tilemap_object_layer_foreach(joy_tilemap_object_layer_p ol, void (*callback)(joy_tilemap_object_p obj, void*arg), void* arg);
        JOY_API void joy_scene_tilemap_object_layer_foreach_all(joy_tilemap_object_layer_p ol, void (*callback)(joy_tilemap_object_p obj, void*arg), void* arg);
        JOY_API int joy_scene_tilemap_get_tile(const joy_tilemap_tile_layer_p layer,
                int tx, int ty);
        JOY_API joy_tilemap_render_p joy_scene_tilemap_render_create(joy_r2d_p renderer,
                const char* asset_base_path,
                joy_tilemap_path_remap_t remap);
        JOY_API void joy_scene_tilemap_render_all(joy_tilemap_render_p r,
                joy_tilemap_p tm,
                float camera_x, float camera_y,
                uint32_t time_ms);
        JOY_API void joy_scene_tilemap_render_tiles(joy_tilemap_render_p r,
                joy_tilemap_p tm,
                float camera_x, float camera_y,
                uint32_t time_ms);
        JOY_API void joy_scene_tilemap_render_set_tile_px(joy_tilemap_render_p r, float px);
        JOY_API bool joy_scene_tilemap_render_layer(joy_tilemap_render_p r,
                joy_tilemap_p tm,
                const char* layer_name,
                float camera_x, float camera_y,
                uint32_t time_ms);
        JOY_API void joy_scene_tilemap_render_destroy(joy_tilemap_render_p r);


        JOY_API joy_scene_node_p joy_scene_node_create(void);
        JOY_API void joy_scene_node_destroy(joy_scene_node_p node);
        JOY_API void joy_scene_node_add_child(joy_scene_node_p parent, joy_scene_node_p child);
        JOY_API void joy_scene_node_remove_child(joy_scene_node_p parent, joy_scene_node_p child);
        JOY_API joy_scene_node_p joy_scene_node_get_parent(joy_scene_node_p node);
        JOY_API joy_scene_node_p joy_scene_node_get_first_child(joy_scene_node_p node);
        JOY_API joy_scene_node_p joy_scene_node_get_next_sibling(joy_scene_node_p node);
        JOY_API int joy_scene_node_get_child_count(joy_scene_node_p node);
        JOY_API void joy_scene_node_set_position(joy_scene_node_p node, float x, float y);
        JOY_API void joy_scene_node_get_position(joy_scene_node_p node, float* out_x, float* out_y);
        JOY_API void joy_scene_node_set_world_position(joy_scene_node_p node, float x, float y);
        JOY_API void joy_scene_node_get_world_position(joy_scene_node_p node, float* out_x, float* out_y);
        JOY_API void joy_scene_node_set_rotation(joy_scene_node_p node, float radians);
        JOY_API float joy_scene_node_get_rotation(joy_scene_node_p node);
        JOY_API void joy_scene_node_set_scale(joy_scene_node_p node, float scale_x, float scale_y);
        JOY_API void joy_scene_node_get_scale(joy_scene_node_p node, float* out_scale_x, float* out_scale_y);
        JOY_API void joy_scene_node_set_zorder(joy_scene_node_p node, int zorder);
        JOY_API int joy_scene_node_get_zorder(joy_scene_node_p node);
        JOY_API void joy_scene_node_set_userdata(joy_scene_node_p node, void* userdata);
        JOY_API void* joy_scene_node_get_userdata(joy_scene_node_p node);
        JOY_API void joy_scene_node_set_name(joy_scene_node_p node, const char* name);
        JOY_API const char* joy_scene_node_get_name(joy_scene_node_p node);
        JOY_API void joy_scene_node_set_tag(joy_scene_node_p node, const char* tag);
        JOY_API const char* joy_scene_node_get_tag(joy_scene_node_p node);
        JOY_API joy_scene_node_p joy_scene_find_node_by_name(joy_scene_node_p root, const char* name);
        JOY_API joy_scene_node_p joy_scene_find_node_by_name_deep(joy_scene_p scene, const char* name);
        JOY_API int  joy_scene_find_nodes_by_tag(joy_scene_node_p root, const char* tag, joy_scene_node_p* out, int max);
        JOY_API int  joy_scene_find_nodes_by_tag_deep(joy_scene_p scene, const char* tag, joy_scene_node_p* out, int max);
        JOY_API void joy_scene_node_set_update_callback(joy_scene_node_p node, joy_scene_node_update_fn cb);
        JOY_API void joy_scene_node_set_render_callback(joy_scene_node_p node, joy_scene_node_render_fn cb);
        JOY_API void joy_scene_node_set_destroy_callback(joy_scene_node_p node, joy_scene_node_destroy_fn cb);
        JOY_API void joy_scene_node_set_event_callback(joy_scene_node_p node, joy_scene_node_event_fn cb);
        JOY_API void joy_scene_node_handle_event(joy_scene_node_p node, const void* e);
        JOY_API void joy_scene_node_update(joy_scene_node_p node, float delta_time);
        JOY_API void joy_scene_node_render(joy_scene_node_p node, const void* arg);
        JOY_API joy_scene_p joy_scene_create(const char* name);
        JOY_API void joy_scene_destroy(joy_scene_p scene);
        JOY_API void joy_scene_add_root_node(joy_scene_p scene, joy_scene_node_p root);
        JOY_API void joy_scene_remove_root_node(joy_scene_p scene, joy_scene_node_p root);
        JOY_API int joy_scene_get_root_count(joy_scene_p scene);
        JOY_API joy_scene_node_p joy_scene_get_root_at(joy_scene_p scene, int index);
        JOY_API void joy_scene_update(joy_scene_p scene, float delta_time);
        JOY_API void joy_scene_render(joy_scene_p scene, const void* arg);
        JOY_API void joy_scene_handle_event(joy_scene_p scene, const void* e);
        JOY_API void joy_scene_set_load_callback(joy_scene_p scene, joy_scene_load_fn cb);
        JOY_API void joy_scene_set_update_callback(joy_scene_p scene, joy_scene_update_fn cb);
        JOY_API void joy_scene_set_render_callback(joy_scene_p scene, joy_scene_render_fn cb);
        JOY_API void joy_scene_set_destroy_callback(joy_scene_p scene, joy_scene_destroy_fn cb);
        JOY_API void joy_scene_set_event_callback(joy_scene_p scene, joy_scene_event_fn cb);
        JOY_API void joy_scene_set_fixed_update_callback(joy_scene_p scene, joy_scene_fixed_update_fn cb, float fixed_step);
        JOY_API void joy_scene_set_popup_node(joy_scene_p scene, joy_scene_node_p node);
        JOY_API joy_scene_node_p joy_scene_get_popup_node(joy_scene_p scene);
        JOY_API void joy_scene_set_userdata(joy_scene_p scene, void* ud);
        JOY_API void* joy_scene_get_userdata(joy_scene_p scene);
        JOY_API const char* joy_scene_get_name(joy_scene_p scene);
        JOY_API joy_scene_manager_p joy_scene_manager_create(void);
        JOY_API void joy_scene_manager_destroy(joy_scene_manager_p mgr);
        JOY_API void joy_scene_manager_push(joy_scene_manager_p mgr, joy_scene_p scene);
        JOY_API void joy_scene_manager_replace(joy_scene_manager_p mgr, joy_scene_p scene);
        JOY_API void joy_scene_manager_pop(joy_scene_manager_p mgr);
        JOY_API void joy_scene_manager_update(joy_scene_manager_p mgr, float delta_time);
        JOY_API void joy_scene_manager_render(joy_scene_manager_p mgr, const void* arg);
        JOY_API void joy_scene_manager_handle_event(joy_scene_manager_p mgr, const void* e);
        JOY_API joy_scene_p joy_scene_manager_get_current(joy_scene_manager_p mgr);
        JOY_API int joy_scene_manager_get_count(joy_scene_manager_p mgr);



        JOY_API joy_ecs_world_p joy_scene_ecs_create();
        JOY_API void joy_scene_ecs_destroy(joy_ecs_world_p world);
        JOY_API void joy_scene_ecs_process(joy_ecs_world_p world, float dt);
        JOY_API joy_ecs_id_t joy_scene_ecs_spawn(joy_ecs_world_p world);
        JOY_API void joy_scene_ecs_kill(joy_ecs_world_p world, joy_ecs_id_t id);
        JOY_API void joy_scene_ecs_define(joy_ecs_world_p world, const char* name, size_t size);
        JOY_API void joy_scene_ecs_set(joy_ecs_world_p world, joy_ecs_id_t id, const char* name, const void* ptr, size_t size);
        JOY_API size_t joy_scene_ecs_get(joy_ecs_world_p world, joy_ecs_id_t id, const char* name, void** data);
        JOY_API void joy_scene_ecs_remove(joy_ecs_world_p world, joy_ecs_id_t id, const char* name);
        JOY_API joy_ecs_id_array_t joy_scene_ecs_query(joy_ecs_world_p world, int arg_cnt, ...);
        JOY_API joy_ecs_id_array_t joy_scene_ecs_queryx(joy_ecs_world_p world, int arg_cnt, char** args);
        JOY_API void joy_scene_ecs_register(joy_ecs_world_p world, joy_ecs_system_t system);
        JOY_API int joy_scene_ecs_test();

#ifdef __cplusplus
}
#endif

#endif /* JOY_SCENE_IMPL_H */
