/******************************************************************************
 * jspine —— joy 的 Spine 骨骼动画模块（可选，需定义 JOY_USE_SPINE 并链接 spine-c）
 *
 * 渲染实现基于 Spine 官方 spine-sdl 封装（spine-runtimes 4.1，spine-sdl/src/spine-sdl-c.c），
 * 移植到 SDL3（SDL_Vertex.color 在 SDL3 是 SDL_FColor）并接入 joy 的 jr2d。
 *
 * 面向 ECS 的两层结构：
 *   jspine_asset_t    共享资源（atlas + skeletonData + animationStateData），多实体复用
 *   jspine_instance_t 组件：每实体一份，纯数据，可直接 emplace 进 ECS（entt 等）
 *
 * 未启用 JOY_USE_SPINE 时本文件为空编译单元，不影响任何项目。
 *****************************************************************************/

#ifdef JOY_USE_SPINE

#include <SDL3/SDL.h>
#include <spine/spine.h>
#include <spine/extension.h>

#include "joy.h"
#include "external/stb_image.h" /* 实现在 jimpl.c（STB_IMAGE_IMPLEMENTATION） */

#include <float.h>
#include <string.h>

/* ==================== 内部类型 ==================== */

struct jspine_asset {
        spAtlas *atlas;
        spSkeletonData *data;
        spAnimationStateData *state_data; /* 共享的 mix 配置 */
        float scale;
};

/* 顶点缓存（组件里以 void* 持有，避免 joy.h 暴露 SDL 类型） */
typedef struct {
        SDL_Vertex *items;
        int size;
        int capacity;
} jspine_vertex_array_t;

typedef struct jspine_draw_cache {
        jspine_vertex_array_t vertices;
        spIntArray *indices;
        spFloatArray *world_vertices;
} jspine_draw_cache_t;

/* ==================== spine-c 要求的宿主回调 ==================== */

void _spAtlasPage_createTexture(spAtlasPage *self, const char *path) {
        int width = 0, height = 0, components = 0;
        stbi_uc *image_data;
        SDL_Texture *texture;

        image_data = stbi_load(path, &width, &height, &components, 4);
        if (!image_data) {
                /* 官方原版静默返回，这里补日志便于定位资源路径问题 */
                printf("[jspine] atlas page load failed: '%s' (%s)\n",
                       path, stbi_failure_reason() ? stbi_failure_reason() : "unknown");
                return;
        }
        texture = SDL_CreateTexture((SDL_Renderer *) self->atlas->rendererObject,
                                    SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, width, height);
        if (!texture) {
                printf("[jspine] SDL_CreateTexture failed: %s\n", SDL_GetError());
                stbi_image_free(image_data);
                return;
        }
        if (!SDL_UpdateTexture(texture, NULL, image_data, width * 4)) {
                printf("[jspine] SDL_UpdateTexture failed: %s\n", SDL_GetError());
                SDL_DestroyTexture(texture);
                stbi_image_free(image_data);
                return;
        }
        stbi_image_free(image_data);
        self->rendererObject = texture;
}

void _spAtlasPage_disposeTexture(spAtlasPage *self) {
        if (self && self->rendererObject) SDL_DestroyTexture((SDL_Texture *) self->rendererObject);
}

char *_spUtil_readFile(const char *path, int *length) {
        return _spReadFile(path, length);
}

/* ==================== 内部工具 ==================== */

static void vertex_array_clear(jspine_vertex_array_t *arr) {
        if (arr) arr->size = 0;
}

static void vertex_array_add(jspine_vertex_array_t *arr, SDL_Vertex v) {
        int i;
        if (!arr) return;
        if (arr->size == arr->capacity) {
                int new_cap = arr->capacity > 0 ? arr->capacity * 2 : 32;
                SDL_Vertex *new_items = (SDL_Vertex *) calloc((size_t) new_cap, sizeof(SDL_Vertex));
                if (!new_items) return;
                for (i = 0; i < arr->size; i++) new_items[i] = arr->items[i];
                free(arr->items);
                arr->items = new_items;
                arr->capacity = new_cap;
        }
        arr->items[arr->size++] = v;
}

