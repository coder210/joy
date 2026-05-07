/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jscene.c
Description: 场景管理实现
Author: ydlc
Version: 1.0
Date: 2026.5.5
History:
************************************************/
#include "jscene.h"
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>

// ============================================================
// 节点结构体（定义在 .c 中，外部不可见）
// ============================================================

struct scene_node {
    float local_x, local_y;
    float rotation;
    float scale_x, scale_y;
    float world_x, world_y;

    struct scene_node* parent;
    struct scene_node* first_child;
    struct scene_node* next_sibling;
    int child_count;

    scene_node_update_fn  on_update;
    scene_node_render_fn  on_render;
    scene_node_destroy_fn on_destroy;
    scene_node_event_fn   on_event;

    void* userdata;
    int zorder;
    bool is_root;
};

// ============================================================
// 节点实现
// ============================================================

scene_node_p scene_node_create(void)
{
    struct scene_node* n = (struct scene_node*)SDL_malloc(sizeof(struct scene_node));
    SDL_assert(n);
    SDL_memset(n, 0, sizeof(struct scene_node));
    n->scale_x = 1.0f;
    n->scale_y = 1.0f;
    n->is_root = false;
    return n;
}

void scene_node_destroy(scene_node_p node)
{
    if (!node) return;
    scene_node_p child = node->first_child;
    while (child) {
        scene_node_p next = child->next_sibling;
        scene_node_destroy(child);
        child = next;
    }
    if (node->on_destroy) {
        node->on_destroy(node);
    }
    SDL_free(node);
}

void scene_node_add_child(scene_node_p parent, scene_node_p child)
{
    if (!parent || !child) return;
    if (child->parent) {
        scene_node_remove_child(child->parent, child);
    }
    child->parent = parent;
    child->next_sibling = parent->first_child;
    parent->first_child = child;
    parent->child_count++;
}

void scene_node_remove_child(scene_node_p parent, scene_node_p child)
{
    if (!parent || !child) return;
    struct scene_node* prev = NULL;
    struct scene_node* curr = parent->first_child;
    while (curr) {
        if (curr == child) {
            if (prev) {
                prev->next_sibling = curr->next_sibling;
            } else {
                parent->first_child = curr->next_sibling;
            }
            child->parent = NULL;
            child->next_sibling = NULL;
            parent->child_count--;
            return;
        }
        prev = curr;
        curr = curr->next_sibling;
    }
}

scene_node_p scene_node_get_parent(scene_node_p node) { return node ? node->parent : NULL; }
scene_node_p scene_node_get_first_child(scene_node_p node) { return node ? node->first_child : NULL; }
scene_node_p scene_node_get_next_sibling(scene_node_p node) { return node ? node->next_sibling : NULL; }
int scene_node_get_child_count(scene_node_p node) { return node ? node->child_count : 0; }

void scene_node_set_position(scene_node_p node, float x, float y)
{
    if (!node) return;
    node->local_x = x;
    node->local_y = y;
}

void scene_node_get_position(scene_node_p node, float* out_x, float* out_y)
{
    if (!node) return;
    if (out_x) *out_x = node->local_x;
    if (out_y) *out_y = node->local_y;
}

void scene_node_set_world_position(scene_node_p node, float x, float y)
{
    if (!node) return;
    node->world_x = x;
    node->world_y = y;
}

void scene_node_get_world_position(scene_node_p node, float* out_x, float* out_y)
{
    if (!node) return;
    float wx = node->local_x;
    float wy = node->local_y;
    scene_node_p p = node->parent;
    while (p) {
        wx += p->local_x;
        wy += p->local_y;
        p = p->parent;
    }
    if (out_x) *out_x = wx;
    if (out_y) *out_y = wy;
}

void scene_node_set_rotation(scene_node_p node, float radians) { if (node) node->rotation = radians; }
float scene_node_get_rotation(scene_node_p node) { return node ? node->rotation : 0.0f; }

void scene_node_set_scale(scene_node_p node, float scale_x, float scale_y)
{
    if (!node) return;
    node->scale_x = scale_x;
    node->scale_y = scale_y;
}

void scene_node_get_scale(scene_node_p node, float* out_x, float* out_y)
{
    if (!node) return;
    if (out_x) *out_x = node->scale_x;
    if (out_y) *out_y = node->scale_y;
}

void scene_node_set_zorder(scene_node_p node, int zorder) { if (node) node->zorder = zorder; }
int scene_node_get_zorder(scene_node_p node) { return node ? node->zorder : 0; }

