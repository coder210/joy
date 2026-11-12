/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jscene.c
Description: 场景管理实现
Author: ydlc
Version: 1.0
Date: 2026.5.5
History:
************************************************/
#include <SDL3/SDL.h>
#include "joy.h"

// ============================================================
// 节点结构体（定义在 .c 中，外部不可见）
// ============================================================

struct joy_scene_node {
    float local_x, local_y;
    float rotation;
    float scale_x, scale_y;
    float world_x, world_y;

    struct joy_scene_node* parent;
    struct joy_scene_node* first_child;
    struct joy_scene_node* next_sibling;
    int child_count;

    scene_node_update_fn  on_update;
    scene_node_render_fn  on_render;
    scene_node_destroy_fn on_destroy;
    scene_node_event_fn   on_event;

    void* userdata;
    int zorder;
    bool is_root;

    char name[JOY_MIN_BUFFER];   /* 节点名称，用于查找 */
    char tag[JOY_MIN_BUFFER];    /* 节点标签，用于分组查找 */
};

// ============================================================
// 节点实现
// ============================================================

joy_scene_node_p joy_scene_node_create(void)
{
    struct joy_scene_node* n = (struct joy_scene_node*)SDL_malloc(sizeof(struct joy_scene_node));
    SDL_assert(n);
    SDL_memset(n, 0, sizeof(struct joy_scene_node));
    n->scale_x = 1.0f;
    n->scale_y = 1.0f;
    n->is_root = false;
    return n;
}

void joy_scene_node_destroy(joy_scene_node_p node)
{
    if (!node) return;
    joy_scene_node_p child = node->first_child;
    while (child) {
        joy_scene_node_p next = child->next_sibling;
        joy_scene_node_destroy(child);
        child = next;
    }
    if (node->on_destroy) {
        node->on_destroy(node);
    }
    SDL_free(node);
}

void joy_scene_node_add_child(joy_scene_node_p parent, joy_scene_node_p child)
{
    if (!parent || !child) return;
    if (child->parent) {
        joy_scene_node_remove_child(child->parent, child);
    }
    child->parent = parent;
    child->next_sibling = parent->first_child;
    parent->first_child = child;
    parent->child_count++;
}