static int ends_with(const char *path, const char *suffix) {
        size_t plen = strlen(path);
        size_t slen = strlen(suffix);
        if (slen > plen) return 0;
#ifdef _MSC_VER
        return _stricmp(path + plen - slen, suffix) == 0;
#else
        return strcasecmp(path + plen - slen, suffix) == 0;
#endif
}

/* 收集一个 slot 的世界顶点/UV/索引/纹理，返回值：0=无可绘制内容，1=ok */
static int collect_attachment(spSlot *slot, spAttachment *attachment,
                              spFloatArray *world_vertices,
                              float **out_uvs, unsigned short **out_indices,
                              int *out_vertices_count, int *out_indices_count,
                              SDL_Texture **out_texture) {
        if (attachment->type == SP_ATTACHMENT_REGION) {
                static unsigned short quad_indices[] = {0, 1, 2, 2, 3, 0};
                spRegionAttachment *region = (spRegionAttachment *) attachment;
                spFloatArray_setSize(world_vertices, 8);
                spRegionAttachment_computeWorldVertices(region, slot, world_vertices->items, 0, 2);
                *out_uvs = region->uvs;
                *out_indices = quad_indices;
                *out_vertices_count = 4;
                *out_indices_count = 6;
                *out_texture = (SDL_Texture *) ((spAtlasRegion *) region->rendererObject)->page->rendererObject;
                return 1;
        } else if (attachment->type == SP_ATTACHMENT_MESH) {
                spMeshAttachment *mesh = (spMeshAttachment *) attachment;
                spFloatArray_setSize(world_vertices, mesh->super.worldVerticesLength);
                spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0,
                                                        mesh->super.worldVerticesLength,
                                                        world_vertices->items, 0, 2);
                *out_uvs = mesh->uvs;
                *out_indices = mesh->triangles;
                *out_vertices_count = mesh->super.worldVerticesLength >> 1;
                *out_indices_count = mesh->trianglesCount;
                *out_texture = (SDL_Texture *) ((spAtlasRegion *) mesh->rendererObject)->page->rendererObject;
                return 1;
        }
        return 0;
}

/* ==================== 共享资源 ==================== */

jspine_asset_p jspine_asset_load(jr2d_p r2d, const char *atlas_path, const char *skeleton_path,
                                 float scale) {
        jspine_asset_p asset;
        spSkeletonJson *json = NULL;
        spSkeletonBinary *binary = NULL;

        if (!r2d || !atlas_path || !skeleton_path) return NULL;

        /* 全局：让 spine 使用 y 向下的坐标系，与屏幕坐标一致 */
        spBone_setYDown(-1);

        asset = (jspine_asset_p) calloc(1, sizeof(jspine_asset_t));
        if (!asset) return NULL;
        asset->scale = scale > 0 ? scale : 1.0f;

        /* renderer 作为 atlas 的 rendererObject，回调里用它建纹理 */
        asset->atlas = spAtlas_createFromFile(atlas_path, r2d_unwrap(r2d));
        if (!asset->atlas) {
                printf("[jspine] atlas read failed: %s\n", atlas_path);
                free(asset);
                return NULL;
        }

        if (ends_with(skeleton_path, ".skel")) {
                binary = spSkeletonBinary_create(asset->atlas);
                binary->scale = asset->scale;
                asset->data = spSkeletonBinary_readSkeletonDataFile(binary, skeleton_path);
                if (!asset->data) {
                        printf("[jspine] skeleton binary failed: %s\n",
                               binary->error ? binary->error : "(unknown)");
                }
                spSkeletonBinary_dispose(binary);
        } else {
                json = spSkeletonJson_create(asset->atlas);
                json->scale = asset->scale;
                asset->data = spSkeletonJson_readSkeletonDataFile(json, skeleton_path);
                if (!asset->data) {
                        printf("[jspine] skeleton json failed: %s\n",
                               json->error ? json->error : "(unknown)");
                }
                spSkeletonJson_dispose(json);
        }
        if (!asset->data) {
                spAtlas_dispose(asset->atlas);
                free(asset);
                return NULL;
        }

        asset->state_data = spAnimationStateData_create(asset->data);
        asset->state_data->defaultMix = 0.2f;
        return asset;
}

