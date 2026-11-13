/******************************************************************************
 * src/joy_r3d.h —— r3d 域声明头（joy_r3d.c 的配套头）
 * 职责：3D 渲染 / 光照 / glTF
 * 方向：本头是该域声明的归属地，由 include/joy/japi.h 聚合包含；
 *       本头只依赖基础类型头，不再 include 聚合门面 joy.h，避免循环包含。
 * 生成链：本头是 gen_bindings.js 的 DECL_SOURCES 之一，改动声明后必须跑 --scan。
 ******************************************************************************/
#ifndef JOY_R3D_IMPL_H
#define JOY_R3D_IMPL_H

#include <joy/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

        ///////////////////////////////////////////////////////////////////////
        ////////////////////////////  j3rd ///////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        JOY_API void* joy_r3d_load_shader(void* device, const char* filename,
                int stage, int sampler_num, int uniform_buffer_num);
        JOY_API void* joy_r3d_create_pipeline(void* device,
                void* vertex_shader, void* fragment_shader,
                int swapchain_format, int depth_format);
        JOY_API void* joy_r3d_upload_buffer(void* device, const void* data, size_t size);
        JOY_API void* joy_r3d_create_solid_texture(void* device,
                unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        JOY_API void* joy_r3d_create_depth_texture(void* device, int width, int height);
        JOY_API void* joy_r3d_create_sampler(void* device);
        JOY_API void  joy_r3d_mat44_to_float16(const joy_mat44_t* m, float out[16]);

        /* ================================================================
         * 高级 API (v3.0) — 可重入，无 SDL3 类型暴露
         * ================================================================ */

         /* ---------- 生命周期 ---------- */
        JOY_API joy_r3d_p joy_r3d_create(joy_window_p win, const char* shader_dir);
        JOY_API void   joy_r3d_destroy(joy_r3d_p r);
        /* 返回内部 SDL_GPUDevice*, 供调用方用 SDL3 API 自传纹理(如字体图集) */
        JOY_API void*  joy_r3d_get_device(joy_r3d_p r);

        /* ---------- 帧管理 ---------- */
        JOY_API bool joy_r3d_begin(joy_r3d_p r);
        JOY_API void joy_r3d_end(joy_r3d_p r);
        /* 返回本帧已提交的 draw call 数（joy_r3d_begin 清零, 供性能诊断） */
        JOY_API int joy_r3d_get_draw_calls(joy_r3d_p r);
        /* 设置清屏色（默认深色 0.05/0.06/0.10），游戏场景可改为白底蓝图风 */
        JOY_API void joy_r3d_set_clear_color(joy_r3d_p r, uint8_t cr, uint8_t cg, uint8_t cb);

        /* ---------- 相机 ---------- */
        JOY_API void joy_r3d_camera_lookat(joy_r3d_p r, joy_vec3_t eye, joy_vec3_t target, joy_vec3_t up, float fovy);
        JOY_API void joy_r3d_camera_fps(joy_r3d_p r, joy_vec3_t pos, float yaw, float pitch, float fovy);

        /* ---------- 网格 ---------- */
        JOY_API joy_render_mesh_p joy_r3d_mesh_cube(joy_r3d_p r, float w, float h, float d);
        JOY_API joy_render_mesh_p joy_r3d_mesh_sphere(joy_r3d_p r, float radius, int slices, int stacks);
        JOY_API joy_render_mesh_p joy_r3d_mesh_plane(joy_r3d_p r, float w, float d, int segs);
        JOY_API joy_render_mesh_p joy_r3d_mesh_ramp(joy_r3d_p r, float w, float l, float h);
        JOY_API joy_render_mesh_p joy_r3d_mesh_custom(joy_r3d_p r, const void* verts, int vertex_count);
        /* 单位平面 quad: 4 顶点(FLOAT3 pos + FLOAT2 uv) + 6 索引(0,1,2,0,3,2), 范围 x/y ∈ [-0.5,0.5] */
        JOY_API joy_render_mesh_p joy_r3d_mesh_quad(joy_r3d_p r);
        /* 线框网格 (pos FLOAT3 + uv FLOAT2, 5 float/顶点, LINELIST 基元, 无索引)
           仅包含边缘线段, 无对角线, 适合纯线框画风 */
        JOY_API joy_render_mesh_p joy_r3d_mesh_cube_wire(joy_r3d_p r, float w, float h, float d);
        JOY_API joy_render_mesh_p joy_r3d_mesh_sphere_wire(joy_r3d_p r, float radius, int slices, int stacks);
        JOY_API joy_render_mesh_p joy_r3d_mesh_plane_wire(joy_r3d_p r, float w, float d, int segs);
        JOY_API joy_render_mesh_p joy_r3d_mesh_ramp_wire(joy_r3d_p r, float w, float l, float h);
        JOY_API void         joy_r3d_mesh_destroy(joy_r3d_p r, joy_render_mesh_p m);

        /* ---------- 纹理 ---------- */
        /* 通用 RGBA32 纹理创建(自动处理 upload 行对齐, flip=true 时上下翻转, 与 stbi_set_flip_vertically_on_load 对应) */
        JOY_API void* joy_r3d_create_texture(joy_r3d_p r, int w, int h, const void* rgba32, int flip);
        /* 从 PNG 文件加载并创建 GPU 纹理(SDL_LoadPNG -> RGBA32 -> joy_r3d_create_texture, flip=1)
           成功返回 SDL_GPUTexture*(void*), 失败返回 NULL 并通过 SDL_Log 暴露原因 */
        JOY_API void* joy_r3d_load_texture_png(joy_r3d_p r, const char* path, int* outW, int* outH);
        JOY_API void* joy_r3d_gen_checkerboard(joy_r3d_p r, int tile_size, int tile_count);

        /* ---------- 字体渲染 (stb_truetype) ---------- */
        /* 初始化字体(读取 TTF, 由引擎持有缓冲, 程序退出前不释放) */
        JOY_API bool joy_r3d_font_init(joy_r3d_p r, const char* ttf_path, int pixel_height);
        /* 在 3D 空间以 billboard 绘制文字: topLeft 为左上角锚点(世界坐标),
           yaw/pitch 为屏幕基向量方向(与相机一致), size 为文字世界高度。
           right/up 由 yaw/pitch 推得, 保证文字直立不斜切。 */
        JOY_API bool joy_r3d_font_draw_3d(joy_r3d_p r, joy_render_mesh_p m, const char* text,
                                       joy_vec3_t topLeft, float yaw, float pitch, float size);
        /* 设置 3D 世界文字颜色(RGB 烘焙进下一段 bake 的纹理, 需在 draw_3d 前调用) */
        JOY_API void joy_r3d_font_set_color(joy_r3d_p r, uint8_t cr, uint8_t cg, uint8_t cb);
        /* 固定朝向绘制文字(不随相机变化): 始终水平直立、正对 -Z。
           topLeft 为左上角锚点(世界坐标), size 为文字世界高度。 */
        JOY_API bool joy_r3d_font_draw_fixed(joy_r3d_p r, joy_render_mesh_p m, const char* text,
                                         joy_vec3_t topLeft, float size);
        /* 相机朝向 billboard 绘制文字: 文字平面由 right/up 基向量张成(取相机基向量),
           文字沿 right 横向、沿 up 纵向, 字形正向同 2D 文字。 */
        JOY_API bool joy_r3d_font_draw_billboard(joy_r3d_p r, joy_render_mesh_p m, const char* text,
                                             joy_vec3_t topLeft, joy_vec3_t right, joy_vec3_t up, float size);
        /* 屏幕 2D HUD 文字: 固定在窗口像素坐标(pixelX,pixelY 左上, Y向下), 不随相机变化。
           fontSize 为像素高度, winW/winH 为窗口像素尺寸。 */
        JOY_API bool joy_r3d_font_draw_2d(joy_r3d_p r, joy_render_mesh_p m, const char* text,
                                      float pixelX, float pixelY, float fontSize,
                                      float winW, float winH);
        /* 带颜色的 2D HUD 文字(其余参数同 joy_r3d_font_draw_2d) */
        JOY_API bool joy_r3d_font_draw_2d_color(joy_r3d_p r, joy_render_mesh_p m, const char* text,
                                            float pixelX, float pixelY, float fontSize,
                                            float winW, float winH,
                                            uint8_t cr, uint8_t cg, uint8_t cb);
        /* 带描边的 2D HUD 文字: 主色 + 描边色(4 方向偏移), 任何背景下可读 */
        JOY_API bool joy_r3d_font_draw_2d_outline(joy_r3d_p r, joy_render_mesh_p m, const char* text,
                                              float pixelX, float pixelY, float fontSize,
                                              float winW, float winH,
                                              uint8_t cr, uint8_t cg, uint8_t cb,
                                              uint8_t or_, uint8_t og, uint8_t ob,
                                              float outline);
        /* 取当前渲染尺寸(像素, 与视口一致), 用于 2D HUD 投影。可传 NULL 忽略某一项 */
        JOY_API void joy_r3d_get_window_size(joy_r3d_p r, int* outW, int* outH);

        /* 文字烘焙缓存控制
         * 3D UI 的每句文字会烘焙成一张 GPU 纹理并按 (字符串,颜色) 缓存(LRU)。
         * 当不同文本组合超过上限时, 最久未使用的条目会被淘汰并释放其纹理。
         * 编辑器/HUD 这类有大量不同文本的场景, 默认上限 32 可能偏小导致缓存颠簸
         * (固定文字被频繁变化的文本挤掉后每帧重建纹理), 可按需调大。
         */
        JOY_API void joy_r3d_font_cache_set_limit(joy_r3d_p r, int max_entries);
        JOY_API int  joy_r3d_font_cache_get_count(joy_r3d_p r);
        /* 清空缓存并释放全部缓存纹理(字体销毁前调用) */
        JOY_API void joy_r3d_font_cache_clear(joy_r3d_p r);
        /* 回收闲置超过 max_idle_frames 帧的缓存条目并释放其纹理(单位: 渲染帧,
           由 joy_r3d_begin 推进)。对齐 joy_r2d_text_cache_sweep 的语义, 供周期性调用清理
           一次性文本(如滚动的聊天记录), 避免缓存长期被历史文本占满。 */
        JOY_API void joy_r3d_font_cache_sweep(joy_r3d_p r, int max_idle_frames);

        /* 世界坐标 -> 屏幕像素坐标(原点左上, Y 向下)。返回 false 表示点在相机后面或视锥外 */
        JOY_API bool joy_r3d_world_to_screen(joy_r3d_p r, joy_vec3_t world, float* screenX, float* screenY);

        /* ---------- 2D UI 逻辑分辨率(仿 jr2d joy_r2d_set_logical_presentation, 窗口缩放自适应) ---------- */
        /* 设置逻辑分辨率: 所有 2D UI(按钮/输入框/滑块/复选框/文字)以逻辑坐标布局,
           渲染时按 窗口尺寸/逻辑尺寸 等比缩放, 命中测试自动把窗口坐标换算回逻辑坐标。
           传 0 表示关闭(直接用窗口像素坐标)。 */
        JOY_API void joy_r3d_set_logical_presentation(joy_r3d_p r, int w, int h);
        /* 设置逻辑分辨率 + 呈现模式。
           mode=JOY_LOGICAL_PRESENTATION_STRETCH:   分别按 x/y 拉伸铺满(默认, 旧行为, 非 3:2 屏圆会变形);
           mode=JOY_LOGICAL_PRESENTATION_LETTERBOX: 等比缩放并居中, 圆/文字在任意屏宽比下都不拉伸(推荐移动端)。 */
        JOY_API void joy_r3d_set_logical_presentation_mode(joy_r3d_p r, int w, int h, int mode);
        /* 获取逻辑分辨率 */
        JOY_API void joy_r3d_get_logical_presentation(joy_r3d_p r, int* w, int* h);
        /* 窗口像素坐标 -> 逻辑坐标 */
        JOY_API void joy_r3d_window_to_logical(joy_r3d_p r, float wx, float wy, float* lx, float* ly);
        /* 逻辑坐标 -> 窗口像素坐标 */
        JOY_API void joy_r3d_logical_to_window(joy_r3d_p r, float lx, float ly, float* wx, float* wy);

        /* ---------- 2D UI: button (仿 jr2d, 用屏幕正交投影渲染, 不随相机变化) ---------- */
        /* 按钮状态 */
        typedef enum {
            JR3D_BTN_NORMAL  = 0,
            JR3D_BTN_HOVER   = 1,
            JR3D_BTN_PRESSED = 2
        } joy_r3d_button_state_t;

        /* 按钮对象(屏幕像素坐标, 原点左上) */
        typedef struct joy_r3d_button joy_r3d_button_t, * joy_r3d_button_p;
        struct joy_r3d_button {
            float x, y, w, h;          /* 位置与尺寸(像素) */
            char  text[128];           /* 按钮文字 */
            joy_r3d_button_state_t state; /* 当前状态 */
            bool  pressedInside;       /* 按下时是否在按钮内(用于判定 click) */
            void (*onClick)(joy_r3d_button_p);  /* 点击回调(按下+松开均在按钮内触发) */
        };

        /* 创建一个按钮(屏幕像素坐标, 原点左上) */
        JOY_API joy_r3d_button_p joy_r3d_button_create(float x, float y, float w, float h, const char* text);
        /* 绘制按钮(需先 joy_r3d_font_init; m 为单位 quad)。按钮会自己处理 normal/hover/pressed 配色 */
        JOY_API void joy_r3d_button_draw(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_button_p btn, float winW, float winH);
        /* 圆形按钮: 视觉为圆形(直径=min(w,h), 黑描边+填充), 击打/文字处理同矩形按钮 */
        JOY_API void joy_r3d_button_draw_circle(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_button_p btn, float winW, float winH);
        /* 命中测试: 屏幕像素坐标 (mx,my) 是否在按钮内 */
        JOY_API bool joy_r3d_button_is_hovered(joy_r3d_p r, const joy_r3d_button_p btn, float mx, float my);
        /* 按下测试: 鼠标在按钮内且 mouse_down 为 true 时返回 true(并置为 pressed 状态) */
        JOY_API bool joy_r3d_button_is_pressed(joy_r3d_p r, joy_r3d_button_p btn, bool mouse_down, float mx, float my);
        /* 设置点击回调(按下并松开均在按钮内时触发) */
        JOY_API void joy_r3d_button_set_onclick(joy_r3d_button_p btn, void (*cb)(joy_r3d_button_p));
        /* 释放按钮: 更新状态为 hover/normal */
        JOY_API void joy_r3d_button_release(joy_r3d_p r, joy_r3d_button_p btn, float mx, float my);
        /* 销毁按钮 */
        JOY_API void joy_r3d_button_destroy(joy_r3d_button_p btn);

        /* 2D 实心矩形绘制(逻辑/窗口像素坐标, 左上角 x,y 尺寸 w,h, 颜色 cr,cg,cb) */
        JOY_API void joy_r3d_draw_rect_2d(joy_r3d_p r, joy_render_mesh_p m,
                                      float x, float y, float w, float h,
                                      uint8_t cr, uint8_t cg, uint8_t cb,
                                      float winW, float winH);

        /* 2D 自定义实心色 mesh 绘制(逻辑坐标, 中心 cx,cy 缩放 sx,sy, 用于环形/扇形等) */
        JOY_API void joy_r3d_draw_mesh_2d(joy_r3d_p r, joy_render_mesh_p m,
                                      float cx, float cy, float sx, float sy,
                                      uint8_t cr, uint8_t cg, uint8_t cb,
                                      float winW, float winH);

        /* 2D 贴图 quad 绘制(逻辑坐标左上 (x,y) 尺寸 w,h, 单位 quad 自带 UV)。
           tex 为 joy_r3d_load_texture_png 返回的 SDL_GPUTexture*。 */
        JOY_API void joy_r3d_draw_tex_2d(joy_r3d_p r, joy_render_mesh_p m,
                                     float x, float y, float w, float h,
                                     void* tex, float winW, float winH);

        /* ---------- 2D UI: label (仿 jr2d, 纯文字 2D HUD, 固定屏幕位置) ---------- */
        typedef struct joy_r3d_label joy_r3d_label_t, * joy_r3d_label_p;
        struct joy_r3d_label {
            float x, y;          /* 左上角位置(像素) */
            float fontSize;      /* 字号(像素) */
            uint8_t r, g, b;     /* 文字颜色 */
            char  text[256];     /* 文字内容 */
        };

        /* 创建 label(屏幕像素坐标, 原点左上) */
        JOY_API joy_r3d_label_p joy_r3d_label_create(float x, float y, float fontSize, uint8_t cr, uint8_t cg, uint8_t cb, const char* text);
        /* 更新文字内容 */
        JOY_API void joy_r3d_label_set_text(joy_r3d_label_p l, const char* text);
        /* 绘制 label */
        JOY_API void joy_r3d_label_draw(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_label_p l, float winW, float winH);
        /* 销毁 label */
        JOY_API void joy_r3d_label_destroy(joy_r3d_label_p l);

        /* ---------- 2D UI: input (仿 jr2d inputbox, 屏幕像素坐标, 不随相机变化) ---------- */
        /* 输入框对象(屏幕像素坐标, 原点左上) */
        typedef struct joy_r3d_input joy_r3d_input_t, * joy_r3d_input_p;
        struct joy_r3d_input {
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
            void (*on_submit)(joy_r3d_input_p);  /* 回车提交回调 */
        };

        /* 创建输入框(屏幕像素坐标, 原点左上) */
        JOY_API joy_r3d_input_p joy_r3d_input_create(float x, float y, float w, float h, float fontSize,
                                              uint8_t cr, uint8_t cg, uint8_t cb, const char* text);
        /* 设置/获取文本 */
        JOY_API void   joy_r3d_input_set_text(joy_r3d_input_p inp, const char* text);
        JOY_API const char* joy_r3d_input_get_text(joy_r3d_input_p inp);
        /* 命中测试: 屏幕像素坐标 (mx,my) 是否在输入框内 */
        JOY_API bool   joy_r3d_input_is_hovered(joy_r3d_p r, const joy_r3d_input_p inp, float mx, float my);
        /* 聚焦/失焦 */
        JOY_API void   joy_r3d_input_focus(joy_r3d_input_p inp);
        JOY_API void   joy_r3d_input_defocus(joy_r3d_input_p inp);
        JOY_API bool   joy_r3d_input_is_focused(const joy_r3d_input_p inp);
        /* 启用系统文本输入(需聚焦后调用, 否则 SDL 不产生 TEXT_INPUT 事件) */
        JOY_API void   joy_r3d_input_start_text_input(joy_r3d_p r, joy_r3d_input_p inp);
        /* 停止系统文本输入 */
        JOY_API void   joy_r3d_input_stop_text_input(joy_r3d_p r, joy_r3d_input_p inp);
        /* 在光标处插入字符串(支持中文等多字节) */
        JOY_API void   joy_r3d_input_insert(joy_r3d_input_p inp, const char* s);
        /* 删除光标前/后的 UTF-8 字符(退格/Delete) */
        JOY_API void   joy_r3d_input_backspace(joy_r3d_input_p inp);
        JOY_API void   joy_r3d_input_delete(joy_r3d_input_p inp);
        /* 光标移动(UTF-8 安全): 左/右/行首/行尾 */
        JOY_API void   joy_r3d_input_cursor_left(joy_r3d_input_p inp);
        JOY_API void   joy_r3d_input_cursor_right(joy_r3d_input_p inp);
        JOY_API void   joy_r3d_input_cursor_home(joy_r3d_input_p inp);
        JOY_API void   joy_r3d_input_cursor_end(joy_r3d_input_p inp);
        /* 鼠标点击定位光标: 屏幕像素坐标 (mx,my) -> cursor_pos(并聚焦) */
        JOY_API void   joy_r3d_input_click(joy_r3d_p r, joy_r3d_input_p inp, float mx, float my);
        /* 统一事件处理(对齐 jr2d joy_ui_inputbox_process_event): 内部处理
           鼠标聚焦/定位、文本输入、键盘(退格/删除/方向/Home/End/回车)。
           返回 true 表示事件已被输入框消费(调用方应停止后续处理, 如相机旋转)。 */
        JOY_API bool   joy_r3d_input_handle_event(joy_r3d_p r, joy_r3d_input_p inp, const void* event);
        /* 设置回车提交回调 */
        JOY_API void   joy_r3d_input_set_onsubmit(joy_r3d_input_p inp, void (*cb)(joy_r3d_input_p));
        /* 触发提交(调用 on_submit); 供回车事件调用 */
        JOY_API void   joy_r3d_input_submit(joy_r3d_input_p inp);
        /* 绘制输入框(需先 joy_r3d_font_init; m 为单位 quad) */
        JOY_API void   joy_r3d_input_draw(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_input_p inp, float winW, float winH);
        /* 销毁输入框 */
        JOY_API void   joy_r3d_input_destroy(joy_r3d_input_p inp);

        /* ---------- 2D UI: combobox_input (可输入下拉列表框, 仿 jr2d combobox_input) ---------- */
        /* 可输入下拉列表框: 左侧为文本输入框, 右侧为下拉箭头, 展开后显示列表项 */
        typedef struct joy_r3d_combobox_input joy_r3d_combobox_input_t, * joy_r3d_combobox_input_p;
        struct joy_r3d_combobox_input {
            joy_r3d_input_p input;          /* 内部输入框(文本编辑复用 r3d_input) */
            float x, y, w, h;            /* 整体区域(像素) */
            float fontSize;              /* 字号(像素) */
            char items[20][JOY_MAX_IP];  /* 下拉列表项 */
            int item_count;
            int selected_index;
            bool is_open;                /* 下拉列表是否展开 */
            int item_height;             /* 每项高度(像素) */
            float arrow_w;               /* 右侧箭头宽度(像素) */
            bool arrow_hovered;
            void (*on_change)(joy_r3d_combobox_input_p);  /* 文本变化回调 */
        };

        /* 创建可输入下拉列表框(屏幕像素坐标, 原点左上) */
        JOY_API joy_r3d_combobox_input_p joy_r3d_combobox_input_create(float x, float y, float w, float h,
                float fontSize, uint8_t cr, uint8_t cg, uint8_t cb, const char* text);
        /* 销毁 */
        JOY_API void joy_r3d_combobox_input_destroy(joy_r3d_combobox_input_p ci);
        /* 添加下拉列表项 */
        JOY_API void joy_r3d_combobox_input_add_item(joy_r3d_combobox_input_p ci, const char* str);
        /* 清空下拉列表项 */
        JOY_API void joy_r3d_combobox_input_clear_items(joy_r3d_combobox_input_p ci);
        /* 获取/设置文本 */
        JOY_API const char* joy_r3d_combobox_input_get_text(joy_r3d_combobox_input_p ci);
        JOY_API void joy_r3d_combobox_input_set_text(joy_r3d_combobox_input_p ci, const char* str);
        /* 命中测试: 返回点击的下拉列表项索引, 未展开或未命中返回 -1 */
        JOY_API int joy_r3d_combobox_input_hit_item(joy_r3d_combobox_input_p ci, float x, float y);
        /* 统一事件处理: 鼠标(箭头切换/列表选中/外部关闭) + 键盘(上下导航/回车选中) + 文本输入 */
        JOY_API bool joy_r3d_combobox_input_handle_event(joy_r3d_p r, joy_r3d_combobox_input_p ci, const void* event);
        /* 绘制(输入框 + 箭头 + 展开的下拉列表) */
        JOY_API void joy_r3d_combobox_input_draw(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_combobox_input_p ci, float winW, float winH);
        /* 仅绘制展开的下拉列表(供场景在最后调用, 保证列表在最上层不被其他控件遮挡) */
        JOY_API void joy_r3d_combobox_input_draw_dropdown(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_combobox_input_p ci, float winW, float winH);

        /* ---------- 2D UI: slider (仿 jr2d slider, 屏幕像素坐标, 不随相机变化) ---------- */
        /* 滑块对象(屏幕像素坐标, 原点左上) */
        typedef struct joy_r3d_slider joy_r3d_slider_t, * joy_r3d_slider_p;
        struct joy_r3d_slider {
            float x, y, w, h;          /* 滑轨区域(像素) */
            float min, max;            /* 取值范围 */
            float value;               /* 当前值 */
            float thumb_width;         /* 滑块宽度(高度 = track.h) */
            bool  is_dragging;         /* 是否在拖动 */
            bool  hovered;             /* 滑块是否悬停 */
            joy_color_t track_color;      /* 滑轨背景色 */
            joy_color_t fill_color;       /* 已填充部分颜色 */
            joy_color_t thumb_color;      /* 滑块颜色 */
            joy_color_t thumb_hover_color;/* 滑块悬停颜色 */
        };

        /* 创建滑块(屏幕像素坐标, 原点左上) */
        JOY_API joy_r3d_slider_p joy_r3d_slider_create(float x, float y, float w, float h, float min, float max, float initial);
        /* 销毁滑块 */
        JOY_API void joy_r3d_slider_destroy(joy_r3d_slider_p s);
        /* 设置/获取值(自动夹在 [min,max] 内) */
        JOY_API void  joy_r3d_slider_set_value(joy_r3d_slider_p s, float v);
        JOY_API float joy_r3d_slider_get_value(joy_r3d_slider_p s);
        /* 设置配色: 滑轨 / 已填充 / 滑块 / 滑块悬停 */
        JOY_API void joy_r3d_slider_set_colors(joy_r3d_slider_p s, joy_color_t track, joy_color_t fill, joy_color_t thumb, joy_color_t thumb_hover);
        /* 绘制滑块(m 为单位 quad) */
        JOY_API void joy_r3d_slider_draw(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_slider_p s, float winW, float winH);
        /* 按下测试: 命中滑块或滑轨时更新值并进入拖动 */
        JOY_API bool joy_r3d_slider_is_pressed(joy_r3d_p r, joy_r3d_slider_p s, float x, float y);
        /* 设置拖动状态 */
        JOY_API void joy_r3d_slider_set_pressed(joy_r3d_slider_p s, bool v);
        /* 拖动中更新值 */
        JOY_API void joy_r3d_slider_drag_move(joy_r3d_p r, joy_r3d_slider_p s, float x, float y);
        /* 悬停测试: 命中滑块时置 hovered */
        JOY_API bool joy_r3d_slider_is_hovered(joy_r3d_p r, joy_r3d_slider_p s, float x, float y);
        /* 设置悬停状态 */
        JOY_API void joy_r3d_slider_set_hovered(joy_r3d_slider_p s, bool v);

        /* ---------- 2D UI: checkbox (仿 jr2d checkbox, 屏幕像素坐标, 不随相机变化) ---------- */
        typedef enum joy_r3d_checkbox_state {
            JOY_R3D_CHECKBOX_UNCHECKED = 0,
            JOY_R3D_CHECKBOX_CHECKED,
            JOY_R3D_CHECKBOX_INDETERMINATE
        } joy_r3d_checkbox_state_t;

        typedef struct joy_r3d_checkbox joy_r3d_checkbox_t, * joy_r3d_checkbox_p;
        struct joy_r3d_checkbox {
            float x, y;                 /* 方框左上角(像素) */
            float size;                 /* 方框边长 */
            joy_r3d_checkbox_state_t state;
            bool  hovered;
            bool  enabled;
            char* label;                /* 标签文本 */
            float label_w;              /* 标签宽度(命中区域用) */
            joy_color_t box_color;         /* 框体背景色 */
            joy_color_t check_color;       /* 勾选颜色 */
            joy_color_t text_color;        /* 标签颜色 */
            joy_color_t hover_color;       /* 悬停色 */
        };

        /* 创建复选框(屏幕像素坐标, 原点左上) */
        JOY_API joy_r3d_checkbox_p joy_r3d_checkbox_create(float x, float y, float size, const char* label);
        /* 销毁复选框 */
        JOY_API void joy_r3d_checkbox_destroy(joy_r3d_checkbox_p cb);
        /* 设置/获取状态 */
        JOY_API void joy_r3d_checkbox_set_state(joy_r3d_checkbox_p cb, joy_r3d_checkbox_state_t state);
        JOY_API joy_r3d_checkbox_state_t joy_r3d_checkbox_get_state(joy_r3d_checkbox_p cb);
        /* 切换勾选状态 */
        JOY_API void joy_r3d_checkbox_toggle(joy_r3d_checkbox_p cb);
        /* 按下测试: 命中方框或标签时切换状态 */
        JOY_API bool joy_r3d_checkbox_is_pressed(joy_r3d_p r, joy_r3d_checkbox_p cb, float x, float y);
        /* 设置/获取悬停状态 */
        JOY_API void joy_r3d_checkbox_set_hovered(joy_r3d_checkbox_p cb, bool v);
        JOY_API bool joy_r3d_checkbox_is_hovered(joy_r3d_p r, joy_r3d_checkbox_p cb, float x, float y);
        /* 设置配色: 框体 / 勾选 / 悬停 */
        JOY_API void joy_r3d_checkbox_set_colors(joy_r3d_checkbox_p cb, joy_color_t box, joy_color_t check, joy_color_t hover);
        /* 绘制复选框(m 为单位 quad) */
        JOY_API void joy_r3d_checkbox_draw(joy_r3d_p r, joy_render_mesh_p m, joy_r3d_checkbox_p cb, float winW, float winH);

        /* ---------- 绘制 ---------- */
        JOY_API void joy_r3d_draw_mesh(joy_r3d_p r, joy_render_mesh_p m, joy_vec3_t pos, joy_vec3_t scale, joy_color_t color);
        JOY_API void joy_r3d_draw_mesh_tex(joy_r3d_p r, joy_render_mesh_p m, joy_vec3_t pos, joy_vec3_t scale, void* tex);
        /* billboard 绘制: 用 center + right + up 基向量(无需归一化, 内部按 size 缩放) 把单位 quad 朝向相机 */
        JOY_API void joy_r3d_draw_mesh_tex_basis(joy_r3d_p r, joy_render_mesh_p m,
                joy_vec3_t center, joy_vec3_t right, joy_vec3_t up, float size, void* tex);
        JOY_API void joy_r3d_draw_grid(joy_r3d_p r, int slices, float spacing, float line_width);

        /* ---------- 实体填充渲染（纯色填充 + 深度写入, 用于"填充+描边"画风） ---------- */
        /* 用写深度的实体管线绘制纯色网格, 提供遮挡与层次; 再叠加 joy_r3d_draw_wire 描边 */
        JOY_API void joy_r3d_draw_mesh_solid(joy_r3d_p r, joy_render_mesh_p m, joy_vec3_t pos, joy_vec3_t scale, joy_color_t color);
        JOY_API void joy_r3d_draw_mesh_solid_basis(joy_r3d_p r, joy_render_mesh_p m, const joy_mat44_t* model, joy_color_t color);

        /* ---------- 线框渲染（LINELIST 基元, 需在 joy_r3d_begin/joy_r3d_end 之间调用） ---------- */
        /* 绘制线框网格, 使用纯色填充(内部通过颜色纹理实现) */
        JOY_API void joy_r3d_draw_wire(joy_r3d_p r, joy_render_mesh_p m, joy_vec3_t pos, joy_vec3_t scale, joy_color_t color);
        /* 用 mat44 模型矩阵绘制线框, 适合 ECS 系统中直接传入 transform */
        JOY_API void joy_r3d_draw_wire_basis(joy_r3d_p r, joy_render_mesh_p m, const joy_mat44_t* model, joy_color_t color);
        /* 线框 pipeline 创建（与非 lit pipeline 共用 shader, 仅 primitive type 为 LINELIST） */
        JOY_API void* joy_r3d_create_line_pipeline(void* device,
                void* vertex_shader, void* fragment_shader,
                int swapchain_format, int depth_format);

        /* ---------- 光照渲染（顶点格式 pos+normal+uv, pitch=32） ---------- */
        /* 创建光照管线: 顶点 uniform slot0 = joy_render_mvp_t(MVP),
           片元 uniform slot0 = joy_r3d_light_block_t, 片元采样器 slot0 = 纹理 */
        JOY_API void* joy_r3d_create_lighting_pipeline(void* device,
                void* vertex_shader, void* fragment_shader,
                int swapchain_format, int depth_format);
        /* 带法线的网格生成（pos+normal+uv, 8 float/顶点） */
        JOY_API joy_render_mesh_p joy_r3d_mesh_cube_lit(joy_r3d_p r, float w, float h, float d);
        JOY_API joy_render_mesh_p joy_r3d_mesh_sphere_lit(joy_r3d_p r, float radius, int slices, int stacks);
        JOY_API joy_render_mesh_p joy_r3d_mesh_plane_lit(joy_r3d_p r, float w, float d, int segs);
        JOY_API joy_render_mesh_p joy_r3d_mesh_ramp_lit(joy_r3d_p r, float w, float l, float h);
        JOY_API joy_render_mesh_p joy_r3d_mesh_custom_lit(joy_r3d_p r, const void* verts, int vertex_count);
        /* 设置光照数据; 相机更新时 viewPos 会自动覆盖为当前相机位置 */
        JOY_API void joy_r3d_set_light_block(joy_r3d_p r, const joy_r3d_light_block_t* block);
        /* 用光照管线绘制网格（需在 joy_r3d_begin/joy_r3d_end 之间调用） */
        JOY_API void joy_r3d_draw_mesh_lit(joy_r3d_p r, joy_render_mesh_p m, const joy_mat44_t* model, void* tex);
        JOY_API void joy_r3d_draw_mesh_lit_color(joy_r3d_p r, joy_render_mesh_p m, const joy_mat44_t* model, joy_color_t color);

        ///////////////////////////////////////////////////////////////////////
       //////////////////////  field-of-view (FOV)  /////////////////////////
       ///////////////////////////////////////////////////////////////////////
       /* 通用战争迷雾/视野遮挡。纯引擎能力：调用方提供遮挡矩形与观察点，
          引擎负责逐角精确射线遮挡检测 + 羽化软边视野网格绘制，不依赖游戏数据。 */

        /* 水平遮挡矩形(包围盒): 中心(x,z), 半宽(hx/hz) */
        typedef struct joy_fov_rect { float x, z, hx, hz; } joy_fov_rect;

        /* 从 (px,pz) 沿归一化方向 (dx,dz) 求最近遮挡距离；无命中返回 1e9。
           blockers 为 NULL 或 n==0 时视为无遮挡(返回 1e9)。 */
        JOY_API float joy_r3d_fov_ray_dist(const joy_fov_rect* blockers, int n,
                float px, float pz, float dx, float dz);
        /* 计算观察点周围 seg 个方向的可见半径(每方向=min(视距,遮挡距离), 逐角精确射线)。
           angle_start/angle_step 单位度；out 长度 seg。返回 false 仅当入参非法。 */
        JOY_API bool joy_r3d_fov_compute_radii(const joy_fov_rect* blockers, int n,
                float px, float pz, float vis_radius,
                float angle_start, float angle_step, int seg, float* out);
        /* 绘制羽化视野网格: 观察点(py 为雾面高度), radii[seg] 每方向可见半径,
           feather=可见边界的羽化宽度(alpha 渐变), outer_radius=完全遮蔽的雾外径。
           需在 joy_r3d_begin/joy_r3d_end 之间调用。 */
        JOY_API void joy_r3d_fov_draw(joy_r3d_p r, float px, float py, float pz,
                const float* radii, int seg, float feather, float outer_radius);


        JOY_API joy_light_sys_p joy_r3d_light_sys_create(int width, int height);
        JOY_API void joy_r3d_light_sys_destroy(joy_light_sys_p sys);
        JOY_API void joy_r3d_light_sys_resize(joy_light_sys_p sys, int width, int height);
        JOY_API int  joy_r3d_light_add(joy_light_sys_p sys, joy_light_t light);
        JOY_API void joy_r3d_light_remove(joy_light_sys_p sys, int id);
        JOY_API void joy_r3d_light_set(joy_light_sys_p sys, int id, joy_light_t light);
        JOY_API void joy_r3d_light_clear(joy_light_sys_p sys);
        JOY_API int joy_r3d_light_add_occluder(joy_light_sys_p sys, float x1, float y1, float x2, float y2);
        JOY_API void joy_r3d_light_clear_occluders(joy_light_sys_p sys);
        JOY_API void joy_r3d_light_set_ambient(joy_light_sys_p sys, uint8_t r, uint8_t g, uint8_t b);
        JOY_API void joy_r3d_light_render(joy_light_sys_p sys, joy_r2d_p r2d);

        /* 生成链路专用：joy_r3d_mesh_destroy 的单参数版本，供 IDL 的 owns 使用。
         * render_mesh 内部记了创建它的 r3d_p（owner），所以 dtor 只需一个参数，
         * 能通过 ownsIssue() 的「单参数、参数类型与 ret 严格相同」硬校验。
         * 约束：mesh 句柄不得比创建它的 r3d 上下文活得久，否则 owner 悬空。
         * 声明放在文件末尾，避免打乱 joy.idl 既有条目的 source 行号。 */
        JOY_API void joy_r3d_mesh_destroy_one(joy_render_mesh_p m);

#ifdef __cplusplus
}
#endif

#endif /* JOY_R3D_IMPL_H */