void joy_scene_node_remove_child(joy_scene_node_p parent, joy_scene_node_p child)
{
    if (!parent || !child) return;
    struct joy_scene_node* prev = NULL;
    struct joy_scene_node* curr = parent->first_child;
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

joy_scene_node_p joy_scene_node_get_parent(joy_scene_node_p node) { return node ? node->parent : NULL; }
joy_scene_node_p joy_scene_node_get_first_child(joy_scene_node_p node) { return node ? node->first_child : NULL; }
joy_scene_node_p joy_scene_node_get_next_sibling(joy_scene_node_p node) { return node ? node->next_sibling : NULL; }
int joy_scene_node_get_child_count(joy_scene_node_p node) { return node ? node->child_count : 0; }

void joy_scene_node_set_position(joy_scene_node_p node, float x, float y)
{
    if (!node) return;
    node->local_x = x;
    node->local_y = y;
}

void joy_scene_node_get_position(joy_scene_node_p node, float* out_x, float* out_y)
{
    if (!node) return;
    if (out_x) *out_x = node->local_x;
    if (out_y) *out_y = node->local_y;
}

void joy_scene_node_set_world_position(joy_scene_node_p node, float x, float y)
{
    if (!node) return;
    node->world_x = x;
    node->world_y = y;
}

void joy_scene_node_get_world_position(joy_scene_node_p node, float* out_x, float* out_y)
{
    if (!node) return;
    float wx = node->local_x;
    float wy = node->local_y;
    joy_scene_node_p p = node->parent;
    while (p) {
        wx += p->local_x;
        wy += p->local_y;
        p = p->parent;
    }
    if (out_x) *out_x = wx;
    if (out_y) *out_y = wy;
}

void joy_scene_node_set_rotation(joy_scene_node_p node, float radians) { if (node) node->rotation = radians; }
float joy_scene_node_get_rotation(joy_scene_node_p node) { return node ? node->rotation : 0.0f; }

void joy_scene_node_set_scale(joy_scene_node_p node, float scale_x, float scale_y)
{
    if (!node) return;
    node->scale_x = scale_x;
    node->scale_y = scale_y;
}

void joy_scene_node_get_scale(joy_scene_node_p node, float* out_x, float* out_y)
{
    if (!node) return;
    if (out_x) *out_x = node->scale_x;
    if (out_y) *out_y = node->scale_y;
}

void joy_scene_node_set_zorder(joy_scene_node_p node, int zorder) { if (node) node->zorder = zorder; }
int joy_scene_node_get_zorder(joy_scene_node_p node) { return node ? node->zorder : 0; }

void joy_scene_node_set_userdata(joy_scene_node_p node, void* ud) { if (node) node->userdata = ud; }
void* joy_scene_node_get_userdata(joy_scene_node_p node) { return node ? node->userdata : NULL; }

/* ===== name / tag ===== */
void joy_scene_node_set_name(joy_scene_node_p node, const char* name) {
    if (node && name) SDL_strlcpy(node->name, name, sizeof(node->name));
}
const char* joy_scene_node_get_name(joy_scene_node_p node) { return node ? node->name : ""; }

void joy_scene_node_set_tag(joy_scene_node_p node, const char* tag) {
    if (node && tag) SDL_strlcpy(node->tag, tag, sizeof(node->tag));
}
const char* joy_scene_node_get_tag(joy_scene_node_p node) { return node ? node->tag : ""; }


/* ===== 递归查找 ===== */

/* 从 root 开始向下 DFS，找 name 匹配的第一个节点 */
static joy_scene_node_p find_name_r(joy_scene_node_p root, const char* name) {
    if (!root || !name) return NULL;
    if (SDL_strcmp(root->name, name) == 0) return root;
    joy_scene_node_p child = root->first_child;
    while (child) {
        joy_scene_node_p found = find_name_r(child, name);
        if (found) return found;
        child = child->next_sibling;
    }
    return NULL;
}

joy_scene_node_p joy_scene_find_node_by_name(joy_scene_node_p root, const char* name) {
    return find_name_r(root, name);
}

/* 从 root 开始 DFS，收集所有 tag 匹配的节点到 out 数组 */
static int find_tag_r(joy_scene_node_p root, const char* tag, joy_scene_node_p* out, int max, int count) {
    if (!root || !tag) return count;
    if (SDL_strcmp(root->tag, tag) == 0 && count < max) {
        out[count++] = root;
    }
    joy_scene_node_p child = root->first_child;
    while (child) {
        count = find_tag_r(child, tag, out, max, count);
        child = child->next_sibling;
    }
    return count;
}

int joy_scene_find_nodes_by_tag(joy_scene_node_p root, const char* tag, joy_scene_node_p* out, int max) {
    if (!root || !tag) return 0;
    return find_tag_r(root, tag, out, max, 0);
}

void joy_scene_node_set_update_callback(joy_scene_node_p node, scene_node_update_fn cb) { if (node) node->on_update = cb; }
void joy_scene_node_set_render_callback(joy_scene_node_p node, scene_node_render_fn cb) { if (node) node->on_render = cb; }
void joy_scene_node_set_destroy_callback(joy_scene_node_p node, scene_node_destroy_fn cb) { if (node) node->on_destroy = cb; }
void joy_scene_node_set_event_callback(joy_scene_node_p node, scene_node_event_fn cb) { if (node) node->on_event = cb; }

void joy_scene_node_update(joy_scene_node_p node, float delta_time)
{
    if (!node) return;

    node->world_x = node->local_x;
    node->world_y = node->local_y;
    joy_scene_node_p p = node->parent;
    while (p) {
        node->world_x += p->local_x;
        node->world_y += p->local_y;
        p = p->parent;
    }

    if (node->on_update) node->on_update(node, delta_time);

    joy_scene_node_p child = node->first_child;
    while (child) {
        joy_scene_node_update(child, delta_time);
        child = child->next_sibling;
    }
}

void joy_scene_node_render(joy_scene_node_p node, const void* arg)
{
    if (!node) return;
    if (node->on_render) node->on_render(node, arg);
    joy_scene_node_p child = node->first_child;
    while (child) {
        joy_scene_node_render(child, arg);
        child = child->next_sibling;
    }
}

void joy_scene_node_handle_event(joy_scene_node_p node, const void* e)
{
    if (!node) return;
    if (node->on_event) node->on_event(node, e);
    joy_scene_node_p child = node->first_child;
    while (child) {
        joy_scene_node_handle_event(child, e);
        child = child->next_sibling;
    }
}

// ============================================================
// 场景结构体
// ============================================================

struct joy_scene {
    joy_scene_node_p* root_nodes;
    int root_count;
    int root_capacity;
    joy_scene_node_p popup_node;   // 弹层节点，渲染时保证在最上层

    scene_load_fn    on_load;
    scene_update_fn  on_update;
    scene_render_fn  on_render;
    scene_destroy_fn on_destroy;
    scene_event_fn   on_event;

    scene_fixed_update_fn on_fixed_update; /* 固定步长更新回调(Unity FixedUpdate 对应)，NULL=不用 */
    float fixed_step;                      /* 固定步长(s)；<=0 表示关闭固定步长调度 */
    float fixed_accum;                     /* 固定步长累加器 */

    void* userdata;
    char name[JOY_MAX_BUFFER];
};

// ============================================================
// 场景实现
// ============================================================

joy_scene_p joy_scene_create(const char* name)
{
    joy_scene_p s = (joy_scene_p)SDL_malloc(sizeof(struct joy_scene));
    SDL_assert(s);
    SDL_memset(s, 0, sizeof(struct joy_scene));
    if (name) SDL_strlcpy(s->name, name, sizeof(s->name));
    s->root_capacity = 16;
    s->root_nodes = (joy_scene_node_p*)SDL_malloc(sizeof(joy_scene_node_p) * s->root_capacity);
    SDL_assert(s->root_nodes);
    return s;
}

void joy_scene_destroy(joy_scene_p scene)
{
    if (!scene) return;
    if (scene->on_destroy) scene->on_destroy(scene);
    for (int i = 0; i < scene->root_count; i++) {
        joy_scene_node_destroy(scene->root_nodes[i]);
    }
    if (scene->popup_node) joy_scene_node_destroy(scene->popup_node);
    SDL_free(scene->root_nodes);
    SDL_free(scene);
}

void joy_scene_add_root_node(joy_scene_p scene, joy_scene_node_p root)
{
    if (!scene || !root) return;
    if (scene->root_count >= scene->root_capacity) {
        scene->root_capacity *= 2;
        scene->root_nodes = (joy_scene_node_p*)SDL_realloc(scene->root_nodes, sizeof(joy_scene_node_p) * scene->root_capacity);
        SDL_assert(scene->root_nodes);
    }
    root->is_root = true;
    scene->root_nodes[scene->root_count++] = root;
}

void joy_scene_remove_root_node(joy_scene_p scene, joy_scene_node_p root)
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

int joy_scene_get_root_count(joy_scene_p scene) { return scene ? scene->root_count : 0; }

joy_scene_node_p joy_scene_get_root_at(joy_scene_p scene, int index)
{
    if (!scene || index < 0 || index >= scene->root_count) return NULL;
    return scene->root_nodes[index];
}

/* ===== 场景级递归查找（依赖 struct joy_scene 定义） ===== */
joy_scene_node_p joy_scene_find_node_by_name_deep(joy_scene_p scene, const char* name) {
    if (!scene || !name) return NULL;
    for (int i = 0; i < scene->root_count; i++) {
        joy_scene_node_p found = find_name_r(scene->root_nodes[i], name);
        if (found) return found;
    }
    return NULL;
}

int joy_scene_find_nodes_by_tag_deep(joy_scene_p scene, const char* tag, joy_scene_node_p* out, int max) {
    if (!scene || !tag || !out || max <= 0) return 0;
    int count = 0;
    for (int i = 0; i < scene->root_count && count < max; i++) {
        count = find_tag_r(scene->root_nodes[i], tag, out, max, count);
    }
    return count;
}

void joy_scene_update(joy_scene_p scene, float delta_time)
{
    if (!scene) return;
    if (scene->fixed_step > 0.0f) {
        scene->fixed_accum += delta_time;
        while (scene->fixed_accum >= scene->fixed_step) {
            scene->fixed_accum -= scene->fixed_step;
            if (scene->on_fixed_update) scene->on_fixed_update(scene, scene->fixed_step);
        }
    }
    if (scene->on_update) scene->on_update(scene, delta_time);
    for (int i = 0; i < scene->root_count; i++) {
        joy_scene_node_update(scene->root_nodes[i], delta_time);
    }
}

void joy_scene_render(joy_scene_p scene, const void* arg)
{
    if (!scene) return;
    if (scene->on_render) scene->on_render(scene);
    for (int i = 0; i < scene->root_count; i++) {
        joy_scene_node_render(scene->root_nodes[i], arg);
    }
    // 弹层节点最后渲染，保证在所有普通节点之上
    if (scene->popup_node) joy_scene_node_render(scene->popup_node, arg);
}

void joy_scene_handle_event(joy_scene_p scene, const void* e)
{
    if (!scene || !e) return;
    if (scene->on_event) scene->on_event(scene, e);
    for (int i = 0; i < scene->root_count; i++) {
        joy_scene_node_handle_event(scene->root_nodes[i], e);
    }
}

void joy_scene_set_load_callback(joy_scene_p scene, scene_load_fn cb) { if (scene) scene->on_load = cb; }
void joy_scene_set_update_callback(joy_scene_p scene, scene_update_fn cb)
{ 
        if (scene) scene->on_update = cb;
}
void joy_scene_set_render_callback(joy_scene_p scene, scene_render_fn cb) { if (scene) scene->on_render = cb; }
void joy_scene_set_destroy_callback(joy_scene_p scene, scene_destroy_fn cb) { if (scene) scene->on_destroy = cb; }
void joy_scene_set_event_callback(joy_scene_p scene, scene_event_fn cb) { if (scene) scene->on_event = cb; }

/* 启用固定步长更新(Unity FixedUpdate 对应)。step<=0 关闭，回调可传 NULL */
void joy_scene_set_fixed_update_callback(joy_scene_p scene, scene_fixed_update_fn cb, float step) {
	if (scene) {
		scene->on_fixed_update = cb;
		scene->fixed_step = step;
		scene->fixed_accum = 0.0f;
	}
}
void joy_scene_set_userdata(joy_scene_p scene, void* ud) { if (scene) scene->userdata = ud; }
void* joy_scene_get_userdata(joy_scene_p scene) { return scene ? scene->userdata : NULL; }
const char* joy_scene_get_name(joy_scene_p scene) { return scene ? scene->name : ""; }

void joy_scene_set_popup_node(joy_scene_p scene, joy_scene_node_p node) { if (scene) scene->popup_node = node; }
joy_scene_node_p joy_scene_get_popup_node(joy_scene_p scene) { return scene ? scene->popup_node : NULL; }

// ============================================================
// 场景管理器结构体
// ============================================================

struct joy_scene_manager {
    joy_scene_p* scenes;
    int scene_count;
    int scene_capacity;
};

// ============================================================
// 场景管理器实现
// ============================================================

joy_scene_manager_p joy_scene_manager_create(void)
{
    struct joy_scene_manager* mgr = (struct joy_scene_manager*)SDL_malloc(sizeof(struct joy_scene_manager));
    SDL_assert(mgr);
    SDL_memset(mgr, 0, sizeof(struct joy_scene_manager));
    mgr->scene_capacity = 8;
    mgr->scenes = (joy_scene_p*)SDL_malloc(sizeof(joy_scene_p) * mgr->scene_capacity);
    SDL_assert(mgr->scenes);
    return mgr;
}

void joy_scene_manager_destroy(joy_scene_manager_p mgr)
{
    if (!mgr) return;
    for (int i = 0; i < mgr->scene_count; i++) {
        joy_scene_destroy(mgr->scenes[i]);
    }
    SDL_free(mgr->scenes);
    SDL_free(mgr);
}

void joy_scene_manager_push(joy_scene_manager_p mgr, joy_scene_p scene)
{
    if (!mgr || !scene) return;
    if (mgr->scene_count >= mgr->scene_capacity) {
        mgr->scene_capacity *= 2;
        mgr->scenes = (joy_scene_p*)SDL_realloc(mgr->scenes, sizeof(joy_scene_p) * mgr->scene_capacity);
        SDL_assert(mgr->scenes);
    }
    if (scene->on_load) scene->on_load(scene);
    mgr->scenes[mgr->scene_count++] = scene;
}

void joy_scene_manager_replace(joy_scene_manager_p mgr, joy_scene_p scene)
{
    if (!mgr || !scene) return;
    if (mgr->scene_count > 0) {
        joy_scene_p old = mgr->scenes[mgr->scene_count - 1];
        if (old != scene) {
            joy_scene_destroy(old);
        }
        mgr->scenes[mgr->scene_count - 1] = scene;
    } else {
        if (mgr->scene_capacity < 1) {
            mgr->scene_capacity = 8;
            mgr->scenes = (joy_scene_p*)SDL_realloc(mgr->scenes, sizeof(joy_scene_p) * mgr->scene_capacity);
        }
        mgr->scenes[mgr->scene_count++] = scene;
    }
    if (scene->on_load) scene->on_load(scene);
}

void joy_scene_manager_pop(joy_scene_manager_p mgr)
{
    if (!mgr || mgr->scene_count <= 0) return;
    joy_scene_destroy(mgr->scenes[--mgr->scene_count]);
}

void joy_scene_manager_update(joy_scene_manager_p mgr, float delta_time)
{
    if (!mgr || mgr->scene_count == 0) return;
    joy_scene_update(mgr->scenes[mgr->scene_count - 1], delta_time);
}

void joy_scene_manager_render(joy_scene_manager_p mgr, const void* arg)
{
    if (!mgr || mgr->scene_count == 0) return;
    joy_scene_render(mgr->scenes[mgr->scene_count - 1], arg);
}

void joy_scene_manager_handle_event(joy_scene_manager_p mgr, const void* e)
{
    if (!mgr || mgr->scene_count == 0 || !e) return;
    joy_scene_handle_event(mgr->scenes[mgr->scene_count - 1], e);
}

joy_scene_p joy_scene_manager_get_current(joy_scene_manager_p mgr)
{
    if (!mgr || mgr->scene_count == 0) return NULL;
    return mgr->scenes[mgr->scene_count - 1];
}

int joy_scene_manager_get_count(joy_scene_manager_p mgr)
{
    return mgr ? mgr->scene_count : 0;
}