void jspine_asset_free(jspine_asset_p asset) {
        if (!asset) return;
        if (asset->state_data) spAnimationStateData_dispose(asset->state_data);
        if (asset->data) spSkeletonData_dispose(asset->data);
        if (asset->atlas) spAtlas_dispose(asset->atlas);
        free(asset);
}

spSkeletonData *jspine_asset_data(jspine_asset_p asset) {
        return asset ? asset->data : NULL;
}

int jspine_asset_animation_count(jspine_asset_p asset) {
        return (asset && asset->data) ? asset->data->animationsCount : 0;
}

const char *jspine_asset_animation_name(jspine_asset_p asset, int index) {
        if (!asset || !asset->data || index < 0 || index >= asset->data->animationsCount) return NULL;
        return asset->data->animations[index]->name;
}

float jspine_asset_animation_duration(jspine_asset_p asset, int index) {
        if (!asset || !asset->data || index < 0 || index >= asset->data->animationsCount) return 0;
        return asset->data->animations[index]->duration;
}

/* ==================== 组件（实例） ==================== */

bool jspine_instance_init(jspine_instance_p inst, jspine_asset_p asset) {
        jspine_draw_cache_t *cache;

        if (!inst || !asset) return false;
        memset(inst, 0, sizeof(*inst));

        inst->asset = asset;
        inst->skeleton = spSkeleton_create(asset->data);
        inst->state = spAnimationState_create(asset->state_data);
        inst->clipper = spSkeletonClipping_create();

        cache = (jspine_draw_cache_t *) calloc(1, sizeof(jspine_draw_cache_t));
        if (!cache) {
                spSkeleton_dispose(inst->skeleton);
                spAnimationState_dispose(inst->state);
                spSkeletonClipping_dispose(inst->clipper);
                memset(inst, 0, sizeof(*inst));
                return false;
        }
        cache->indices = spIntArray_create(12);
        cache->world_vertices = spFloatArray_create(64);
        inst->draw_cache = cache;

        /* 默认状态 */
        inst->scale_x = 1.0f;
        inst->scale_y = 1.0f;
        inst->time_scale = 1.0f;
        inst->alpha = 1.0f;
        inst->tint.r = 255; inst->tint.g = 255; inst->tint.b = 255; inst->tint.a = 255;
        inst->visible = true;
        inst->zorder = 0;

        spSkeleton_setToSetupPose(inst->skeleton);
        spSkeleton_updateWorldTransform(inst->skeleton);
        return true;
}

void jspine_instance_free(jspine_instance_p inst) {
        jspine_draw_cache_t *cache;
        if (!inst) return;
        cache = (jspine_draw_cache_t *) inst->draw_cache;
        if (cache) {
                if (cache->indices) spIntArray_dispose(cache->indices);
                if (cache->world_vertices) spFloatArray_dispose(cache->world_vertices);
                free(cache->vertices.items);
                free(cache);
        }
        if (inst->clipper) spSkeletonClipping_dispose(inst->clipper);
        if (inst->state) spAnimationState_dispose(inst->state);
        if (inst->skeleton) spSkeleton_dispose(inst->skeleton);
        memset(inst, 0, sizeof(*inst)); /* asset 不属于实例，不释放 */
}

void jspine_instance_reset(jspine_instance_p inst) {
        if (!inst || !inst->skeleton) return;
        spSkeleton_setToSetupPose(inst->skeleton);
        spSkeleton_updateWorldTransform(inst->skeleton);
}

bool jspine_instance_set_animation(jspine_instance_p inst, int track, const char *name, bool loop) {
        spAnimation *animation;
        if (!inst || !inst->state || !name) return false;
        animation = spSkeletonData_findAnimation(inst->asset->data, name);
        if (!animation) return false;
        spAnimationState_setAnimation(inst->state, track, animation, loop ? 1 : 0);
        strncpy(inst->animation, name, JSPINE_NAME_MAX - 1);
        inst->animation[JSPINE_NAME_MAX - 1] = '\0';
        return true;
}

bool jspine_instance_add_animation(jspine_instance_p inst, int track, const char *name, bool loop,
                                   float delay) {
        spAnimation *animation;
        if (!inst || !inst->state || !name) return false;
        animation = spSkeletonData_findAnimation(inst->asset->data, name);
        if (!animation) return false;
        spAnimationState_addAnimation(inst->state, track, animation, loop ? 1 : 0, delay);
        return true;
}

