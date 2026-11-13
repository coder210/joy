/******************************************************************************
 * src/joy_r2d.h —— r2d 域声明头（joy_r2d.c 的配套头）
 * 职责：2D 渲染 / 字体 / 粒子 / spine
 * 方向：本头是该域声明的归属地，由 include/joy/japi.h 聚合包含；
 *       本头只依赖基础类型头，不再 include 聚合门面 joy.h，避免循环包含。
 * 生成链：本头是 gen_bindings.js 的 DECL_SOURCES 之一，改动声明后必须跑 --scan。
 ******************************************************************************/
#ifndef JOY_R2D_IMPL_H
#define JOY_R2D_IMPL_H

#include <joy/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

        ///////////////////////////////////////////////////////////////////////
        ////////////////////////////  font ///////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        JOY_API joy_font_p joy_r2d_font_create(const char* filename, int fontsize);
        JOY_API void joy_r2d_font_destroy(joy_font_p font);
        JOY_API float joy_r2d_font_get_scale(joy_font_p font);
        JOY_API int joy_r2d_font_get_size(joy_font_p font);
        JOY_API int joy_r2d_font_get_ascent(joy_font_p font);


        ///////////////////////////////////////////////////////////////////////
        ////////////////////////////  j2rd ///////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        JOY_API joy_r2d_p joy_r2d_create(joy_window_p win);
        JOY_API joy_r2d_p joy_r2d_wrap(void* sdl_renderer);
        JOY_API void joy_r2d_destroy(joy_r2d_p r2d);
        JOY_API void* joy_r2d_unwrap(joy_r2d_p r2d);
        JOY_API joy_vec2_t joy_r2d_coordinates_from_window(joy_r2d_p r2d, float x, float y);

        /* 颜色/状态 */
        JOY_API bool joy_r2d_get_color(joy_r2d_p r2d, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
        JOY_API bool joy_r2d_set_color(joy_r2d_p r2d, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        JOY_API void joy_r2d_begin(joy_r2d_p r2d);
        JOY_API void joy_r2d_end(joy_r2d_p r2d);
        JOY_API bool joy_r2d_set_logical_presentation(joy_r2d_p r2d, int w, int h);
        JOY_API bool joy_r2d_set_logical_presentation_mode(joy_r2d_p r2d, int w, int h, int mode);
        JOY_API bool joy_r2d_get_logical_presentation(joy_r2d_p r2d, int* w, int* h, int* mode);
        JOY_API void joy_r2d_get_size(joy_r2d_p r2d, int* w, int* h);
        JOY_API bool joy_r2d_set_vsync(joy_r2d_p r2d, int vsync);
        JOY_API bool joy_r2d_set_scale(joy_r2d_p r2d, float sx, float sy);
        JOY_API void joy_r2d_set_colorf(joy_r2d_p r2d, float r, float g, float b, float a);
        JOY_API void joy_r2d_get_colorf(joy_r2d_p r2d, float* r, float* g, float* b, float* a);
        JOY_API bool joy_r2d_set_blend_mode(joy_r2d_p r2d, int mode);
        JOY_API bool joy_r2d_draw_point(joy_r2d_p r2d, float x, float y);
        JOY_API bool joy_r2d_draw_points(joy_r2d_p r2d, const joy_vec2_t* points, int count);

        /* 裁剪
         * SDL_Renderer 同时只支持一个裁剪矩形，因此这里额外维护一个栈：
         * push 会与父级矩形求交，pop 恢复上一层。UI 的滚动面板需要它，
         * 否则滚动时内容会绘制到面板外面。
         */
        JOY_API bool joy_r2d_set_clip_rect(joy_r2d_p r2d, joy_rect_t rect);
        JOY_API bool joy_r2d_get_clip_rect(joy_r2d_p r2d, joy_rect_t* out_rect);
        JOY_API void joy_r2d_clear_clip(joy_r2d_p r2d);
        JOY_API bool joy_r2d_push_clip_rect(joy_r2d_p r2d, joy_rect_t rect);
        JOY_API void joy_r2d_pop_clip_rect(joy_r2d_p r2d);
        JOY_API void joy_r2d_reset_clip_stack(joy_r2d_p r2d);
        JOY_API bool joy_r2d_point_in_clip(joy_r2d_p r2d, float x, float y);

        /* 形状绘制 */
        JOY_API bool joy_r2d_shape_draw_line(joy_r2d_p r2d, float x1, float y1, float x2, float y2);
        JOY_API bool joy_r2d_draw_lines(joy_r2d_p r2d, const joy_vec2_t* points, int count);
        JOY_API bool joy_r2d_shape_draw_rectangle(joy_r2d_p r2d, const char* mode, joy_rect_t rect);
        JOY_API bool joy_r2d_shape_draw_polygon(joy_r2d_p r2d, const char* mode, joy_vec2_t* points, int point_count, joy_color_t color);
        JOY_API bool joy_r2d_shape_draw_circle(joy_r2d_p r2d, const char* mode, joy_vec2_t center, float radius, int segments);
        JOY_API bool joy_r2d_shape_draw_grid(joy_r2d_p r2d, joy_vec2_t start, joy_vec2_t end, float grid_size);
        JOY_API bool joy_r2d_shape_draw_gridx(joy_r2d_p r2d, joy_vec2_t position, int rows, int cols, float grid_size);

        /* 图片操作 */
        JOY_API joy_image_p joy_r2d_image_create(joy_r2d_p r2d, const char* filename);
        JOY_API joy_image_p joy_r2d_image_create_from_data(joy_r2d_p r2d, const void* rgba_pixels, int w, int h);
        JOY_API joy_image_p joy_r2d_image_create_empty(joy_r2d_p r2d, int w, int h);
        JOY_API void  joy_r2d_image_destroy(joy_image_p image);
        JOY_API void  joy_r2d_image_draw(joy_image_p image, joy_vec2_t position);
        JOY_API void  joy_r2d_image_draw1(joy_image_p image, const joy_rect_t* srcrect, const joy_rect_t* destrect);
        JOY_API void  joy_r2d_image_draw_ex(joy_image_p image, const joy_rect_t* srcrect, joy_vec2_t position, float rotation, joy_vec2_t scale, joy_vec2_t origin);
        JOY_API void  joy_r2d_image_set_alpha(joy_image_p image, uint8_t alpha);
        JOY_API void  joy_r2d_image_set_blend_mode(joy_image_p image, int mode);
        JOY_API bool  joy_r2d_image_get_size(joy_image_p image, float* w, float* h);
        JOY_API void  joy_r2d_image_update(joy_image_p image, const joy_rect_t* rect, const void* pixels, int pitch);
        JOY_API void  joy_r2d_image_set_scale_mode(joy_image_p image, int nearest);

        /* 文字 */
        JOY_API joy_text_p joy_r2d_text_createx(joy_r2d_p r2d, joy_font_p font, const char* str, int len, joy_color_t color);
        JOY_API joy_text_p joy_r2d_text_create(joy_r2d_p r2d, joy_font_p font, const int* codepoints, int num_codepoints, joy_color_t color);
        JOY_API void joy_r2d_text_updatex(joy_text_p text, joy_font_p font, const char* str, int len, joy_color_t color);
        JOY_API void joy_r2d_text_update(joy_text_p text, joy_font_p font, const int* codepoints, int num_codepoints, joy_color_t color);
        JOY_API void joy_r2d_text_destroy(joy_text_p jtext_tex);
        JOY_API void joy_r2d_text_print(joy_r2d_p r2d, joy_text_p jtext_tex, float x, float y);
        JOY_API void joy_r2d_text_printx(joy_r2d_p r2d, joy_font_p font, const char* str, int len, joy_color_t color, float x, float y);

        /* ==================== 比例坐标（0~1）绘制接口 ====================
         *
         * 语义与 joy_ui_button_create / joy_ui_joystick_create / joy_ui_popup_create 等控件的百分比参数完全一致：
         *   x / w 相对"逻辑分辨率的宽"，y / h 相对"逻辑分辨率的高"。
         *
         * 这样绘制层(shape/image/text)与控件层用同一套坐标：逻辑分辨率变化时
         * 无需改动任何常量（原来绘制层只认像素，分辨率一改就全部挤在左上）。
         *
         * 只新增、不改动上面的像素版接口，既有项目不受影响。
         */
        JOY_API joy_rect_t joy_r2d_frect(joy_r2d_p r2d, float fx, float fy, float fw, float fh);
        JOY_API void    joy_r2d_fpx(joy_r2d_p r2d, float fx, float fy, float fw, float fh,
                                float* x, float* y, float* w, float* h);

        JOY_API bool    joy_r2d_draw_point_f(joy_r2d_p r2d, float fx, float fy);
        JOY_API bool    joy_r2d_shape_draw_rectangle_f(joy_r2d_p r2d, const char* mode,
                                               float fx, float fy, float fw, float fh);
        JOY_API bool    joy_r2d_shape_draw_line_f(joy_r2d_p r2d, float fx1, float fy1, float fx2, float fy2);
        JOY_API void    joy_r2d_image_draw1_f(joy_r2d_p r2d, joy_image_p image, const joy_rect_t* srcrect,
                                      float fx, float fy, float fw, float fh);

        /* 文字位置用比例（字号仍由 font 的烘焙尺寸决定；*_scaled 可再乘一个缩放系数） */
        JOY_API void    joy_r2d_text_printf(joy_r2d_p r2d, joy_font_p font, const char* str, int len,
                                     joy_color_t color, float fx, float fy);
        JOY_API void    joy_r2d_text_printf_scaled(joy_r2d_p r2d, joy_font_p font, const char* str, int len,
                                            joy_color_t color, float fx, float fy, float scale);
        JOY_API int joy_r2d_text_get_width(joy_text_p jtext_tex);
        JOY_API int joy_r2d_text_get_height(joy_text_p jtext_tex);

        /* 文字纹理缓存
         * joy_r2d_text_printx() 以前的实现是 joy_r2d_text_createx() -> joy_r2d_text_print() -> joy_r2d_text_destroy()，
         * 即每次调用都创建并销毁一张 SDL 纹理。inputbox、datagrid、popup、tooltip、
         * combobox_input 等控件都走这条路径，一个带选区的 inputbox 每帧要建 3~4 张纹理。
         * 现在按 (renderer, font, color, 内容) 缓存 LRU 复用，纹理不跨 renderer 串用。
         *
         * 帧时钟每实例独立，sweep 单位为帧；下列 4 个接口作用于全局表（clear 清所有实例）。
         * 注意：这只影响 joy_r2d_text_printx；joy_r2d_text_createx 仍由调用方自行持有与释放。
         */
        JOY_API void joy_r2d_text_cache_clear(void);
        JOY_API void joy_r2d_text_cache_set_limit(int max_entries);
        JOY_API int  joy_r2d_text_cache_get_count(void);
        JOY_API void joy_r2d_text_cache_sweep(int max_idle_frames);

        /* 测量文本尺寸，同样走缓存（命中缓存时不产生任何纹理开销）。
         * joy_r2d_text_printx 不返回宽高，而布局（右对齐、居中、换行）必须能测量。 */
        JOY_API bool joy_r2d_text_measurex(joy_r2d_p r2d, joy_font_p font, const char* str, int len,
                joy_color_t color, int* out_w, int* out_h);

        /* 简单动画 */
        JOY_API joy_animation_p joy_r2d_animation_create(joy_r2d_p r2d);
        JOY_API void joy_r2d_animation_destroy(joy_animation_p animation);
        JOY_API void joy_r2d_animation_reset(joy_animation_p animation);
        JOY_API void joy_r2d_animation_add_clip(joy_animation_p animation, const char* image_path, float duration, joy_rect_t src_rect);
        JOY_API bool joy_r2d_animation_is_finished(joy_animation_p animation);
        JOY_API void joy_r2d_animation_set_position(joy_animation_p animation, float x, float y);
        JOY_API void joy_r2d_animation_set_scale(joy_animation_p animation, float x, float y);
        JOY_API void joy_r2d_animation_set_rotation(joy_animation_p animation, float rotation);
        JOY_API void joy_r2d_animation_update(joy_animation_p animation, float dt);
        JOY_API void joy_r2d_animation_draw(joy_animation_p animation, joy_rect_t* camera);
        /* 2D 视野绘制: 在像素平面构造 A/B/C 三环羽化多边形, 复刻 3D 的渐变纹理软边。
           px/py 为观察点在输出像素坐标, radii[seg] 每方向可见半径(像素),
           feather/outer_radius 亦为像素。需在 joy_r2d_begin/joy_r2d_end 与地图同一坐标系(相机相对像素)下调用。
           射线遮挡仍用上方的 joy_r3d_fov_compute_radii 计算(其 px/pz 即 2D 的 x/y。若担心模板冲突,
           2D 侧也可直接以世界米为单位计算后乘以 PPM 得到像素半径)。 */
        JOY_API void joy_r2d_fov_draw_2d(joy_r2d_p r, float px, float py,
                const float* radii, int seg, float feather, float outer_radius);


        /* ==================== Spine 骨骼动画（jspine，可选模块） ====================
         * 启用方式：定义 JOY_USE_SPINE 并链接 spine-c，见 joy_add_sources(<target> SPINE)。
         * 分两层，便于 ECS 使用：
         *   joy_spine_asset_t    —— 共享资源（atlas + skeletonData），多实体复用，由资源管理器持有
         *   joy_spine_instance_t —— 每实体一份的"组件"，纯数据结构，可直接 emplace 到 ECS
         */
#ifdef JOY_USE_SPINE
#include <spine/spine.h>

        typedef struct joy_spine_asset joy_spine_asset_t, * joy_spine_asset_p;

#define JSPINE_NAME_MAX 64

        /* ECS 组件：ecs.emplace<joy_spine_instance_t>(e) 后调用 joy_r2d_spine_instance_init，
         * 销毁前必须调用 joy_r2d_spine_instance_free（不会释放 asset）。 */
        typedef struct joy_spine_instance {
                joy_spine_asset_p asset;              /* 共享资源（不拥有） */
                spSkeleton *skeleton;              /* 拥有的实例数据 */
                spAnimationState *state;
                spSkeletonClipping *clipper;
                void *draw_cache;                  /* 内部顶点/索引缓存（不透明，避免暴露 SDL 类型） */

                float x, y;                        /* 位置（y 向下：模块内已 spBone_setYDown） */
                float scale_x, scale_y;            /* 缩放 */
                float rotation;                    /* 整体旋转（度） */
                float time_scale;                  /* 播放速度倍率 */
                float alpha;                       /* 整体透明度 0~1 */
                joy_color_t tint;                     /* 染色（与贴图颜色相乘） */
                bool flip_x;                       /* 水平翻转 */
                bool visible;                      /* 是否参与绘制 */
                int zorder;                        /* 绘制排序（system 侧使用） */
                char animation[JSPINE_NAME_MAX];   /* 当前动画名 */
        } joy_spine_instance_t, * joy_spine_instance_p;

        /* ---- 共享资源 ---- */
        JOY_API joy_spine_asset_p joy_r2d_spine_asset_load(joy_r2d_p r2d, const char *atlas_path,
                                                 const char *skeleton_path, float scale);
        JOY_API void joy_r2d_spine_asset_free(joy_spine_asset_p asset);
        JOY_API spSkeletonData *joy_r2d_spine_asset_data(joy_spine_asset_p asset);
        JOY_API int joy_r2d_spine_asset_animation_count(joy_spine_asset_p asset);
        JOY_API const char *joy_r2d_spine_asset_animation_name(joy_spine_asset_p asset, int index);
        JOY_API float joy_r2d_spine_asset_animation_duration(joy_spine_asset_p asset, int index);

        /* ---- 组件（实例） ---- */
        JOY_API bool joy_r2d_spine_instance_init(joy_spine_instance_p inst, joy_spine_asset_p asset);
        JOY_API void joy_r2d_spine_instance_free(joy_spine_instance_p inst);
        JOY_API void joy_r2d_spine_instance_reset(joy_spine_instance_p inst);
        JOY_API bool joy_r2d_spine_instance_set_animation(joy_spine_instance_p inst, int track,
                                                   const char *name, bool loop);
        JOY_API bool joy_r2d_spine_instance_add_animation(joy_spine_instance_p inst, int track,
                                                   const char *name, bool loop, float delay);
        /* update = 同步字段 + 推进动画 + 世界变换；
         * sync  = 只同步字段与世界变换（暂停、或改完字段想立刻生效时用） */
        JOY_API void joy_r2d_spine_instance_update(joy_spine_instance_p inst, float dt);
        JOY_API void joy_r2d_spine_instance_sync(joy_spine_instance_p inst);
        JOY_API void joy_r2d_spine_instance_draw(joy_r2d_p r2d, joy_spine_instance_p inst);

        /* 变换/状态：写组件字段，下一次 update 生效 */
        JOY_API void joy_r2d_spine_instance_set_position(joy_spine_instance_p inst, float x, float y);
        JOY_API void joy_r2d_spine_instance_set_scale(joy_spine_instance_p inst, float sx, float sy);
        JOY_API void joy_r2d_spine_instance_set_rotation(joy_spine_instance_p inst, float degrees);
        JOY_API void joy_r2d_spine_instance_set_flip_x(joy_spine_instance_p inst, bool flip);
        JOY_API void joy_r2d_spine_instance_set_alpha(joy_spine_instance_p inst, float alpha);
        JOY_API void joy_r2d_spine_instance_set_tint(joy_spine_instance_p inst, joy_color_t tint);

        /* 包围盒（y 向下坐标系）：便于 ECS 做视口剔除/碰撞粗筛 */
        JOY_API bool joy_r2d_spine_instance_bounds(joy_spine_instance_p inst, float *out_x, float *out_y,
                                            float *out_w, float *out_h);
        JOY_API const char *joy_r2d_spine_instance_animation(joy_spine_instance_p inst);
#endif /* JOY_USE_SPINE */


        JOY_API joy_particle_cfg_t joy_r2d_particle_cfg_default(void);
        JOY_API joy_particle_p joy_r2d_particle_create(joy_particle_cfg_t cfg);
        JOY_API void joy_r2d_particle_destroy(joy_particle_p p);
        JOY_API void joy_r2d_particle_set_position(joy_particle_p p, float x, float y);
        JOY_API void joy_r2d_particle_emit(joy_particle_p p);            /* 爆发一次 */
        JOY_API void joy_r2d_particle_clear(joy_particle_p p);            /* 清除所有粒子 */
        JOY_API void joy_r2d_particle_update(joy_particle_p p, float dt);
        JOY_API void joy_r2d_particle_render(joy_particle_p p, joy_r2d_p r2d);
        JOY_API int  joy_r2d_particle_count(joy_particle_p p);
        JOY_API bool joy_r2d_particle_is_active(joy_particle_p p);

        /* 生成链路专用：joy_r2d_image_draw1 去掉 joy_rect_t* 指针后的全标量版本。
         * srcrect 固定 NULL（整张图），目标矩形摊成 4 个 float。joy_value
         * 表达不了 joy_rect_t*，L3 的 joy_r2d_image_draw1 无法自动生成 thunk；这条 L1
         * 让四个后端都能画贴图，且参数保持编译期强类型。
         *
         * 坐标是【像素】，不是归一化比例：不要走 joy_r2d_frect / joy_r2d_image_draw1_f，
         * 那些 *_f 系列会乘窗口宽高，把 x=227 放大成 108960。命名用 _rect
         * 而不是 _f，就是为了和归一化系列区分开。
         * 声明刻意放在文件末尾，避免打乱 joy.idl 既有条目的 source 行号。 */
        JOY_API void joy_r2d_image_draw_rect(joy_r2d_p r2d, joy_image_p image,
                                     float x, float y, float w, float h);

#ifdef __cplusplus
}
#endif

#endif /* JOY_R2D_IMPL_H */
