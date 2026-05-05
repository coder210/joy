/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jscene.h
Description: 场景管理 - 场景栈 + 节点树
Author: ydlc
Version: 1.0
Date: 2026.5.5
History:
************************************************/
#ifndef JSCENE_H
#define JSCENE_H
#include "jconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

        typedef struct node node_t, * node_p;
        typedef struct scene scene_t, * scene_p;
        typedef struct scene_manager scene_manager_t, * scene_manager_p;

        typedef void (*node_update_fn)(node_p, float);
        typedef void (*node_render_fn)(node_p);
        typedef void (*node_destroy_fn)(node_p);

        typedef void (*scene_load_fn)(scene_p);
        typedef void (*scene_update_fn)(scene_p, float);
        typedef void (*scene_render_fn)(scene_p);
        typedef void (*scene_destroy_fn)(scene_p);


        JOY_API node_p node_create(void);
        JOY_API void node_destroy(node_p node);

        JOY_API void node_add_child(node_p parent, node_p child);
        JOY_API void node_remove_child(node_p parent, node_p child);
        JOY_API node_p node_get_parent(node_p node);
        JOY_API node_p node_get_first_child(node_p node);
        JOY_API node_p node_get_next_sibling(node_p node);
        JOY_API int node_get_child_count(node_p node);

        JOY_API void node_set_position(node_p node, float x, float y);
        JOY_API void node_get_position(node_p node, float* out_x, float* out_y);
        JOY_API void node_set_world_position(node_p node, float x, float y);
        JOY_API void node_get_world_position(node_p node, float* out_x, float* out_y);
        JOY_API void node_set_rotation(node_p node, float radians);
        JOY_API float node_get_rotation(node_p node);
        JOY_API void node_set_scale(node_p node, float scale_x, float scale_y);
        JOY_API void node_get_scale(node_p node, float* out_scale_x, float* out_scale_y);
        JOY_API void node_set_zorder(node_p node, int zorder);
        JOY_API int node_get_zorder(node_p node);
        JOY_API void node_set_userdata(node_p node, void* userdata);
        JOY_API void* node_get_userdata(node_p node);

        JOY_API void node_set_update_callback(node_p node, node_update_fn cb);
        JOY_API void node_set_render_callback(node_p node, node_render_fn cb);
        JOY_API void node_set_destroy_callback(node_p node, node_destroy_fn cb);

        JOY_API void node_update(node_p node, float delta_time);
        JOY_API void node_render(node_p node);

        JOY_API scene_p scene_create(const char* name);
        JOY_API void scene_destroy(scene_p scene);

        JOY_API void scene_add_root_node(scene_p scene, node_p root);
        JOY_API void scene_remove_root_node(scene_p scene, node_p root);
        JOY_API int scene_get_root_count(scene_p scene);
        JOY_API node_p scene_get_root_at(scene_p scene, int index);

        JOY_API void scene_update(scene_p scene, float delta_time);
        JOY_API void scene_render(scene_p scene);

        JOY_API void scene_set_load_callback(scene_p scene, scene_load_fn cb);
        JOY_API void scene_set_update_callback(scene_p scene, scene_update_fn cb);
        JOY_API void scene_set_render_callback(scene_p scene, scene_render_fn cb);
        JOY_API void scene_set_destroy_callback(scene_p scene, scene_destroy_fn cb);
        JOY_API void scene_set_userdata(scene_p scene, void* userdata);
        JOY_API void* scene_get_userdata(scene_p scene);
        JOY_API const char* scene_get_name(scene_p scene);


        JOY_API scene_manager_p scene_manager_create(void);
        JOY_API void scene_manager_destroy(scene_manager_p mgr);

        JOY_API void scene_manager_push(scene_manager_p mgr, scene_p scene);
        JOY_API void scene_manager_replace(scene_manager_p mgr, scene_p scene);
        JOY_API void scene_manager_pop(scene_manager_p mgr);

        JOY_API void scene_manager_update(scene_manager_p mgr, float delta_time);
        JOY_API void scene_manager_render(scene_manager_p mgr);

        JOY_API scene_p scene_manager_get_current(scene_manager_p mgr);
        JOY_API int scene_manager_get_count(scene_manager_p mgr);

#ifdef __cplusplus
}
#endif

#endif // !JSCENE_H