void jspine_instance_update(jspine_instance_p inst, float dt) {
        spSkeleton *skeleton;
        if (!inst || !inst->skeleton) return;
        skeleton = inst->skeleton;

        /* 组件字段 -> spine 骨架 */
        skeleton->x = inst->x;
        skeleton->y = inst->y;
        skeleton->scaleX = inst->scale_x * (inst->flip_x ? -1.0f : 1.0f);
        skeleton->scaleY = inst->scale_y;
        if (skeleton->root) skeleton->root->rotation = inst->rotation;
        skeleton->color.r = inst->tint.r / 255.0f;
        skeleton->color.g = inst->tint.g / 255.0f;
        skeleton->color.b = inst->tint.b / 255.0f;
        skeleton->color.a = inst->alpha * (inst->tint.a / 255.0f);

        if (inst->state) {
                spAnimationState_update(inst->state, dt * inst->time_scale);
                spAnimationState_apply(inst->state, skeleton);
        }
        spSkeleton_updateWorldTransform(skeleton);
}

void jspine_instance_draw(jr2d_p r2d, jspine_instance_p inst) {
        SDL_Renderer *renderer;
        spSkeleton *skeleton;
        spSkeletonClipping *clipper;
        jspine_draw_cache_t *cache;
        int i;

        if (!r2d || !inst || !inst->skeleton || !inst->visible) return;
        cache = (jspine_draw_cache_t *) inst->draw_cache;
        if (!cache) return;

        renderer = (SDL_Renderer *) r2d_unwrap(r2d);
        skeleton = inst->skeleton;
        clipper = inst->clipper;

        for (i = 0; i < skeleton->slotsCount; ++i) {
                spSlot *slot = skeleton->drawOrder[i];
                spAttachment *attachment = slot->attachment;
                spFloatArray *vertices = cache->world_vertices;
                spColor *attachment_color = NULL;
                float *uvs = NULL;
                unsigned short *indices = NULL;
                int vertices_count = 0;
                int indices_count = 0;
                SDL_Texture *texture = NULL;
                SDL_Vertex sdl_vertex;
                int ii;

                if (!attachment) {
                        if (clipper) spSkeletonClipping_clipEnd(clipper, slot);
                        continue;
                }
                if (slot->color.a == 0 || !slot->bone->active) {
                        if (clipper) spSkeletonClipping_clipEnd(clipper, slot);
                        continue;
                }

                if (!collect_attachment(slot, attachment, vertices, &uvs, &indices,
                                        &vertices_count, &indices_count, &texture)) {
                        if (attachment->type == SP_ATTACHMENT_CLIPPING) {
                                spSkeletonClipping_clipStart(clipper, slot,
                                                             (spClippingAttachment *) attachment);
                        }
                        continue;
                }
                attachment_color = (attachment->type == SP_ATTACHMENT_REGION)
                                   ? &((spRegionAttachment *) attachment)->color
                                   : &((spMeshAttachment *) attachment)->color;
                if (attachment_color->a == 0 || !texture) {
                        if (clipper) spSkeletonClipping_clipEnd(clipper, slot);
                        continue;
                }

                /* SDL3: SDL_Vertex.color 是 SDL_FColor（0.0~1.0） */
                sdl_vertex.color.r = skeleton->color.r * slot->color.r * attachment_color->r;
                sdl_vertex.color.g = skeleton->color.g * slot->color.g * attachment_color->g;
                sdl_vertex.color.b = skeleton->color.b * slot->color.b * attachment_color->b;
                sdl_vertex.color.a = skeleton->color.a * slot->color.a * attachment_color->a;

                if (clipper && spSkeletonClipping_isClipping(clipper)) {
                        spSkeletonClipping_clipTriangles(clipper, vertices->items, vertices_count << 1,
                                                         indices, indices_count, uvs, 2);
                        vertices = clipper->clippedVertices;
                        vertices_count = clipper->clippedVertices->size >> 1;
                        uvs = clipper->clippedUVs->items;
                        indices = (unsigned short *) clipper->clippedTriangles->items;
                        indices_count = clipper->clippedTriangles->size;
                }

                vertex_array_clear(&cache->vertices);
                for (ii = 0; ii < vertices_count << 1; ii += 2) {
                        sdl_vertex.position.x = vertices->items[ii];
                        sdl_vertex.position.y = vertices->items[ii + 1];
                        sdl_vertex.tex_coord.x = uvs[ii];
                        sdl_vertex.tex_coord.y = uvs[ii + 1];
                        vertex_array_add(&cache->vertices, sdl_vertex);
                }
                spIntArray_clear(cache->indices);
                for (ii = 0; ii < indices_count; ii++)
                        spIntArray_add(cache->indices, indices[ii]);

                switch (slot->data->blendMode) {
                        case SP_BLEND_MODE_MULTIPLY:
                                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_MOD);
                                break;
                        case SP_BLEND_MODE_ADDITIVE:
                                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
                                break;
                        case SP_BLEND_MODE_SCREEN:
                        case SP_BLEND_MODE_NORMAL:
                        default:
                                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                                break;
                }

                SDL_RenderGeometry(renderer, texture, cache->vertices.items, cache->vertices.size,
                                   cache->indices->items, indices_count);
                if (clipper) spSkeletonClipping_clipEnd(clipper, slot);
        }
        if (clipper) spSkeletonClipping_clipEnd2(clipper);
}

