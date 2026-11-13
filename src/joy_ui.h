/******************************************************************************
 * src/joy_ui.h —— ui 域声明头（joy_ui.c 的配套头）
 * 职责：控件 / 即时模式 UI
 * 方向：本头是该域声明的归属地，由 include/joy/japi.h 聚合包含；
 *       本头只依赖基础类型头，不再 include 聚合门面 joy.h，避免循环包含。
 * 生成链：本头是 gen_bindings.js 的 DECL_SOURCES 之一，改动声明后必须跑 --scan。
 ******************************************************************************/
#ifndef JOY_UI_IMPL_H
#define JOY_UI_IMPL_H

#include <joy/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

        JOY_API joy_label_p joy_ui_label_create(joy_r2d_p r2d, joy_font_p fontinfo, const int* codepoints, int len);
        JOY_API void joy_ui_label_destroy(joy_label_p label);
        JOY_API void joy_ui_label_set_text(joy_label_p label, joy_font_p fontinfo, const int* codepoints, int len, joy_color_t color);
        JOY_API void joy_ui_label_draw(joy_label_p label);
        JOY_API void joy_ui_label_process_event(joy_label_p label, const void* event);
        JOY_API joy_button_p joy_ui_button_create(joy_r2d_p r2d, float cx, float cy, float wf, float hf);
        JOY_API void joy_ui_button_destroy(joy_button_p btn);
        JOY_API void joy_ui_button_set_position(joy_button_p btn, float x, float y);
        JOY_API joy_vec2_t joy_ui_button_get_position(joy_button_p btn);
        /* 布局需要：复用同一个 button 时改变尺寸（旧 API 只能在 create 时定尺寸） */
        JOY_API void joy_ui_button_set_size(joy_button_p btn, float width, float height);
        JOY_API void joy_ui_button_set_normal_color(joy_button_p btn, joy_color_t color);
        JOY_API void joy_ui_button_set_hover_color(joy_button_p btn, joy_color_t color);
        JOY_API void joy_ui_button_set_pressed_color(joy_button_p btn, joy_color_t color);
        JOY_API void joy_ui_button_set_image(joy_button_p btn, joy_image_p image);
        JOY_API void joy_ui_button_set_pressed_image(joy_button_p btn, joy_image_p image);
        JOY_API void joy_ui_button_set_textx(joy_button_p btn, joy_font_p fontinfo, const char* str, int len, joy_color_t color);
        JOY_API void joy_ui_button_set_text(joy_button_p btn, joy_font_p fontinfo, const int* codepoints, int len, joy_color_t color);
        JOY_API bool joy_ui_button_is_hovered(joy_button_p btn, float x, float y);
        JOY_API bool joy_ui_button_is_pressed(joy_button_p btn, float x, float y);
        JOY_API void joy_ui_button_set_hovered(joy_button_p btn, bool v);
        JOY_API void joy_ui_button_set_pressed(joy_button_p btn, bool v);
        JOY_API bool joy_ui_button_get_pressed(joy_button_p btn);
        JOY_API void joy_ui_button_set_text_offset(joy_button_p btn, float x, float y);
        JOY_API void joy_ui_button_draw(joy_button_p btn);
        JOY_API void joy_ui_button_process_event(joy_button_p btn, const void* event);

        /* ============ 滚动层 scrollview（可拖拽滚动的容器）============
         * 横向 + 纵向滚动；鼠标拖拽 / 触摸滑动 / 滚轮都支持（触摸在 SDL 里也走鼠标事件）。
         * 与保留模式子控件（button/inputbox 等）配合的典型用法：
         *
         *   joy_scrollview_p v = joy_ui_scrollview_create(r2d, fx, fy, fw, fh);  // 归一化视口
         *   joy_ui_scrollview_set_content(v, 内容总宽, 内容总高);
         *
         *   // ① 事件：先问滚动层。返回 true = 本次手势是滚动，调用方应忽略点击
         *   //   （否则"滑动一下"会误触发子控件；故点击判定建议放到松手，见下）
         *   if (joy_ui_scrollview_process_event(v, ev)) return;   // 已作为滚动消费
         *   ...你原有的点击/悬停逻辑...
         *
         *   // ② 每帧绘制：begin/end 之间画子控件，超出视口的部分被裁掉
         *   joy_ui_scrollview_begin(v);
         *   for (i) joy_ui_button_set_position(child[i],
         *               base_x[i] + joy_ui_scrollview_get_offset_x(v),
         *               base_y[i] + joy_ui_scrollview_get_offset_y(v));
         *   for (i) joy_ui_button_draw(child[i]);
         *   joy_ui_scrollview_end(v);      // 恢复裁剪 + 画滚动条
         *
         * 偏移恒 ≤0（子控件整体向左/上移 = 用户向右/下看），并夹紧在内容范围内；
         * 内容不大于视口时偏移恒 0、不画滚动条、拖拽与滚轮均无效。
         */
        JOY_API joy_scrollview_p joy_ui_scrollview_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh);
        JOY_API void  joy_ui_scrollview_destroy(joy_scrollview_p v);
        JOY_API void  joy_ui_scrollview_set_view_px(joy_scrollview_p v, float x, float y, float w, float h);
        JOY_API void  joy_ui_scrollview_set_content(joy_scrollview_p v, float content_w, float content_h);
        JOY_API void  joy_ui_scrollview_set_offset(joy_scrollview_p v, float ox, float oy);
        JOY_API void  joy_ui_scrollview_clamp(joy_scrollview_p v);
        JOY_API float joy_ui_scrollview_get_offset_x(joy_scrollview_p v);
        JOY_API float joy_ui_scrollview_get_offset_y(joy_scrollview_p v);
        JOY_API void  joy_ui_scrollview_set_show_bar(joy_scrollview_p v, bool show);
        JOY_API bool  joy_ui_scrollview_dragging(joy_scrollview_p v);
        JOY_API bool  joy_ui_scrollview_hit(joy_scrollview_p v, float win_x, float win_y);
        JOY_API void  joy_ui_scrollview_begin(joy_scrollview_p v);
        JOY_API void  joy_ui_scrollview_end(joy_scrollview_p v);
        /* 返回 true = 该事件已被滚动层消费（拖拽中/滑动结束/滚轮），调用方不要再当点击 */
        JOY_API bool  joy_ui_scrollview_process_event(joy_scrollview_p v, const void* event);
        JOY_API joy_combobox_p joy_ui_combobox_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh);
        JOY_API void joy_ui_combobox_destroy(joy_combobox_p combobox);
        JOY_API void joy_ui_combobox_set_text(joy_combobox_p combobox, joy_font_p font,
                const int* codepoints, int len, joy_color_t color);
        JOY_API void joy_ui_combobox_add_item(joy_combobox_p combobox, joy_font_p font,
                const char* str, int len, joy_color_t color);
        JOY_API void joy_ui_combobox_add_itemx(joy_combobox_p combobox, joy_font_p font,
                const int* codepoints, int len, joy_color_t color);

        JOY_API bool joy_ui_combobox_is_pressed(joy_combobox_p combobox, float x, float y);
        JOY_API bool joy_ui_combobox_is_open(joy_combobox_p combobox);
        JOY_API void joy_ui_combobox_set_open(joy_combobox_p combobox, bool v);
        JOY_API int joy_ui_combobox_hit_item(joy_combobox_p combobox, float x, float y);
        JOY_API void joy_ui_combobox_set_selected(joy_combobox_p combobox, int index);
        JOY_API int joy_ui_combobox_get_selected(joy_combobox_p combobox);
        JOY_API void joy_ui_combobox_draw(joy_combobox_p combobox);
        JOY_API void joy_ui_combobox_process_event(joy_combobox_p cb, const void* event);

        /* ===== 栅格布局 jlayout（纯几何计算 + 可选标签绘制，不持有任何控件） =====
         * 场景按“逻辑像素”创建栅格：ox/oy/width 由逻辑宽计算，row_h 由高度基准 s 计算。
         * 每行：用 joy_ui_layout_grid_label() 在左列画标签、用 joy_ui_layout_grid_cell() 取右列矩形，
         * 再用各控件 *_create(r2d, px/LW, py/LH, pw/LW, ph/LH, ...) 创建控件。
         * col_fracs 为各列宽占比（和≈1），pad 为单元格内边距（像素）。 */
        JOY_API joy_layout_grid_t* joy_ui_layout_grid_create(float ox, float oy, float width, float row_h,
                int ncols, const float* col_fracs, float pad);
        JOY_API void joy_ui_layout_grid_destroy(joy_layout_grid_t* g);
        JOY_API void joy_ui_layout_grid_next_row(joy_layout_grid_t* g);
        JOY_API void joy_ui_layout_grid_set_row(joy_layout_grid_t* g, int row);
        /* 取第 col 列的像素矩形（已含内边距）。x/y/w/h 可为 NULL。 */
        JOY_API void joy_ui_layout_grid_cell(joy_layout_grid_t* g, int col, float* x, float* y, float* w, float* h);
        /* 在第 col 列画左对齐、垂直居中的标签，并返回该列像素矩形（cx/cy/cw/ch 可为 NULL）。 */
        JOY_API void joy_ui_layout_grid_label(joy_layout_grid_t* g, joy_r2d_p r2d, joy_font_p font,
                const char* text, int n, joy_color_t c, int col,
                float* cx, float* cy, float* cw, float* ch);
        /* 在当前行创建 n 个等宽按钮（横向按钮条/菜单），各占一列；
         * 按钮指针写入 out[]（调用方分配 n 个），创建后自动换到下一行。 */
        JOY_API void joy_ui_layout_grid_button_row(joy_layout_grid_t* g, joy_r2d_p r2d, joy_font_p font,
                const char** labels, int n, joy_button_p* out);

        // 可输入下拉列表框
        JOY_API joy_combobox_input_p joy_ui_combobox_input_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, joy_font_p font, int buf_len);
        JOY_API void joy_ui_combobox_input_destroy(joy_combobox_input_p ci);
        JOY_API void joy_ui_combobox_input_add_item(joy_combobox_input_p ci, const char* str, int len);
        JOY_API void joy_ui_combobox_input_clear_items(joy_combobox_input_p ci);
        JOY_API const char* joy_ui_combobox_input_get_text(joy_combobox_input_p ci);
        JOY_API void joy_ui_combobox_input_set_text(joy_combobox_input_p ci, const char* str);
        JOY_API void joy_ui_combobox_input_draw(joy_combobox_input_p ci);
        JOY_API void joy_ui_combobox_input_draw_dropdown(joy_combobox_input_p ci);
        JOY_API bool joy_ui_combobox_input_is_open(joy_combobox_input_p ci);
        JOY_API void joy_ui_combobox_input_set_popup_node(joy_combobox_input_p ci, joy_scene_node_p node);
        JOY_API void joy_ui_combobox_input_process_event(joy_combobox_input_p ci, const void* ev);
        JOY_API int joy_ui_combobox_input_hit_item(joy_combobox_input_p ci, float x, float y);
        /* 主动展开/收起下拉列表（原来只有 is_open，选中某项后无法收起）。 */
        JOY_API void joy_ui_combobox_input_set_open(joy_combobox_input_p ci, bool v);

        JOY_API joy_datagrid_p joy_ui_datagrid_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, int col_count, bool has_header, void* font);
        JOY_API void joy_ui_datagrid_setheaders(joy_datagrid_p grid, const char* headers);
        JOY_API void joy_ui_datagrid_setheader(joy_datagrid_p grid, int index, const char* header);
        JOY_API void joy_ui_datagrid_addrow(joy_datagrid_p grid, const char** row_data);
        JOY_API void joy_ui_datagrid_setcell(joy_datagrid_p grid, int row_index, int col_index, const char* data);
        JOY_API void joy_ui_datagrid_destroy(joy_datagrid_p grid);
        JOY_API void joy_ui_datagrid_draw(joy_datagrid_p grid, joy_r2d_p r2d);
        JOY_API int joy_ui_datagrid_hit_row(joy_datagrid_p grid, float x, float y);
        JOY_API void joy_ui_datagrid_scroll(joy_datagrid_p grid, int lines);
        JOY_API void joy_ui_datagrid_set_selected_row(joy_datagrid_p grid, int row);
        JOY_API void joy_ui_datagrid_process_event(joy_datagrid_p dg, const void* event);
        JOY_API joy_radio_p joy_ui_radio_create(float fx, float fy, float size_frac, const char* label, int group_id);
        JOY_API void joy_ui_radio_destroy(joy_radio_p radio);
        JOY_API bool joy_ui_radio_is_pressed(joy_radio_p radio, float x, float y);
        JOY_API void joy_ui_radio_set_hovered(joy_radio_p radio, bool v);
        JOY_API void joy_ui_radio_process_event(joy_radio_p rb, const void* event);
        JOY_API joy_checkbox_p joy_ui_checkbox_create(float fx, float fy, float size_frac, const char* label);
        JOY_API void joy_ui_checkbox_destroy(joy_checkbox_p checkbox);
        JOY_API void joy_ui_checkbox_set_state(joy_checkbox_p checkbox, joy_checkbox_state_t state);
        JOY_API joy_checkbox_state_t joy_ui_checkbox_get_state(joy_checkbox_p checkbox);
        JOY_API void joy_ui_checkbox_set_colors(joy_checkbox_p checkbox, joy_color_t box, joy_color_t check, joy_color_t hover);
        JOY_API void joy_ui_checkbox_set_r2d(joy_checkbox_p checkbox, joy_r2d_p r2d);
        /* 布局需要：移动复选框（旧 API 只能在 create 时定位置） */
        JOY_API void joy_ui_checkbox_set_position(joy_checkbox_p checkbox, int x, int y);
        JOY_API void joy_ui_checkbox_set_text_color(joy_checkbox_p checkbox, joy_color_t color);
        JOY_API void joy_ui_checkbox_toggle(joy_checkbox_p checkbox);
        JOY_API bool joy_ui_checkbox_is_pressed(joy_checkbox_p cb, float x, float y);
        JOY_API void joy_ui_checkbox_set_hovered(joy_checkbox_p cb, bool v);
        JOY_API void joy_ui_checkbox_draw(joy_checkbox_p cb, joy_r2d_p r2d, void* font);
        JOY_API void joy_ui_checkbox_process_event(joy_checkbox_p cb, const void* event);
        JOY_API joy_joystick_p joy_ui_joystick_create(joy_r2d_p r2d, float cx, float cy, float radius_frac);
        // 摇杆方向模式：约束/量化 joy_ui_joystick_get_direction 输出的方向
        typedef enum {
                JOYSTICK_DIR_OMNI = 0,       // 全方向（模拟，默认）
                JOYSTICK_DIR_4WAY,           // 上/下/左/右 四向
                JOYSTICK_DIR_8WAY,           // 四向 + 四个斜角（八向）
                JOYSTICK_DIR_VERTICAL,       // 仅 上/下
                JOYSTICK_DIR_HORIZONTAL,     // 仅 左/右
        } joy_joystick_dir_mode;

        JOY_API void joy_ui_joystick_destroy(joy_joystick_p joystick);
        JOY_API void joy_ui_joystick_set_textures(joy_joystick_p joystick, const char* base_path, const char* thumb_path);
        JOY_API void joy_ui_joystick_set_dir_mode(joy_joystick_p joystick, joy_joystick_dir_mode mode);
        JOY_API void joy_ui_joystick_set_vector(joy_joystick_p joystick, float dx, float dy);
        JOY_API void joy_ui_joystick_set_textures_for_mode(joy_joystick_p joystick, joy_joystick_dir_mode mode, const char* base_path, const char* thumb_path);
        JOY_API void joy_ui_joystick_draw(joy_joystick_p joystick);
        JOY_API void joy_ui_joystick_reset(joy_joystick_p joystick);
        JOY_API void joy_ui_joystick_set_position(joy_joystick_p joystick, float x, float y);
        /* 布局需要：逻辑分辨率变化（自适应宽高比/旋转）时改变摇杆半径，保持设计尺寸 */
        JOY_API void joy_ui_joystick_set_radius(joy_joystick_p joystick, float radius);
        JOY_API bool joy_ui_joystick_is_dragging(joy_joystick_p joystick);
        JOY_API void joy_ui_joystick_touch_down(joy_joystick_p joystick, float x, float y);
        JOY_API void joy_ui_joystick_touch_move(joy_joystick_p joystick, float x, float y);
        JOY_API void joy_ui_joystick_touch_up(joy_joystick_p joystick);
        JOY_API joy_vec2_t joy_ui_joystick_get_direction(joy_joystick_p joystick);
        JOY_API float joy_ui_joystick_get_magnitude(joy_joystick_p joystick);
        JOY_API void joy_ui_joystick_process_event(joy_joystick_p js, const void* event);
        JOY_API joy_inputbox_p joy_ui_inputbox_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, joy_font_p font);
        JOY_API void joy_ui_inputbox_destroy(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_set_text(joy_inputbox_p ti, const char* text);
        JOY_API const char* joy_ui_inputbox_get_text(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_set_callback(joy_inputbox_p ti, void (*submit)(joy_inputbox_p, void*), void (*change)(joy_inputbox_p, void*), void* userdata);
        JOY_API void joy_ui_inputbox_draw(joy_inputbox_p ti);
        JOY_API bool joy_ui_inputbox_is_pressed(joy_inputbox_p ti, float x, float y);
        JOY_API bool joy_ui_inputbox_is_hovered(joy_inputbox_p ti, float x, float y);
        JOY_API void joy_ui_inputbox_focus(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_defocus(joy_inputbox_p ti);
        /* 仅清除焦点，不调用 SDL_StopTextInput。用于多输入框之间切换焦点时保持软键盘不收起，
         * 避免 Android IME hide/show 同帧竞态导致新的输入框唤不起键盘。 */
        JOY_API void joy_ui_inputbox_blur(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_insert(joy_inputbox_p ti, const char* s, int len);
        JOY_API void joy_ui_inputbox_backspace(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_delete(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_cursor_left(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_cursor_right(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_cursor_home(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_cursor_end(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_select_left(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_select_right(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_select_home(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_select_end(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_select_all(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_paste(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_copy(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_cut(joy_inputbox_p ti);
        JOY_API bool joy_ui_inputbox_is_focused(joy_inputbox_p ti);
        JOY_API void joy_ui_inputbox_process_event(joy_inputbox_p ti, const void* event);
        JOY_API joy_slider_p joy_ui_slider_create(joy_r2d_p r2d, float fx, float fy, float fw, float fh, float min, float max, float initial);
        JOY_API void joy_ui_slider_destroy(joy_slider_p s);
        JOY_API void joy_ui_slider_set_value(joy_slider_p s, float v);
        JOY_API float joy_ui_slider_get_value(joy_slider_p s);
        JOY_API void joy_ui_slider_set_colors(joy_slider_p s, joy_color_t track, joy_color_t fill, joy_color_t thumb, joy_color_t thumb_hover);
        /* 布局需要：移动/缩放滑轨（旧 API 只能在 create 时定位置与尺寸） */
        JOY_API void joy_ui_slider_set_rect(joy_slider_p s, float x, float y, float w, float h);
        JOY_API void joy_ui_slider_set_range(joy_slider_p s, float min, float max);
        JOY_API void joy_ui_slider_draw(joy_slider_p s);
        JOY_API bool joy_ui_slider_is_pressed(joy_slider_p s, float x, float y);
        JOY_API void joy_ui_slider_set_pressed(joy_slider_p s, bool v);
        JOY_API void joy_ui_slider_drag_move(joy_slider_p s, float x, float y);
        JOY_API bool joy_ui_slider_is_hovered(joy_slider_p s, float x, float y);
        JOY_API void joy_ui_slider_set_hovered(joy_slider_p s, bool v);
        JOY_API void joy_ui_slider_process_event(joy_slider_p s, const void* event);
        JOY_API joy_popup_p joy_ui_popup_create(joy_r2d_p r2d, float fw, float fh);
        JOY_API void joy_ui_popup_destroy(joy_popup_p p);
        JOY_API void joy_ui_popup_set_title(joy_popup_p p, const char* text, int len);
        JOY_API void joy_ui_popup_set_content(joy_popup_p p, const char* text, int len);
        JOY_API void joy_ui_popup_set_position(joy_popup_p p, float x, float y);
        JOY_API void joy_ui_popup_set_visible(joy_popup_p p, bool v);
        JOY_API void joy_ui_popup_set_closeable(joy_popup_p p, bool v);
        JOY_API void joy_ui_popup_draw(joy_popup_p p, joy_font_p title_font, joy_font_p content_font);
        JOY_API bool joy_ui_popup_is_close_clicked(joy_popup_p p, float mx, float my);
        JOY_API bool joy_ui_popup_is_title_clicked(joy_popup_p p, float mx, float my);
        JOY_API void joy_ui_popup_drag_move(joy_popup_p p, float mx, float my);
        JOY_API bool joy_ui_popup_is_inside(joy_popup_p p, float mx, float my);
        JOY_API void joy_ui_popup_process_event(joy_popup_p p, const void* event);
        JOY_API joy_tooltip_p joy_ui_tooltip_create(joy_r2d_p r2d, const char* text, int len);
        JOY_API void joy_ui_tooltip_destroy(joy_tooltip_p t);
        JOY_API void joy_ui_tooltip_set_text(joy_tooltip_p t, const char* text, int len);
        JOY_API void joy_ui_tooltip_set_position(joy_tooltip_p t, float x, float y);
        JOY_API void joy_ui_tooltip_set_visible(joy_tooltip_p t, bool v);
        JOY_API void joy_ui_tooltip_draw(joy_tooltip_p t, joy_font_p font);
        JOY_API void joy_ui_tooltip_process_event(joy_tooltip_p t, const void* event);
        JOY_API void joy_ui_process_widget_event(void** widgets, int count, const void* event);

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

        JOY_API joy_ui_ctx_p joy_ui_ctx_create(joy_r2d_p r2d, joy_font_p font);
        JOY_API void      joy_ui_ctx_destroy(joy_ui_ctx_p ctx);
        JOY_API void      joy_ui_set_theme(joy_ui_ctx_p ctx, const joy_ui_theme_t* theme);

        /* 每帧开始 / 结束。begin 会转存一次性输入（按下/抬起/滚轮），end 清零。 */
        JOY_API void joy_ui_begin(joy_ui_ctx_p ctx);
        JOY_API void joy_ui_end(joy_ui_ctx_p ctx);

        /* 输入：从 SDL 事件喂进来。坐标为**窗口坐标**，内部转成逻辑坐标。 */
        JOY_API void joy_ui_input_motion(joy_ui_ctx_p ctx, float win_x, float win_y);
        JOY_API void joy_ui_input_down(joy_ui_ctx_p ctx, int button);
        JOY_API void joy_ui_input_up(joy_ui_ctx_p ctx, int button);
        JOY_API void joy_ui_input_wheel(joy_ui_ctx_p ctx, float dy);
        JOY_API void joy_ui_input_key(joy_ui_ctx_p ctx, bool shift, bool ctrl);

        /* 逻辑鼠标坐标（供调用方自行做命中判断，例如视口拾取） */
        JOY_API void  joy_ui_get_mouse(joy_ui_ctx_p ctx, float* out_x, float* out_y);
        JOY_API bool  joy_ui_is_down(joy_ui_ctx_p ctx);
        JOY_API bool  joy_ui_is_pressed(joy_ui_ctx_p ctx);
        JOY_API bool  joy_ui_is_released(joy_ui_ctx_p ctx);
        JOY_API float joy_ui_take_wheel(joy_ui_ctx_p ctx);   /* 取走并标记已消费 */
        JOY_API bool  joy_ui_is_shift(joy_ui_ctx_p ctx);
        JOY_API bool  joy_ui_is_ctrl(joy_ui_ctx_p ctx);

        /* 裁剪：内部委托 r2d_push/pop_clip_rect，同时记录当前矩形用于命中测试 */
        JOY_API void joy_ui_push_clip(joy_ui_ctx_p ctx, joy_rect_t rect);
        JOY_API void joy_ui_pop_clip(joy_ui_ctx_p ctx);
        JOY_API void joy_ui_reset_clip(joy_ui_ctx_p ctx);
        /* 命中测试：既要在矩形内，也要在当前裁剪区内（否则滚动区域外会误触） */
        JOY_API bool joy_ui_hit(joy_ui_ctx_p ctx, joy_rect_t rect);

        /* 绘制辅助 */
        JOY_API void joy_ui_fill_rect(joy_ui_ctx_p ctx, joy_rect_t r, joy_color_t c);
        JOY_API void joy_ui_stroke_rect(joy_ui_ctx_p ctx, joy_rect_t r, joy_color_t c);
        JOY_API void joy_ui_line(joy_ui_ctx_p ctx, float x1, float y1, float x2, float y2, joy_color_t c);
        JOY_API int  joy_ui_text(joy_ui_ctx_p ctx, float x, float y, const char* s, joy_color_t c);
        JOY_API bool joy_ui_text_measure(joy_ui_ctx_p ctx, const char* s, joy_color_t c, int* out_w, int* out_h);
        JOY_API int  joy_ui_text_centered(joy_ui_ctx_p ctx, joy_rect_t r, const char* s, joy_color_t c);

        /* 面板（标题栏 + 内容裁剪区）*/
        JOY_API void joy_ui_panel_begin(joy_ui_ctx_p ctx, joy_rect_t r, const char* title);
        JOY_API void joy_ui_panel_end(joy_ui_ctx_p ctx);
        JOY_API void joy_ui_panel_frame(joy_ui_ctx_p ctx, joy_rect_t r);

        /* 滚动：id 用于在 ctx 内保存各自的滚动偏移 */
        JOY_API float joy_ui_scroll_begin(joy_ui_ctx_p ctx, int id, joy_rect_t view, float content_h);
        JOY_API void  joy_ui_scroll_end(joy_ui_ctx_p ctx);
        JOY_API void  joy_ui_scrollbar(joy_ui_ctx_p ctx, joy_rect_t view, float content_h, float off);
        /* 强制设置滚动偏移（控制台贴底跟随用）；content_h 传 0 时仅设偏移 */
        JOY_API void  joy_ui_scroll_set_offset(joy_ui_ctx_p ctx, int id, float off, float content_h, float view_h);

        /* 控件：复用 button / checkbox / slider，按 id 池化 */
        JOY_API bool joy_ui_button(joy_ui_ctx_p ctx, int id, joy_rect_t r, const char* text);
        JOY_API bool joy_ui_checkbox(joy_ui_ctx_p ctx, int id, joy_rect_t r, bool* value, const char* label);
        /* rect 是滑轨区域；标签与数值文本由调用方自行绘制（格式各异） */
        JOY_API bool joy_ui_slider(joy_ui_ctx_p ctx, int id, joy_rect_t track, float* value, float minv, float maxv);

#ifdef __cplusplus
}
#endif

#endif /* JOY_UI_IMPL_H */