void scene_node_set_userdata(scene_node_p node, void* ud) { if (node) node->userdata = ud; }
void* scene_node_get_userdata(scene_node_p node) { return node ? node->userdata : NULL; }

void scene_node_set_update_callback(scene_node_p node, scene_node_update_fn cb) { if (node) node->on_update = cb; }
void scene_node_set_render_callback(scene_node_p node, scene_node_render_fn cb) { if (node) node->on_render = cb; }
void scene_node_set_destroy_callback(scene_node_p node, scene_node_destroy_fn cb) { if (node) node->on_destroy = cb; }
void scene_node_set_event_callback(scene_node_p node, scene_node_event_fn cb) { if (node) node->on_event = cb; }

void scene_node_update(scene_node_p node, float delta_time)
{
    if (!node) return;

    node->world_x = node->local_x;
    node->world_y = node->local_y;
    scene_node_p p = node->parent;
    while (p) {
        node->world_x += p->local_x;
        node->world_y += p->local_y;
        p = p->parent;
    }

    if (node->on_update) node->on_update(node, delta_time);

    scene_node_p child = node->first_child;
    while (child) {
        scene_node_update(child, delta_time);
        child = child->next_sibling;
    }
}

void scene_node_render(scene_node_p node, const void* arg)
{
    if (!node) return;
    if (node->on_render) node->on_render(node, arg);
    scene_node_p child = node->first_child;
    while (child) {
        scene_node_render(child, arg);
        child = child->next_sibling;
    }
}

void scene_node_handle_event(scene_node_p node, const void* e)
{
    if (!node) return;
    if (node->on_event) node->on_event(node, e);
    scene_node_p child = node->first_child;
    while (child) {
        scene_node_handle_event(child, e);
        child = child->next_sibling;
    }
}

// ============================================================
// 场景结构体
// ============================================================

struct scene {
    scene_node_p* root_nodes;
    int root_count;
    int root_capacity;

    scene_load_fn    on_load;
    scene_update_fn  on_update;
    scene_render_fn  on_render;
    scene_destroy_fn on_destroy;
    scene_event_fn   on_event;

    void* userdata;
    char name[64];
};

// ============================================================
// 场景实现
// ============================================================

scene_p scene_create(const char* name)
{
    struct scene* s = (struct scene*)SDL_malloc(sizeof(struct scene));
    SDL_assert(s);
    SDL_memset(s, 0, sizeof(struct scene));
    if (name) SDL_strlcpy(s->name, name, sizeof(s->name));
    s->root_capacity = 16;
    s->root_nodes = (scene_node_p*)SDL_malloc(sizeof(scene_node_p) * s->root_capacity);
    SDL_assert(s->root_nodes);
    return s;
}

void scene_destroy(scene_p scene)
{
    if (!scene) return;
    if (scene->on_destroy) scene->on_destroy(scene);
    for (int i = 0; i < scene->root_count; i++) {
        scene_node_destroy(scene->root_nodes[i]);
    }
    SDL_free(scene->root_nodes);
    SDL_free(scene);
}

void scene_add_root_node(scene_p scene, scene_node_p root)
{
    if (!scene || !root) return;
    if (scene->root_count >= scene->root_capacity) {
        scene->root_capacity *= 2;
        scene->root_nodes = (scene_node_p*)SDL_realloc(scene->root_nodes, sizeof(scene_node_p) * scene->root_capacity);
        SDL_assert(scene->root_nodes);
    }
    root->is_root = true;
    scene->root_nodes[scene->root_count++] = root;
}

void scene_remove_root_node(scene_p scene, scene_node_p root)
{
    if (!scene || !root) return;
    for (int i = 0; i < scene->root_count; i++) {
        if (scene->root_nodes[i] == root) {
            for (int j = i; j < scene->root_count - 1; j++) {
                scene->root_nodes[j] = scene->root_nodes[j + 1];
            }
            scene->root_count--;
            root->is_root = false;
            return;
        }
    }
}

int scene_get_root_count(scene_p scene) { return scene ? scene->root_count : 0; }

scene_node_p scene_get_root_at(scene_p scene, int index)
{
    if (!scene || index < 0 || index >= scene->root_count) return NULL;
    return scene->root_nodes[index];
}

void scene_update(scene_p scene, float delta_time)
{
    if (!scene) return;
    if (scene->on_update) scene->on_update(scene, delta_time);
    for (int i = 0; i < scene->root_count; i++) {
        scene_node_update(scene->root_nodes[i], delta_time);
    }
}

void scene_render(scene_p scene, const void* arg)
{
    if (!scene) return;
    if (scene->on_render) scene->on_render(scene);
    for (int i = 0; i < scene->root_count; i++) {
        scene_node_render(scene->root_nodes[i], arg);
    }
}

