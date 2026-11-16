/******************************************************************************
 * src/jr2d.h —— r2d 域声明头（jr2d.c 的配套头）
 * 职责：2D 渲染 / 字体 / 粒子 / spine / 2D 控件与即时模式 UI
 * 边界：原 ui 域已按渲染上下文并入 r2d，218 条 joy_ui_* 统一为 joy_r2d_*；
 *       3D 侧 HUD 控件同理归 r3d，域边界按渲染上下文划分，不按「控件」概念划分。
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
         * 语义与 joy_r2d_button_create / joy_r2d_joystick_create / joy_r2d_popup_create 等控件的百分比参数完全一致：
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


        /* ==================== Spine 骨骼动画（jspine，无条件编译） ====================
         * spine-c 源码随仓库分发，joy::all 无条件链接；不再有 JOY_ENABLE_SPINE 开关。
         * 分两层，便于 ECS 使用：
         *   joy_spine_asset_t    —— 共享资源（atlas + skeletonData），多实体复用，由资源管理器持有
         *   joy_spine_instance_t —— 每实体一份的"组件"，纯数据结构，可直接 emplace 到 ECS
         */
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


        /* ============ 2D 控件 / 即时模式 UI（原 ui 域声明，已按渲染上下文并入 r2d） ============ */
        /* 边界：ui 不再作为独立域；依赖 joy_r2d_p 的控件归 r2d，依赖 joy_r3d_p 的 HUD 归 r3d。 */

        JOY_API joy_label_p joy_r2d_label_create(joy_r2d_p r2d, joy_font_p fontinfo, const int* codepoints, int len);
        JOY_API void joy_r2d_label_destroy(joy_label_p label);
        JOY_API void joy_r2d_label_set_text(joy_label_p label, joy_font_p fontinfo, const int* codepoints, int len, joy_color_t color);
        JOY_API void joy_r2d_label_draw(joy_label_p label);
        JOY_API void joy_r2d_label_process_event(joy_label_p label, const void* event);
        JOY_API joy_button_p joy_r2d_button_create(joy_r2d_p r2d, float cx, float cy, float wf, float hf);
        JOY_API void joy_r2d_button_destroy(joy_button_p btn);
        JOY_API void joy_r2d_button_set_position(joy_button_p btn, float x, float y);
        JOY_API joy_vec2_t joy_r2d_button_get_position(joy_button_p btn);
        /* 布局需要：复用同一个 button 时改变尺寸（旧 API 只能在 create 时定尺寸） */
        JOY_API void joy_r2d_button_set_size(joy_button_p btn, float width, float height);
        JOY_API void joy_r2d_button_set_normal_color(joy_button_p btn, joy_color_t color);
        JOY_API void joy_r2d_button_set_hover_color(joy_button_p btn, joy_color_t color);
        JOY_API void joy_r2d_button_set_pressed_color(joy_button_p btn, joy_color_t color);
        JOY_API void joy_r2d_button_set_image(joy_button_p btn, joy_image_p image);
        JOY_API void joy_r2d_button_set_pressed_image(joy_button_p btn, joy_image_p image);
        JOY_API void joy_r2d_button_set_textx(joy_button_p btn, joy_font_p fontinfo, const char* str, int len, joy_color_t color);
        JOY_API void joy_r2d_button_set_text(joy_button_p btn, joy_font_p fontinfo, const int* codepoints, int len, joy_color_t color);
        JOY_API bool joy_r2d_button_is_hovered(joy_button_p btn, float x, float y);
        JOY_API bool joy_r2d_button_is_pressed(joy_button_p btn, float x, float y);
        JOY_API void joy_r2d_button_set_hovered(joy_button_p btn, bool v);
        JOY_API void joy_r2d_button_set_pressed(joy_button_p btn, bool v);
        JOY_API bool joy_r2d_button_get_pressed(joy_button_p btn);
        JOY_API void joy_r2d_button_set_text_offset(joy_button_p btn, float x, float y);
        JOY_API void joy_r2d_button_draw(joy_button_p btn);
        JOY_API void joy_r2d_button_process_event(joy_button_p btn, const void* event);

        /* ============ 滚动层 scrollview（可拖拽滚动的容器）============
         * 横向 + 纵向滚动；鼠标拖拽 / 触摸滑动 / 滚轮都支持（触摸在 SDL 里也走鼠标事件）。
         * 与保留模式子控件（button/inputbox 等）配合的典型用法：
         *
         *   joy_scrollview_p v = joy_r2d_scrollview_create(r2d, fx, fy, fw, fh);  // 归一化视口
         *   joy_r2d_scrollview_set_content(v, 内容总宽, 内容总高);
         *
         *   // ① 事件：先问滚动层。返回 true = 本次手势是滚动，调用方应忽略点击
         *   //   （否则"滑动一下"会误触发子控件；故点击判定建议放到松手，见下）
         *   if (joy_r2d_scrollview_process_event(v, ev)) return;   // 已作为滚动消费
         *   ...你原有的点击/悬停逻辑...
         *
         *   // ② 每帧绘制：begin/end 之间画子控件，超出视口的部分被裁掉
         *   joy_r2d_scrollview_begin(v);
         *   for (i) joy_r2d_button_set_position(child[i],
         *               base_x[i] + joy_r2d_scrollview_get_offset_x(v),
         *               base_y[i] + joy_r2d_scrollview_get_offset_y(v));
         *   for (i) joy_r2d_button_draw(child[i]);
         *   joy_r2d_scrollview_end(v);      // 恢复裁剪 + 画滚动条
         *
         * 偏移恒 ≤0（子控件整体向左/上移 = 用户向右/下看），并夹紧在内容范围内；
         * 内容不大于视口时偏移恒 0、不画滚动条、拖拽与滚轮均无效。
         */
        JOY_API joy_scrollview_p joy_r2d_scrollview_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh);
        JOY_API void  joy_r2d_scrollview_destroy(joy_scrollview_p v);
        JOY_API void  joy_r2d_scrollview_set_view_px(joy_scrollview_p v, float x, float y, float w, float h);
        JOY_API void  joy_r2d_scrollview_set_content(joy_scrollview_p v, float content_w, float content_h);
        JOY_API void  joy_r2d_scrollview_set_offset(joy_scrollview_p v, float ox, float oy);
        JOY_API void  joy_r2d_scrollview_clamp(joy_scrollview_p v);
        JOY_API float joy_r2d_scrollview_get_offset_x(joy_scrollview_p v);
        JOY_API float joy_r2d_scrollview_get_offset_y(joy_scrollview_p v);
        JOY_API void  joy_r2d_scrollview_set_show_bar(joy_scrollview_p v, bool show);
        JOY_API bool  joy_r2d_scrollview_dragging(joy_scrollview_p v);
        JOY_API bool  joy_r2d_scrollview_hit(joy_scrollview_p v, float win_x, float win_y);
        JOY_API void  joy_r2d_scrollview_begin(joy_scrollview_p v);
        JOY_API void  joy_r2d_scrollview_end(joy_scrollview_p v);
        /* 返回 true = 该事件已被滚动层消费（拖拽中/滑动结束/滚轮），调用方不要再当点击 */
        JOY_API bool  joy_r2d_scrollview_process_event(joy_scrollview_p v, const void* event);
        JOY_API joy_combobox_p joy_r2d_combobox_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh);
        JOY_API void joy_r2d_combobox_destroy(joy_combobox_p combobox);
        JOY_API void joy_r2d_combobox_set_text(joy_combobox_p combobox, joy_font_p font,
                const int* codepoints, int len, joy_color_t color);
        JOY_API void joy_r2d_combobox_add_item(joy_combobox_p combobox, joy_font_p font,
                const char* str, int len, joy_color_t color);
        JOY_API void joy_r2d_combobox_add_itemx(joy_combobox_p combobox, joy_font_p font,
                const int* codepoints, int len, joy_color_t color);

        JOY_API bool joy_r2d_combobox_is_pressed(joy_combobox_p combobox, float x, float y);
        JOY_API bool joy_r2d_combobox_is_open(joy_combobox_p combobox);
        JOY_API void joy_r2d_combobox_set_open(joy_combobox_p combobox, bool v);
        JOY_API int joy_r2d_combobox_hit_item(joy_combobox_p combobox, float x, float y);
        JOY_API void joy_r2d_combobox_set_selected(joy_combobox_p combobox, int index);
        JOY_API int joy_r2d_combobox_get_selected(joy_combobox_p combobox);
        JOY_API void joy_r2d_combobox_draw(joy_combobox_p combobox);
        JOY_API void joy_r2d_combobox_process_event(joy_combobox_p cb, const void* event);

        /* ===== 栅格布局 jlayout（纯几何计算 + 可选标签绘制，不持有任何控件） =====
         * 场景按“逻辑像素”创建栅格：ox/oy/width 由逻辑宽计算，row_h 由高度基准 s 计算。
         * 每行：用 joy_r2d_layout_grid_label() 在左列画标签、用 joy_r2d_layout_grid_cell() 取右列矩形，
         * 再用各控件 *_create(r2d, px/LW, py/LH, pw/LW, ph/LH, ...) 创建控件。
         * col_fracs 为各列宽占比（和≈1），pad 为单元格内边距（像素）。 */
        JOY_API joy_layout_grid_t* joy_r2d_layout_grid_create(float ox, float oy, float width, float row_h,
                int ncols, const float* col_fracs, float pad);
        JOY_API void joy_r2d_layout_grid_destroy(joy_layout_grid_t* g);
        JOY_API void joy_r2d_layout_grid_next_row(joy_layout_grid_t* g);
        JOY_API void joy_r2d_layout_grid_set_row(joy_layout_grid_t* g, int row);
        /* 取第 col 列的像素矩形（已含内边距）。x/y/w/h 可为 NULL。 */
        JOY_API void joy_r2d_layout_grid_cell(joy_layout_grid_t* g, int col, float* x, float* y, float* w, float* h);
        /* 在第 col 列画左对齐、垂直居中的标签，并返回该列像素矩形（cx/cy/cw/ch 可为 NULL）。 */
        JOY_API void joy_r2d_layout_grid_label(joy_layout_grid_t* g, joy_r2d_p r2d, joy_font_p font,
                const char* text, int n, joy_color_t c, int col,
                float* cx, float* cy, float* cw, float* ch);
        /* 在当前行创建 n 个等宽按钮（横向按钮条/菜单），各占一列；
         * 按钮指针写入 out[]（调用方分配 n 个），创建后自动换到下一行。 */
        JOY_API void joy_r2d_layout_grid_button_row(joy_layout_grid_t* g, joy_r2d_p r2d, joy_font_p font,
                const char** labels, int n, joy_button_p* out);

        // 可输入下拉列表框
        JOY_API joy_combobox_input_p joy_r2d_combobox_input_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, joy_font_p font, int buf_len);
        JOY_API void joy_r2d_combobox_input_destroy(joy_combobox_input_p ci);
        JOY_API void joy_r2d_combobox_input_add_item(joy_combobox_input_p ci, const char* str, int len);
        JOY_API void joy_r2d_combobox_input_clear_items(joy_combobox_input_p ci);
        JOY_API const char* joy_r2d_combobox_input_get_text(joy_combobox_input_p ci);
        JOY_API void joy_r2d_combobox_input_set_text(joy_combobox_input_p ci, const char* str);
        JOY_API void joy_r2d_combobox_input_draw(joy_combobox_input_p ci);
        JOY_API void joy_r2d_combobox_input_draw_dropdown(joy_combobox_input_p ci);
        JOY_API bool joy_r2d_combobox_input_is_open(joy_combobox_input_p ci);
        JOY_API void joy_r2d_combobox_input_set_popup_node(joy_combobox_input_p ci, joy_scene_node_p node);
        JOY_API void joy_r2d_combobox_input_process_event(joy_combobox_input_p ci, const void* ev);
        JOY_API int joy_r2d_combobox_input_hit_item(joy_combobox_input_p ci, float x, float y);
        /* 主动展开/收起下拉列表（原来只有 is_open，选中某项后无法收起）。 */
        JOY_API void joy_r2d_combobox_input_set_open(joy_combobox_input_p ci, bool v);

        JOY_API joy_datagrid_p joy_r2d_datagrid_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, int col_count, bool has_header, void* font);
        JOY_API void joy_r2d_datagrid_setheaders(joy_datagrid_p grid, const char* headers);
        JOY_API void joy_r2d_datagrid_setheader(joy_datagrid_p grid, int index, const char* header);
        JOY_API void joy_r2d_datagrid_addrow(joy_datagrid_p grid, const char** row_data);
        JOY_API void joy_r2d_datagrid_setcell(joy_datagrid_p grid, int row_index, int col_index, const char* data);
        JOY_API void joy_r2d_datagrid_destroy(joy_datagrid_p grid);
        JOY_API void joy_r2d_datagrid_draw(joy_datagrid_p grid, joy_r2d_p r2d);
        JOY_API int joy_r2d_datagrid_hit_row(joy_datagrid_p grid, float x, float y);
        JOY_API void joy_r2d_datagrid_scroll(joy_datagrid_p grid, int lines);
        JOY_API void joy_r2d_datagrid_set_selected_row(joy_datagrid_p grid, int row);
        JOY_API void joy_r2d_datagrid_process_event(joy_datagrid_p dg, const void* event);
        JOY_API joy_radio_p joy_r2d_radio_create(float fx, float fy, float size_frac, const char* label, int group_id);
        JOY_API void joy_r2d_radio_destroy(joy_radio_p radio);
        JOY_API bool joy_r2d_radio_is_pressed(joy_radio_p radio, float x, float y);
        JOY_API void joy_r2d_radio_set_hovered(joy_radio_p radio, bool v);
        JOY_API void joy_r2d_radio_process_event(joy_radio_p rb, const void* event);
        JOY_API joy_checkbox_p joy_r2d_checkbox_create(float fx, float fy, float size_frac, const char* label);
        JOY_API void joy_r2d_checkbox_destroy(joy_checkbox_p checkbox);
        JOY_API void joy_r2d_checkbox_set_state(joy_checkbox_p checkbox, joy_checkbox_state_t state);
        JOY_API joy_checkbox_state_t joy_r2d_checkbox_get_state(joy_checkbox_p checkbox);
        JOY_API void joy_r2d_checkbox_set_colors(joy_checkbox_p checkbox, joy_color_t box, joy_color_t check, joy_color_t hover);
        JOY_API void joy_r2d_checkbox_set_r2d(joy_checkbox_p checkbox, joy_r2d_p r2d);
        /* 布局需要：移动复选框（旧 API 只能在 create 时定位置） */
        JOY_API void joy_r2d_checkbox_set_position(joy_checkbox_p checkbox, int x, int y);
        JOY_API void joy_r2d_checkbox_set_text_color(joy_checkbox_p checkbox, joy_color_t color);
        JOY_API void joy_r2d_checkbox_toggle(joy_checkbox_p checkbox);
        JOY_API bool joy_r2d_checkbox_is_pressed(joy_checkbox_p cb, float x, float y);
        JOY_API void joy_r2d_checkbox_set_hovered(joy_checkbox_p cb, bool v);
        JOY_API void joy_r2d_checkbox_draw(joy_checkbox_p cb, joy_r2d_p r2d, void* font);
        JOY_API void joy_r2d_checkbox_process_event(joy_checkbox_p cb, const void* event);
        JOY_API joy_joystick_p joy_r2d_joystick_create(joy_r2d_p r2d, float cx, float cy, float radius_frac);
        // 摇杆方向模式：约束/量化 joy_r2d_joystick_get_direction 输出的方向
        typedef enum {
                JOYSTICK_DIR_OMNI = 0,       // 全方向（模拟，默认）
                JOYSTICK_DIR_4WAY,           // 上/下/左/右 四向
                JOYSTICK_DIR_8WAY,           // 四向 + 四个斜角（八向）
                JOYSTICK_DIR_VERTICAL,       // 仅 上/下
                JOYSTICK_DIR_HORIZONTAL,     // 仅 左/右
        } joy_joystick_dir_mode;

        JOY_API void joy_r2d_joystick_destroy(joy_joystick_p joystick);
        JOY_API void joy_r2d_joystick_set_textures(joy_joystick_p joystick, const char* base_path, const char* thumb_path);
        JOY_API void joy_r2d_joystick_set_dir_mode(joy_joystick_p joystick, joy_joystick_dir_mode mode);
        JOY_API void joy_r2d_joystick_set_vector(joy_joystick_p joystick, float dx, float dy);
        JOY_API void joy_r2d_joystick_set_textures_for_mode(joy_joystick_p joystick, joy_joystick_dir_mode mode, const char* base_path, const char* thumb_path);
        JOY_API void joy_r2d_joystick_draw(joy_joystick_p joystick);
        JOY_API void joy_r2d_joystick_reset(joy_joystick_p joystick);
        JOY_API void joy_r2d_joystick_set_position(joy_joystick_p joystick, float x, float y);
        /* 布局需要：逻辑分辨率变化（自适应宽高比/旋转）时改变摇杆半径，保持设计尺寸 */
        JOY_API void joy_r2d_joystick_set_radius(joy_joystick_p joystick, float radius);
        JOY_API bool joy_r2d_joystick_is_dragging(joy_joystick_p joystick);
        JOY_API void joy_r2d_joystick_touch_down(joy_joystick_p joystick, float x, float y);
        JOY_API void joy_r2d_joystick_touch_move(joy_joystick_p joystick, float x, float y);
        JOY_API void joy_r2d_joystick_touch_up(joy_joystick_p joystick);
        JOY_API joy_vec2_t joy_r2d_joystick_get_direction(joy_joystick_p joystick);
        JOY_API float joy_r2d_joystick_get_magnitude(joy_joystick_p joystick);
        JOY_API void joy_r2d_joystick_process_event(joy_joystick_p js, const void* event);
        JOY_API joy_inputbox_p joy_r2d_inputbox_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, joy_font_p font);
        JOY_API void joy_r2d_inputbox_destroy(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_set_text(joy_inputbox_p ti, const char* text);
        JOY_API const char* joy_r2d_inputbox_get_text(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_set_callback(joy_inputbox_p ti, void (*submit)(joy_inputbox_p, void*), void (*change)(joy_inputbox_p, void*), void* userdata);
        JOY_API void joy_r2d_inputbox_draw(joy_inputbox_p ti);
        JOY_API bool joy_r2d_inputbox_is_pressed(joy_inputbox_p ti, float x, float y);
        JOY_API bool joy_r2d_inputbox_is_hovered(joy_inputbox_p ti, float x, float y);
        JOY_API void joy_r2d_inputbox_focus(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_defocus(joy_inputbox_p ti);
        /* 仅清除焦点，不调用 SDL_StopTextInput。用于多输入框之间切换焦点时保持软键盘不收起，
         * 避免 Android IME hide/show 同帧竞态导致新的输入框唤不起键盘。 */
        JOY_API void joy_r2d_inputbox_blur(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_insert(joy_inputbox_p ti, const char* s, int len);
        JOY_API void joy_r2d_inputbox_backspace(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_delete(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_cursor_left(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_cursor_right(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_cursor_home(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_cursor_end(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_select_left(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_select_right(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_select_home(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_select_end(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_select_all(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_paste(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_copy(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_cut(joy_inputbox_p ti);
        JOY_API bool joy_r2d_inputbox_is_focused(joy_inputbox_p ti);
        JOY_API void joy_r2d_inputbox_process_event(joy_inputbox_p ti, const void* event);
        JOY_API joy_slider_p joy_r2d_slider_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, float min, float max, float initial);
        JOY_API void joy_r2d_slider_destroy(joy_slider_p s);
        JOY_API void joy_r2d_slider_set_value(joy_slider_p s, float v);
        JOY_API float joy_r2d_slider_get_value(joy_slider_p s);
        JOY_API void joy_r2d_slider_set_colors(joy_slider_p s, joy_color_t track, joy_color_t fill, joy_color_t thumb, joy_color_t thumb_hover);
        /* 布局需要：移动/缩放滑轨（旧 API 只能在 create 时定位置与尺寸） */
        JOY_API void joy_r2d_slider_set_rect(joy_slider_p s, float x, float y, float w, float h);
        JOY_API void joy_r2d_slider_set_range(joy_slider_p s, float min, float max);
        JOY_API void joy_r2d_slider_draw(joy_slider_p s);
        JOY_API bool joy_r2d_slider_is_pressed(joy_slider_p s, float x, float y);
        JOY_API void joy_r2d_slider_set_pressed(joy_slider_p s, bool v);
        JOY_API void joy_r2d_slider_drag_move(joy_slider_p s, float x, float y);
        JOY_API bool joy_r2d_slider_is_hovered(joy_slider_p s, float x, float y);
        JOY_API void joy_r2d_slider_set_hovered(joy_slider_p s, bool v);
        JOY_API void joy_r2d_slider_process_event(joy_slider_p s, const void* event);
        JOY_API joy_popup_p joy_r2d_popup_create(joy_r2d_p r2d, float fw, float fh);
        JOY_API void joy_r2d_popup_destroy(joy_popup_p p);
        JOY_API void joy_r2d_popup_set_title(joy_popup_p p, const char* text, int len);
        JOY_API void joy_r2d_popup_set_content(joy_popup_p p, const char* text, int len);
        JOY_API void joy_r2d_popup_set_position(joy_popup_p p, float x, float y);
        JOY_API void joy_r2d_popup_set_visible(joy_popup_p p, bool v);
        JOY_API void joy_r2d_popup_set_closeable(joy_popup_p p, bool v);
        JOY_API void joy_r2d_popup_draw(joy_popup_p p, joy_font_p title_font, joy_font_p content_font);
        JOY_API bool joy_r2d_popup_is_close_clicked(joy_popup_p p, float mx, float my);
        JOY_API bool joy_r2d_popup_is_title_clicked(joy_popup_p p, float mx, float my);
        JOY_API void joy_r2d_popup_drag_move(joy_popup_p p, float mx, float my);
        JOY_API bool joy_r2d_popup_is_inside(joy_popup_p p, float mx, float my);
        JOY_API void joy_r2d_popup_process_event(joy_popup_p p, const void* event);
        JOY_API joy_tooltip_p joy_r2d_tooltip_create(joy_r2d_p r2d, const char* text, int len);
        JOY_API void joy_r2d_tooltip_destroy(joy_tooltip_p t);
        JOY_API void joy_r2d_tooltip_set_text(joy_tooltip_p t, const char* text, int len);
        JOY_API void joy_r2d_tooltip_set_position(joy_tooltip_p t, float x, float y);
        JOY_API void joy_r2d_tooltip_set_visible(joy_tooltip_p t, bool v);
        JOY_API void joy_r2d_tooltip_draw(joy_tooltip_p t, joy_font_p font);
        JOY_API void joy_r2d_tooltip_process_event(joy_tooltip_p t, const void* event);
        JOY_API void joy_r2d_process_widget_event(void** widgets, int count, const void* event);

        /* ======================================================================
         * 即时模式 UI 层（jui_im）
         *
         * jui 原有控件是 retained 模式：create / set / draw / process_event 分离，
         * 适合固定布局的游戏 HUD。但层级树、控制台、资源列表这类**行数动态变化**
         * 的界面，retained 需要手工管理每一行的创建与销毁。
         *
         * 这层提供即时模式写法：调用方每帧只描述“这里有个按钮”，控件实例由 ctx
         * 按 id 池化复用——既保留了 button 的颜色过渡动画、slider 的拖拽状态，
         * 又不必由调用方管理生命周期。
         *
         * 关键依赖：joy_r2d_push_clip_rect / joy_r2d_pop_clip_rect。
         * jui 原本完全没有裁剪，而滚动面板是编辑器的基础，缺了它内容会溢出面板。
         * ====================================================================== */

        typedef struct joy_ui_ctx joy_ui_ctx_t, * joy_ui_ctx_p;

        /* 即时模式层用到的配色 */
        typedef struct joy_ui_theme {
                joy_color_t text;
                joy_color_t btn_normal, btn_hover, btn_pressed, btn_text;
                joy_color_t cb_box, cb_check, cb_hover, cb_text;
                joy_color_t sl_track, sl_fill, sl_thumb, sl_thumb_hover;
                joy_color_t panel, panel_title, border;
        } joy_ui_theme_t;

        JOY_API joy_ui_ctx_p joy_r2d_ctx_create(joy_r2d_p r2d, joy_font_p font);
        JOY_API void      joy_r2d_ctx_destroy(joy_ui_ctx_p ctx);
        JOY_API void      joy_r2d_set_theme(joy_ui_ctx_p ctx, const joy_ui_theme_t* theme);

        /* 每帧开始 / 结束。begin 会转存一次性输入（按下/抬起/滚轮），end 清零。 */
        JOY_API void joy_r2d_im_begin(joy_ui_ctx_p ctx);
        JOY_API void joy_r2d_im_end(joy_ui_ctx_p ctx);

        /* 输入：从 SDL 事件喂进来。坐标为**窗口坐标**，内部转成逻辑坐标。 */
        JOY_API void joy_r2d_input_motion(joy_ui_ctx_p ctx, float win_x, float win_y);
        JOY_API void joy_r2d_input_down(joy_ui_ctx_p ctx, int button);
        JOY_API void joy_r2d_input_up(joy_ui_ctx_p ctx, int button);
        JOY_API void joy_r2d_input_wheel(joy_ui_ctx_p ctx, float dy);
        JOY_API void joy_r2d_input_key(joy_ui_ctx_p ctx, bool shift, bool ctrl);

        /* 逻辑鼠标坐标（供调用方自行做命中判断，例如视口拾取） */
        JOY_API void  joy_r2d_get_mouse(joy_ui_ctx_p ctx, float* out_x, float* out_y);
        JOY_API bool  joy_r2d_is_down(joy_ui_ctx_p ctx);
        JOY_API bool  joy_r2d_is_pressed(joy_ui_ctx_p ctx);
        JOY_API bool  joy_r2d_is_released(joy_ui_ctx_p ctx);
        JOY_API float joy_r2d_take_wheel(joy_ui_ctx_p ctx);   /* 取走并标记已消费 */
        JOY_API bool  joy_r2d_is_shift(joy_ui_ctx_p ctx);
        JOY_API bool  joy_r2d_is_ctrl(joy_ui_ctx_p ctx);

        /* 裁剪：内部委托 r2d_push/pop_clip_rect，同时记录当前矩形用于命中测试 */
        JOY_API void joy_r2d_push_clip(joy_ui_ctx_p ctx, joy_rect_t rect);
        JOY_API void joy_r2d_pop_clip(joy_ui_ctx_p ctx);
        JOY_API void joy_r2d_reset_clip(joy_ui_ctx_p ctx);
        /* 命中测试：既要在矩形内，也要在当前裁剪区内（否则滚动区域外会误触） */
        JOY_API bool joy_r2d_hit(joy_ui_ctx_p ctx, joy_rect_t rect);

        /* 绘制辅助 */
        JOY_API void joy_r2d_fill_rect(joy_ui_ctx_p ctx, joy_rect_t r, joy_color_t c);
        JOY_API void joy_r2d_stroke_rect(joy_ui_ctx_p ctx, joy_rect_t r, joy_color_t c);
        JOY_API void joy_r2d_line(joy_ui_ctx_p ctx, float x1, float y1, float x2, float y2, joy_color_t c);
        JOY_API int  joy_r2d_text(joy_ui_ctx_p ctx, float x, float y, const char* s, joy_color_t c);
        JOY_API bool joy_r2d_text_measure(joy_ui_ctx_p ctx, const char* s, joy_color_t c, int* out_w, int* out_h);
        JOY_API int  joy_r2d_text_centered(joy_ui_ctx_p ctx, joy_rect_t r, const char* s, joy_color_t c);

        /* 面板（标题栏 + 内容裁剪区）*/
        JOY_API void joy_r2d_panel_begin(joy_ui_ctx_p ctx, joy_rect_t r, const char* title);
        JOY_API void joy_r2d_panel_end(joy_ui_ctx_p ctx);
        JOY_API void joy_r2d_panel_frame(joy_ui_ctx_p ctx, joy_rect_t r);

        /* 滚动：id 用于在 ctx 内保存各自的滚动偏移 */
        JOY_API float joy_r2d_scroll_begin(joy_ui_ctx_p ctx, int id, joy_rect_t view, float content_h);
        JOY_API void  joy_r2d_scroll_end(joy_ui_ctx_p ctx);
        JOY_API void  joy_r2d_scrollbar(joy_ui_ctx_p ctx, joy_rect_t view, float content_h, float off);
        /* 强制设置滚动偏移（控制台贴底跟随用）；content_h 传 0 时仅设偏移 */
        JOY_API void  joy_r2d_scroll_set_offset(joy_ui_ctx_p ctx, int id, float off, float content_h, float view_h);

        /* 控件：复用 button / checkbox / slider，按 id 池化 */
        JOY_API bool joy_r2d_button(joy_ui_ctx_p ctx, int id, joy_rect_t r, const char* text);
        JOY_API bool joy_r2d_checkbox(joy_ui_ctx_p ctx, int id, joy_rect_t r, bool* value, const char* label);
        /* rect 是滑轨区域；标签与数值文本由调用方自行绘制（格式各异） */
        JOY_API bool joy_r2d_slider(joy_ui_ctx_p ctx, int id, joy_rect_t track, float* value, float minv, float maxv);

#ifdef __cplusplus
}
#endif

#endif /* JOY_R2D_IMPL_H */