/* ==================== 变换/状态便捷接口 ==================== */

void jspine_instance_set_position(jspine_instance_p inst, float x, float y) {
        if (inst) { inst->x = x; inst->y = y; }
}

void jspine_instance_set_scale(jspine_instance_p inst, float sx, float sy) {
        if (inst) { inst->scale_x = sx; inst->scale_y = sy; }
}

void jspine_instance_set_rotation(jspine_instance_p inst, float degrees) {
        if (inst) inst->rotation = degrees;
}

void jspine_instance_set_flip_x(jspine_instance_p inst, bool flip) {
        if (inst) inst->flip_x = flip;
}

void jspine_instance_set_alpha(jspine_instance_p inst, float alpha) {
        if (inst) inst->alpha = alpha;
}

void jspine_instance_set_tint(jspine_instance_p inst, jcolor_t tint) {
        if (inst) inst->tint = tint;
}

const char *jspine_instance_animation(jspine_instance_p inst) {
        return (inst && inst->animation[0]) ? inst->animation : NULL;
}

bool jspine_instance_bounds(jspine_instance_p inst, float *out_x, float *out_y, float *out_w,
                            float *out_h) {
        spSkeleton *skeleton;
        jspine_draw_cache_t *cache;
        float min_x = FLT_MAX, min_y = FLT_MAX, max_x = -FLT_MAX, max_y = -FLT_MAX;
        int i, found = 0;

        if (!inst || !inst->skeleton) return false;
        skeleton = inst->skeleton;
        cache = (jspine_draw_cache_t *) inst->draw_cache;
        if (!cache) return false;

        for (i = 0; i < skeleton->slotsCount; i++) {
                spSlot *slot = skeleton->drawOrder[i];
                spAttachment *attachment = slot->attachment;
                float *uvs = NULL;
                unsigned short *indices = NULL;
                int vertices_count = 0, indices_count = 0;
                SDL_Texture *texture = NULL;
                int v;

                if (!attachment) continue;
                if (!collect_attachment(slot, attachment, cache->world_vertices, &uvs, &indices,
                                        &vertices_count, &indices_count, &texture)) continue;
                for (v = 0; v < vertices_count; v++) {
                        float x = cache->world_vertices->items[v * 2];
                        float y = cache->world_vertices->items[v * 2 + 1];
                        if (x < min_x) min_x = x;
                        if (y < min_y) min_y = y;
                        if (x > max_x) max_x = x;
                        if (y > max_y) max_y = y;
                }
                found = 1;
        }
        if (!found) return false;
        if (out_x) *out_x = min_x;
        if (out_y) *out_y = min_y;
        if (out_w) *out_w = max_x - min_x;
        if (out_h) *out_h = max_y - min_y;
        return true;
}

#endif /* JOY_USE_SPINE */