void scene_handle_event(scene_p scene, const void* e)
{
    if (!scene || !e) return;
    if (scene->on_event) scene->on_event(scene, e);
    for (int i = 0; i < scene->root_count; i++) {
        scene_node_handle_event(scene->root_nodes[i], e);
    }
}

void scene_set_load_callback(scene_p scene, scene_load_fn cb) { if (scene) scene->on_load = cb; }
void scene_set_update_callback(scene_p scene, scene_update_fn cb) { if (scene) scene->on_update = cb; }
void scene_set_render_callback(scene_p scene, scene_render_fn cb) { if (scene) scene->on_render = cb; }
void scene_set_destroy_callback(scene_p scene, scene_destroy_fn cb) { if (scene) scene->on_destroy = cb; }
void scene_set_event_callback(scene_p scene, scene_event_fn cb) { if (scene) scene->on_event = cb; }
void scene_set_userdata(scene_p scene, void* ud) { if (scene) scene->userdata = ud; }
void* scene_get_userdata(scene_p scene) { return scene ? scene->userdata : NULL; }
const char* scene_get_name(scene_p scene) { return scene ? scene->name : ""; }

// ============================================================
// 场景管理器结构体
// ============================================================

struct scene_manager {
    scene_p* scenes;
    int scene_count;
    int scene_capacity;
};

// ============================================================
// 场景管理器实现
// ============================================================

scene_manager_p scene_manager_create(void)
{
    struct scene_manager* mgr = (struct scene_manager*)SDL_malloc(sizeof(struct scene_manager));
    SDL_assert(mgr);
    SDL_memset(mgr, 0, sizeof(struct scene_manager));
    mgr->scene_capacity = 8;
    mgr->scenes = (scene_p*)SDL_malloc(sizeof(scene_p) * mgr->scene_capacity);
    SDL_assert(mgr->scenes);
    return mgr;
}

void scene_manager_destroy(scene_manager_p mgr)
{
    if (!mgr) return;
    for (int i = 0; i < mgr->scene_count; i++) {
        scene_destroy(mgr->scenes[i]);
    }
    SDL_free(mgr->scenes);
    SDL_free(mgr);
}

void scene_manager_push(scene_manager_p mgr, scene_p scene)
{
    if (!mgr || !scene) return;
    if (mgr->scene_count >= mgr->scene_capacity) {
        mgr->scene_capacity *= 2;
        mgr->scenes = (scene_p*)SDL_realloc(mgr->scenes, sizeof(scene_p) * mgr->scene_capacity);
        SDL_assert(mgr->scenes);
    }
    if (scene->on_load) scene->on_load(scene);
    mgr->scenes[mgr->scene_count++] = scene;
}

void scene_manager_replace(scene_manager_p mgr, scene_p scene)
{
    if (!mgr || !scene) return;
    if (mgr->scene_count > 0) {
        scene_destroy(mgr->scenes[mgr->scene_count - 1]);
        mgr->scenes[mgr->scene_count - 1] = scene;
    } else {
        if (mgr->scene_capacity < 1) {
            mgr->scene_capacity = 8;
            mgr->scenes = (scene_p*)SDL_realloc(mgr->scenes, sizeof(scene_p) * mgr->scene_capacity);
        }
        mgr->scenes[mgr->scene_count++] = scene;
    }
    if (scene->on_load) scene->on_load(scene);
}

void scene_manager_pop(scene_manager_p mgr)
{
    if (!mgr || mgr->scene_count <= 0) return;
    scene_destroy(mgr->scenes[--mgr->scene_count]);
}

void scene_manager_update(scene_manager_p mgr, float delta_time)
{
    if (!mgr || mgr->scene_count == 0) return;
    scene_update(mgr->scenes[mgr->scene_count - 1], delta_time);
}

void scene_manager_render(scene_manager_p mgr, const void* arg)
{
    if (!mgr || mgr->scene_count == 0) return;
    scene_render(mgr->scenes[mgr->scene_count - 1], arg);
}

void scene_manager_handle_event(scene_manager_p mgr, const void* e)
{
    if (!mgr || mgr->scene_count == 0 || !e) return;
    scene_handle_event(mgr->scenes[mgr->scene_count - 1], e);
}

scene_p scene_manager_get_current(scene_manager_p mgr)
{
    if (!mgr || mgr->scene_count == 0) return NULL;
    return mgr->scenes[mgr->scene_count - 1];
}

int scene_manager_get_count(scene_manager_p mgr)
{
    return mgr ? mgr->scene_count : 0;
}
