/* 公共 API 声明：IDL / thunk 生成器的契约输入源
 * 由 joy.h 按行段拆出，聚合入口见 joy.h；改动请在对应分域头进行。 */
#ifndef JOY_API_H
#define JOY_API_H

#include <joy/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif





        ///////////////////////////////////////////////////////////////////////
        ////////////////////////////  utils  //////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        JOY_API char* joy_core_utils_strdup(const char* str, size_t* out_sz);
        JOY_API bool joy_core_utils_get_current_time(uint64_t* ms_ticks);
        JOY_API void joy_core_utils_current_datetime(const char* fmt, char* buf, size_t len);
        JOY_API char* joy_core_utils_read_file(const char* filename, size_t* sz);
        JOY_API bool joy_core_utils_check_file(const char* filename);
        JOY_API void joy_core_utils_append_file(const char* filename, const char* data);
        JOY_API char* joy_core_utils_read_script(const char* file, size_t* data_size);
        JOY_API short joy_core_utils_bit2short(uint8_t* buf);
        JOY_API void  joy_core_utils_short2bit(uint8_t* buf, short value);
        /* c# = BitConverter.FromUInt32 */
        JOY_API int joy_core_utils_bit2int(uint8_t* buf);
        /* c# = BitConverter.ToUInt32 */
        JOY_API void joy_core_utils_int2bit(uint8_t* buf, int value);
        JOY_API bool joy_core_utils_wait_delay(joy_delay_t* delay, uint64_t current_time);
        JOY_API short joy_core_utils_md5x(const char* input, size_t len, char* output);
        JOY_API char* joy_core_utils_des_crypt(const char* key, const char* text, size_t textsz, size_t* out_len);
        JOY_API char* joy_core_utils_des_decrypt(const char* key, const char* text, size_t textsz, size_t* out_len);

        JOY_API unsigned long joy_core_sdbm_hash(const char* str, int size);
        JOY_API unsigned long joy_core_fnv1a_hash(const char* str, int size);
        JOY_API unsigned long joy_core_jenkins_hash(const char* str, int size);
        JOY_API unsigned long joy_core_murmur3_hash(const char* str);
        JOY_API unsigned long joy_core_kr_hash(const char* str, int size);

        /**
         * 解码 UTF-8 字符串的第一个字符
         * @param s    输入字符串
         * @param len  字符串长度（字节数）
         * @param code 输出参数，存储解码后的 Unicode 码点 (int32_t)
         * @return     成功时返回消耗的字节数（1~4），失败返回 0
         */
        JOY_API size_t joy_core_utf8_decode(const char* s, size_t len, int32_t* code);

        /**
         * 迭代遍历 UTF-8 字符串
         * @param s    输入字符串
         * @param len  字符串长度
         * @param pos  当前位置指针（输入/输出）
         * @param code 输出码点 (int32_t)
         * @return     成功返回 1，结束或错误返回 0
         */
        JOY_API int joy_core_utf8_next(const char* s, size_t len,
                size_t* pos, int32_t* code);

        /**
         * 将整个 UTF-8 字符串解码为码点数组（调用者提供缓冲区）
         * @param s            输入字符串
         * @param len          字符串长度（字节数）
         * @param out_codes    输出缓冲区（接收码点数组），可为 NULL 以查询所需长度
         * @param out_capacity 输出缓冲区的容量（最多能存放的码点个数）
         * @return             成功时返回实际写入的码点个数（若 out_codes==NULL，则返回所需个数）；
         *                     若字符串无效或缓冲区容量不足，返回 0
         */
        JOY_API size_t joy_core_utf8_decode_all(const char* s, size_t len,
                int32_t* out_codes, size_t out_capacity);
        JOY_API int joy_core_pack_int8(char* buf, int8_t value, int offset);
        JOY_API int joy_core_pack_int16(char* buf, int16_t value, int offset);
        JOY_API int joy_core_pack_int32(char* buf, int32_t value, int offset);
        JOY_API int joy_core_pack_uint32(char* buf, uint32_t value, int offset);
        JOY_API int joy_core_pack_int64(char* buf, int64_t value, int offset);
        JOY_API int joy_core_unpack_int8(const char* buf, int8_t* value, int offset);
        JOY_API int joy_core_unpack_int16(const char* buf, int16_t* value, int offset);
        JOY_API int joy_core_unpack_int32(const char* buf, int32_t* value, int offset);
        JOY_API int joy_core_unpack_uint32(const char* buf, uint32_t* value, int offset);
        JOY_API int joy_core_unpack_int64(const char* buf, int64_t* value, int offset);
        JOY_API int joy_core_pack_string(char* buf, const char* value, int type_size, int offset);
        JOY_API int joy_core_unpack_string(const char* buf, char* value, int type_size, int offset);
       
        JOY_API joy_timer_p joy_core_timer_create(int interval, joy_timer_callback cb, void* userdata);
        JOY_API void joy_core_timer_destroy(joy_timer_p timer);
        JOY_API void joy_core_timer_trigger(joy_timer_p timer);
        JOY_API void joy_core_timer_set_interval(joy_timer_p timer, int interval);
        JOY_API void joy_core_timer_set_active(joy_timer_p timer, bool active);
        JOY_API void joy_core_timer_reset(joy_timer_p timer);
        JOY_API uint64_t joy_core_timer_get_elapsed(joy_timer_p timer);

        ///////////////////////////////////////////////////////////////////////
        ////////////////////////////  core  ///////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        JOY_API bool joy_core_init_env();
        JOY_API void joy_core_quit_env(void);
        JOY_API const char* joy_core_get_error(void);
        JOY_API uint64_t joy_core_get_ticks(void);

        /** 从鼠标事件中提取坐标 */
        JOY_API void joy_core_get_mouse_pos(const void* event, float* out_x, float* out_y);
        /** 从鼠标事件中提取按钮（1=左, 2=中, 3=右） */
        JOY_API int  joy_core_get_mouse_button(const void* event);

        JOY_API joy_window_p joy_core_create_window(const char* title, int w, int h);
        JOY_API void joy_core_destroy_window(joy_window_p win);
        JOY_API void* joy_core_get_raw_window(joy_window_p win);
        JOY_API void joy_core_window_set_fullscreen(joy_window_p win, bool fullscreen);
        JOY_API bool joy_core_window_get_fullscreen(joy_window_p win);
        JOY_API void joy_core_window_set_icon(joy_window_p win, const char* png_path);

        JOY_API void joy_core_dispatch_event(joy_event_callbacks_t cbs,
                const void* event, void* arg);
        JOY_API int  joy_core_get_event_type(const void* event);
        JOY_API int  joy_core_get_event_code(const void* event);
        JOY_API bool joy_core_is_user_event(const void* event, int type);
        JOY_API bool joy_core_push_user_event(int type, int code);
        JOY_API bool joy_core_push_quit_event(void);
        JOY_API bool joy_core_should_close(void* ev);

        JOY_API int joy_core_log_info(const char* format, ...);
        JOY_API int joy_core_log_debug(const char* format, ...);
        JOY_API int joy_core_log_error(const char* format, ...);
        JOY_API void joy_core_log_title(int type);

        JOY_API joy_env_p joy_core_env_create(void);
        JOY_API const char* joy_core_env_get(joy_env_p env, const char* key);
        JOY_API void joy_core_env_set(joy_env_p env, const char* key, const char* value);
        JOY_API void joy_core_env_destroy(joy_env_p env);

        JOY_API joy_simple_fps_counter_p joy_core_simple_fps_create();
        JOY_API void joy_core_simple_fps_destory(joy_simple_fps_counter_p fps);
        JOY_API void joy_core_simple_fps_update(joy_simple_fps_counter_p fps);
        JOY_API int joy_core_simple_fps_value(joy_simple_fps_counter_p fps);

        JOY_API void joy_core_game_timer_init(joy_game_timer_p clock);
        JOY_API void joy_core_game_timer_update(joy_game_timer_p clock);
        JOY_API float joy_core_game_timer_get_delta_time(const joy_game_timer_p clock);
        JOY_API float joy_core_game_timer_get_unscaled_delta_time(const joy_game_timer_p clock);
        JOY_API void joy_core_game_timer_set_time_scale(joy_game_timer_p clock, float scale);
        JOY_API float joy_core_game_timer_get_time_scale(const joy_game_timer_p clock);
        JOY_API void joy_core_game_timer_set_target_fps(joy_game_timer_p clock, int fps);
        JOY_API int joy_core_game_timer_get_target_fps(const joy_game_timer_p clock);

        /** 获取剪贴板文本（调用者负责释放返回的字符串） */
        JOY_API char* joy_core_get_clipboard_text(void);
        /** 设置剪贴板文本 */
        JOY_API void joy_core_set_clipboard_text(const char* text);


        JOY_API bool joy_net_sys_init_netenv(void);
        JOY_API bool joy_net_sys_release_netenv(void);
        JOY_API int joy_net_get_local_ips(char ips[][JOY_MAX_IP], int max_count);
        JOY_API int joy_net_sys_anyaddr(void);
        JOY_API bool joy_net_sys_closesocket(int64_t sockfd);
        JOY_API int joy_net_sys_set_sock_sndtimeo(int64_t sockfd, int ms);
        JOY_API int joy_net_sys_set_sock_rcvtimeo(int64_t sockfd, int ms);
        JOY_API int joy_net_sys_set_sock_accpettimeo(int64_t sockfd, int ms);
        /* UDP 广播 / 地址复用开关（局域网房间发现用）：成功返回 0 */
        JOY_API int joy_net_sys_set_sock_broadcast(int64_t sockfd);
        JOY_API int joy_net_sys_set_sock_reuseaddr(int64_t sockfd);
        JOY_API int64_t joy_net_sys_tcp();
        JOY_API int64_t joy_net_sys_udp();
        JOY_API bool joy_net_sys_bind(int64_t sockfd, const char* ip, int port);
        JOY_API bool joy_net_sys_connect(int64_t sockfd, const char* ip, int port);
        JOY_API bool joy_net_sys_listen(int64_t sockfd);
        JOY_API int64_t joy_net_sys_accept(int64_t sockfd, char* ip, int* port);
        JOY_API int joy_net_sys_send(int64_t sockfd, const char* buf, int len);
        JOY_API int joy_net_sys_recv(int64_t sockfd, char* buf);
        JOY_API int joy_net_sys_recvfrom(int64_t sockfd, char* buf, char* ip, int* port);
        JOY_API int joy_net_sys_sendto(int64_t sockfd, const char* buf,
                int len, const char* ip, int port);
        JOY_API int64_t joy_net_sys_current_time();
        JOY_API void joy_net_sys_sleep(int ms);
        JOY_API void joy_net_sys_install_crash_handlers();

        JOY_API joy_iocp_server_p joy_net_iocp_server_create(const char* bind_ip, int bind_port);
        JOY_API void joy_net_iocp_server_destroy(joy_iocp_server_p server);
        JOY_API void joy_net_iocp_process_events(joy_iocp_server_p server, int timeout_ms);


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
        /* 2D 视野绘制: 在像素平面构造 A/B/C 三环羽化多边形, 复刻 3D 的渐变纹理软边。
           px/py 为观察点在输出像素坐标, radii[seg] 每方向可见半径(像素),
           feather/outer_radius 亦为像素。需在 joy_r2d_begin/joy_r2d_end 与地图同一坐标系(相机相对像素)下调用。
           射线遮挡仍用上方的 joy_r3d_fov_compute_radii 计算(其 px/pz 即 2D 的 x/y。若担心模板冲突,
           2D 侧也可直接以世界米为单位计算后乘以 PPM 得到像素半径)。 */
        JOY_API void joy_r2d_fov_draw_2d(joy_r2d_p r, float px, float py,
                const float* radii, int seg, float feather, float outer_radius);

        ///////////////////////////////////////////////////////////////////////
       ////////////////////////////  collision ///////////////////////////////
       ///////////////////////////////////////////////////////////////////////


        JOY_API joy_ray2d_collision_t joy_math_collision2d_get_ray_circle(joy_ray2d_t ray, joy_circle_t circle);
        JOY_API joy_ray2d_collision_t joy_math_collision2d_get_ray_rectangle(joy_ray2d_t ray, joy_rectangle_t rect);
        JOY_API joy_ray2d_collision_t joy_math_collision2d_get_ray_rectanglex(joy_ray2d_t ray, joy_rectangle_t rect, joy_ft_t angle);
        JOY_API joy_ray2d_collision_t joy_math_collision2d_get_ray_polygon(joy_ray2d_t ray, joy_polygon_t polygon);
        JOY_API bool joy_math_collision2d_check_point_rectangle(joy_vec2_t point, joy_rectangle_t rect);
        JOY_API bool joy_math_collision2d_check_point_circle(joy_vec2_t point, joy_circle_t circle);
        JOY_API joy_ft_t joy_math_collision2d_get_squared_distance_point_circle(joy_vec2_t point, joy_circle_t circle);
        JOY_API bool joy_math_collision2d_check_point_triangle(joy_vec2_t point, joy_vec2_t p1, joy_vec2_t p2, joy_vec2_t p3);
        JOY_API bool joy_math_collision2d_check_lines(joy_vec2_t a1, joy_vec2_t b1, joy_vec2_t a2, joy_vec2_t b2, joy_vec2_t* cp);
        JOY_API bool joy_math_collision2d_check_point_line(joy_vec2_t point, joy_vec2_t p1, joy_vec2_t p2, joy_ft_t threshold);
        JOY_API bool joy_math_collision2d_check_circles(joy_circle_t a, joy_circle_t b);
        JOY_API bool joy_math_collision2d_check_rectangles(joy_rectangle_t a, joy_rectangle_t b);
        JOY_API bool joy_math_collision2d_check_polygons(joy_polygon_t a, joy_polygon_t b);
        JOY_API bool joy_math_collision2d_check_circle_rectangle(joy_circle_t a, joy_rectangle_t b);
        JOY_API bool joy_math_collision2d_check_circle_polygon(joy_circle_t a, joy_polygon_t b);
        JOY_API int joy_math_collision2d_get_circles(joy_circle_t a, joy_circle_t b, joy_contact2d_t* contact);
        JOY_API int joy_math_collision2d_get_rectangles(joy_rectangle_t a, joy_ft_t a_angle, joy_rectangle_t b, joy_ft_t b_angle, joy_contact2d_t* contact);
        JOY_API int joy_math_collision2d_get_polygons(joy_polygon_t a, joy_polygon_t b, joy_contact2d_t* contact);
        JOY_API joy_ray3d_collision_t joy_math_collision3d_get_ray_triangle(joy_ray3d_t ray, joy_vec3_t p1, joy_vec3_t p2, joy_vec3_t p3);
        JOY_API joy_ray3d_collision_t joy_math_collision3d_get_ray_quad(joy_ray3d_t ray, joy_vec3_t p1, joy_vec3_t p2, joy_vec3_t p3, joy_vec3_t p4);
        JOY_API joy_ray3d_collision_t joy_math_collision3d_get_ray_sphere(joy_ray3d_t ray, joy_sphere_t sphere);
        JOY_API joy_ray3d_collision_t joy_math_collision3d_get_ray_box(joy_ray3d_t ray, joy_bounding_box_t box);
        JOY_API bool joy_math_collision3d_check_spheres(joy_sphere_t a, joy_sphere_t b);
        JOY_API bool joy_math_collision3d_check_boxes(joy_bounding_box_t a, joy_bounding_box_t b);
        JOY_API bool joy_math_collision3d_check_box_sphere(joy_bounding_box_t a, joy_sphere_t b);

        JOY_API joy_ray2d_collisionf_t joy_math_collision2df_get_ray_circle(joy_ray2df_t ray,
                joy_circlef_t circle);
        JOY_API joy_ray2d_collisionf_t joy_math_collision2df_get_ray_rectangle(joy_ray2df_t ray,
                joy_rectanglef_t rect);
        JOY_API joy_ray2d_collisionf_t joy_math_collision2df_get_ray_rectanglex(joy_ray2df_t ray,
                joy_rectanglef_t rect, joy_fp_t angle /* 旋转角, 单位: 度 */);
        JOY_API joy_ray2d_collisionf_t joy_math_collision2df_get_ray_polygon(joy_ray2df_t ray,
                joy_polygonf_t polygon);
        JOY_API bool joy_math_collision2df_check_point_rectangle(joy_vec2f_t point,
                joy_rectanglef_t rect);
        JOY_API bool joy_math_collision2df_check_point_circle(joy_vec2f_t point, joy_circlef_t circle);
        JOY_API joy_fp_t joy_math_collision2df_get_squared_distance_point_circle(joy_vec2f_t point, joy_circlef_t circle);
        JOY_API bool joy_math_collision2df_check_point_triangle(joy_vec2f_t point,
                joy_vec2f_t p1, joy_vec2f_t p2, joy_vec2f_t p3);
        JOY_API bool joy_math_collision2df_check_lines(joy_vec2f_t a1, joy_vec2f_t b1,
                joy_vec2f_t a2, joy_vec2f_t b2, joy_vec2f_t* cp);
        JOY_API bool joy_math_collision2df_check_point_line(joy_vec2f_t point,
                joy_vec2f_t p1, joy_vec2f_t p2, joy_fp_t threshold);
        JOY_API bool joy_math_collision2df_check_circles(joy_circlef_t a, joy_circlef_t b);
        JOY_API bool joy_math_collision2df_check_rectangles(joy_rectanglef_t a, joy_rectanglef_t b);
        JOY_API bool joy_math_collision2df_check_polygons(joy_polygonf_t a, joy_polygonf_t b);
        JOY_API bool joy_math_collision2df_check_circle_rectangle(joy_circlef_t a, joy_rectanglef_t b);
        JOY_API bool joy_math_collision2df_check_circle_polygon(joy_circlef_t a, joy_polygonf_t b);
        JOY_API int joy_math_collision2df_get_circles(joy_circlef_t a, joy_circlef_t b,
                joy_contact2df_t* contact);
        JOY_API int joy_math_collision2df_get_rectangles(joy_rectanglef_t a, joy_fp_t a_angle /* 度 */,
                joy_rectanglef_t b, joy_fp_t b_angle /* 度 */,
                joy_contact2df_t* contact);
        JOY_API joy_fp_t joy_math_find_min_separation(joy_polygonf_t a, joy_polygonf_t b,
                joy_vec2f_t* axis, joy_vec2f_t* point);
        JOY_API int joy_math_collision2df_get_polygons(joy_polygonf_t a, joy_polygonf_t b,
                joy_contact2df_t* contact);

        /* OBB（旋转矩形）沿 (dx,dy) 平移的「扫掠体」-> 凸包顶点（世界坐标）。
         *
         * 注意输出一般是六边形而不是矩形：矩形斜向平移时，起点盒与终点盒的凸包
         * 有 6 个顶点；只有位移平行于矩形某条边时才退化为矩形（4 顶点）。
         * 故这里以多边形输出，调用方缓冲区 out 容量至少 8。
         *
         * center     : OBB 中心（起点位置；终点 = center + (dx,dy)）
         * axis       : 长轴单位向量（短轴取其逆时针 90°，无需三角函数）
         * half_long  : 沿长轴的半长
         * half_short : 沿短轴的半宽
         * dx, dy     : 本步位移
         * out        : 输出顶点（容量 >= 8）
         * out_count  : 输出顶点数（4~8）
         *
         * 绕序：统一输出为「y 向下坐标系下的顺时针」，此时 vec2f_normal 给出外法线，
         * 与 joy_math_find_min_separation 的约定一致 —— 绕序反了会让分离距离符号反转、
         * 判定结果完全相反。
         *
         * 典型用法：把 out/out_count 包成 joy_polygonf_t，再交给 joy_math_collision2df_check_polygons。 */
        JOY_API void joy_math_collision2df_swept_obb(joy_vec2f_t center, joy_vec2f_t axis,
                joy_fp_t half_long, joy_fp_t half_short, joy_fp_t dx, joy_fp_t dy,
                joy_vec2f_t* out, int* out_count);

        /* ---- 瓦片地图扫掠碰撞（定点数，帧同步确定）----
         * solid(col,row) 返回 true 表示该格不可通行；实现须对越界格也返回 true（视为墙）。
         * joy_math_tilemap_sweep_x: 沿 X 推进 dx（可负）；撞墙则把 *px 贴到该格边界并返回 true。
         * joy_math_tilemap_sweep_y: 沿 Y 推进 vy*tick（vy>0 下落、<0 上升）；撞到则把 *py 贴到格边界、
         *                      并把 *vy 清零，返回 true。 */
        typedef bool (*joy_tilemap_solid_fn)(int col, int row);
        JOY_API bool joy_math_tilemap_sweep_x(joy_fp_t* px, joy_fp_t* py, joy_fp_t dx, joy_fp_t w, joy_fp_t h,
                joy_tilemap_solid_fn solid);
        JOY_API bool joy_math_tilemap_sweep_y(joy_fp_t* px, joy_fp_t* py, joy_fp_t* vy, joy_fp_t tick, joy_fp_t w, joy_fp_t h,
                joy_tilemap_solid_fn solid);

        JOY_API joy_ray3d_collisionf_t joy_math_collision3df_get_ray_triangle(joy_ray3df_t ray,
                joy_vec3f_t p1, joy_vec3f_t p2, joy_vec3f_t p3);
        JOY_API joy_ray3d_collisionf_t joy_math_collision3df_get_ray_quad(joy_ray3df_t ray,
                joy_vec3f_t p1, joy_vec3f_t p2, joy_vec3f_t p3, joy_vec3f_t p4);
        JOY_API joy_ray3d_collisionf_t joy_math_collision3df_get_ray_sphere(joy_ray3df_t ray,
                joy_spheref_t sphere);
        JOY_API joy_ray3d_collisionf_t joy_math_collision3df_get_ray_box(joy_ray3df_t ray,
                joy_bounding_boxf_t box);
        /* OBB(旋转盒)射线检测: position=中心, half_size=半边长, rotation=欧拉角(弧度) */
        JOY_API joy_ray3d_collisionf_t joy_math_collision3df_get_ray_obb(joy_ray3df_t ray,
                joy_vec3f_t position, joy_vec3f_t half_size, joy_vec3f_t rotation);
        JOY_API bool joy_math_collision3df_check_spheres(joy_spheref_t a, joy_spheref_t b);
        JOY_API bool joy_math_collision3df_check_boxes(joy_bounding_boxf_t a,
                joy_bounding_boxf_t b);
        JOY_API bool joy_math_collision3df_check_box_sphere(joy_bounding_boxf_t a,
                joy_spheref_t b);


        /* 主碰撞检测: GJK + EPA, 返回是否碰撞, contact填充法线和深度 */
        JOY_API bool joy_math_gjk2d_collide(joy_gjk2d_collider_p c1, joy_gjk2d_collider_p c2,
                joy_vec2f_t init_dir, joy_gjk2d_contact_p contact);
        JOY_API void joy_math_gjk2d_init_circle(joy_gjk2d_collider_t* c, joy_vec2f_t pos, joy_fp_t radius);
        JOY_API void joy_math_gjk2d_init_box(joy_gjk2d_collider_t* c, joy_vec2f_t pos, joy_vec2f_t half_size);
        JOY_API void joy_math_gjk2d_init_capsule(joy_gjk2d_collider_t* c, joy_vec2f_t pos, joy_vec2f_t axis, joy_fp_t height, joy_fp_t radius);
        JOY_API void joy_math_gjk2d_init_polygon(joy_gjk2d_collider_t* c, joy_vec2f_t* vertices, int num);
        JOY_API void joy_math_gjk2d_init_pie(joy_gjk2d_collider_t* c, joy_vec2f_t pos, joy_fp_t radius, joy_fp_t sweep);
        JOY_API bool joy_math_gjk3df_collide(joy_gjk3df_collider_p c1, joy_gjk3df_collider_p c2,
                joy_vec3f_t init_dir, joy_gjk3df_contact_p contact);
        /* 快捷初始化函数(仅设置参数,无内存分配) */
        JOY_API void joy_math_gjk3df_init_sphere(joy_gjk3df_collider_t* c, joy_vec3f_t pos, joy_fp_t radius);
        JOY_API void joy_math_gjk3df_init_box(joy_gjk3df_collider_t* c, joy_vec3f_t pos, joy_vec3f_t half_size);
        JOY_API void joy_math_gjk3df_init_obb(joy_gjk3df_collider_t* c, joy_vec3f_t pos, joy_vec3f_t half_size, joy_vec3f_t rotation);
        JOY_API void joy_math_gjk3df_init_capsule(joy_gjk3df_collider_t* c, joy_vec3f_t pos, joy_fp_t radius, joy_fp_t height, joy_vec3f_t axis);
        JOY_API void joy_math_gjk3df_init_cylinder(joy_gjk3df_collider_t* c, joy_vec3f_t pos, joy_fp_t radius, joy_fp_t height);
        JOY_API void joy_math_gjk3df_init_cone(joy_gjk3df_collider_t* c, joy_vec3f_t pos, joy_fp_t radius, joy_fp_t height);
        JOY_API void joy_math_gjk3df_init_mesh(joy_gjk3df_collider_t* c, joy_vec3f_t* vertices, int num);

        /* ============ GJK 3D 浮点版 (joy_ft_t/joy_vec3_t) ============ */
        /* 主碰撞检测: GJK + EPA, 返回是否碰撞, contact填充法线和深度 */
        JOY_API bool joy_math_gjk3d_collide(joy_gjk3d_collider_p c1, joy_gjk3d_collider_p c2,
                joy_vec3_t init_dir, joy_gjk3d_contact_p contact);
        /* 快捷初始化函数(仅设置参数,无内存分配) */
        JOY_API void joy_math_gjk3d_init_sphere(joy_gjk3d_collider_t* c, joy_vec3_t pos, joy_ft_t radius);
        JOY_API void joy_math_gjk3d_init_box(joy_gjk3d_collider_t* c, joy_vec3_t pos, joy_vec3_t half_size);
        JOY_API void joy_math_gjk3d_init_capsule(joy_gjk3d_collider_t* c, joy_vec3_t pos, joy_ft_t radius, joy_ft_t height, joy_vec3_t axis);
        JOY_API void joy_math_gjk3d_init_cylinder(joy_gjk3d_collider_t* c, joy_vec3_t pos, joy_ft_t radius, joy_ft_t height);
        JOY_API void joy_math_gjk3d_init_cone(joy_gjk3d_collider_t* c, joy_vec3_t pos, joy_ft_t radius, joy_ft_t height);
        JOY_API void joy_math_gjk3d_init_mesh(joy_gjk3d_collider_t* c, joy_vec3_t* vertices, int num);

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

        // ==============================
        // KCP Server & Client
        // ==============================
        JOY_API joy_kcpserver_p joy_net_kcpserver_create(const char* ip, int port);
        JOY_API int joy_net_kcpserver_destroy(joy_kcpserver_p ks);
        JOY_API void joy_net_kcpserver_send(joy_kcpserver_p ks, int conv, const char* data, int len);
        JOY_API void joy_net_kcpserver_broadcast(joy_kcpserver_p ks, const char* data, int len);
        JOY_API void joy_net_kcpserver_offline(joy_kcpserver_p ks, int conv);
        JOY_API void joy_net_kcpserver_update(joy_kcpserver_p ks);
        JOY_API int joy_net_kcpserver_connection_count(joy_kcpserver_p ks);
        JOY_API bool joy_net_kcpserver_poll_message(joy_kcpserver_p ks, joy_net_message_p msg);
        JOY_API void joy_net_kcpserver_set_callback(joy_kcpserver_p ks, joy_net_callback cb, void* userdata);
        JOY_API joy_kcpclient_p joy_net_kcpclient_create(const char* ip, int port);
        JOY_API void joy_net_kcpclient_destroy(joy_kcpclient_p kc);
        JOY_API int joy_net_kcpclient_send(joy_kcpclient_p kc, const char* data, int len);
        JOY_API void joy_net_kcpclient_update(joy_kcpclient_p kc);
        JOY_API bool joy_net_kcpclient_poll_message(joy_kcpclient_p kc, joy_net_message_p msg);
        JOY_API void joy_net_kcpclient_set_callback(joy_kcpclient_p kc, joy_net_callback cb, void* userdata);
        JOY_API joy_tcpserver_p joy_net_tcpserver_create(const char* ip, int port);
        JOY_API int joy_net_tcpserver_destroy(joy_tcpserver_p tcpserver);
        JOY_API void joy_net_tcpserver_send(joy_tcpserver_p tcpserver, int conv, const char* data, int len);
        JOY_API void joy_net_tcpserver_broadcast(joy_tcpserver_p tcpserver, const char* data, int len);
        JOY_API void joy_net_tcpserver_offline(joy_tcpserver_p tcpserver, int conv);
        JOY_API void joy_net_tcpserver_update(joy_tcpserver_p tcpserver);
        JOY_API int joy_net_tcpserver_connection_count(joy_tcpserver_p tcpserver);
        JOY_API bool joy_net_tcpserver_poll_message(joy_tcpserver_p tcpserver, joy_net_message_p msg);
        JOY_API joy_tcpclient_p joy_net_tcpclient_create(const char* ip, int port);
        JOY_API void joy_net_tcpclient_destroy(joy_tcpclient_p tcpclient);
        JOY_API int joy_net_tcpclient_send(joy_tcpclient_p tcpclient, const char* data, int len);
        JOY_API void joy_net_tcpclient_update(joy_tcpclient_p tcpclient);
        JOY_API bool joy_net_tcpclient_poll_message(joy_tcpclient_p tcpclient, joy_net_message_p msg);
        JOY_API joy_wsnetserver_p joy_net_wsnetserver_create(const char* ip, int port);
        JOY_API int joy_net_wsnetserver_destroy(joy_wsnetserver_p ws);
        JOY_API void joy_net_wsnetserver_send(joy_wsnetserver_p ws, int conv, const char* data, int len);
        JOY_API void joy_net_wsnetserver_broadcast(joy_wsnetserver_p ws, const char* data, int len);
        JOY_API void joy_net_wsnetserver_offline(joy_wsnetserver_p ws, int conv);
        JOY_API void joy_net_wsnetserver_update(joy_wsnetserver_p ws);
        JOY_API int joy_net_wsnetserver_connection_count(joy_wsnetserver_p ws);
        JOY_API bool joy_net_wsnetserver_poll_message(joy_wsnetserver_p ws, joy_net_message_p msg);
        JOY_API void joy_net_wsnetserver_set_callback(joy_wsnetserver_p ws, joy_net_callback cb, void* userdata);
        JOY_API joy_wsnetclient_p joy_net_wsnetclient_create(const char* url);
        JOY_API void joy_net_wsnetclient_destroy(joy_wsnetclient_p wc);
        JOY_API int joy_net_wsnetclient_send(joy_wsnetclient_p wc, const char* data, int len);
        JOY_API void joy_net_wsnetclient_update(joy_wsnetclient_p wc);
        JOY_API bool joy_net_wsnetclient_poll_message(joy_wsnetclient_p wc, joy_net_message_p msg);
        JOY_API void joy_net_wsnetclient_set_callback(joy_wsnetclient_p wc, joy_net_callback cb, void* userdata);
        JOY_API joy_netclient_p joy_net_netclient_create(joy_net_client_type type, const char* host, int port);
        JOY_API void joy_net_netclient_destroy(joy_netclient_p nc);
        JOY_API int joy_net_netclient_send(joy_netclient_p nc, const char* data, int len);
        JOY_API void joy_net_netclient_update(joy_netclient_p nc);
        JOY_API bool joy_net_netclient_poll_message(joy_netclient_p nc, joy_net_message_p msg);
        JOY_API void joy_net_netclient_set_callback(joy_netclient_p nc, joy_net_callback cb, void* userdata);
        JOY_API joy_net_client_type joy_net_netclient_get_type(joy_netclient_p nc);
        JOY_API joy_netserver_p joy_net_netserver_create(joy_net_server_type type, const char* ip, int port);
        JOY_API void joy_net_netserver_destroy(joy_netserver_p ns);
        JOY_API void joy_net_netserver_send(joy_netserver_p ns, int conv, const char* data, int len);
        JOY_API void joy_net_netserver_broadcast(joy_netserver_p ns, const char* data, int len);
        JOY_API void joy_net_netserver_offline(joy_netserver_p ns, int conv);
        JOY_API void joy_net_netserver_update(joy_netserver_p ns);
        JOY_API int joy_net_netserver_connection_count(joy_netserver_p ns);
        JOY_API bool joy_net_netserver_poll_message(joy_netserver_p ns, joy_net_message_p msg);
        JOY_API void joy_net_netserver_set_callback(joy_netserver_p ns, joy_net_callback cb, void* userdata);
        JOY_API joy_net_server_type joy_net_netserver_get_type(joy_netserver_p ns);
        JOY_API void joy_net_netmessage_free_data(joy_net_message_p msg);


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


        /* ===== 运行与生命周期管理 ===== */
        joy_bt_status_t joy_scene_bt_node_run(joy_bt_node_t* node, float dt);
        void        joy_scene_bt_node_reset(joy_bt_node_t* node);
        void        joy_scene_bt_node_destroy(joy_bt_node_t* node);
        /* Action: 执行动作，返回 SUCCESS / FAILURE / RUNNING */
        joy_bt_node_t* joy_scene_bt_action_create(joy_bt_action_fn fn);
        joy_bt_node_t* joy_scene_bt_action_createv(joy_bt_action_fn fn, void* userdata);
        void joy_scene_bt_action_set_userdata(joy_bt_node_t* node, void* userdata);
        void* joy_scene_bt_action_get_userdata(joy_bt_node_t* node);
        /* Condition: 条件判断，true → SUCCESS, false → FAILURE */
        joy_bt_node_t* joy_scene_bt_condition_create(joy_bt_condition_fn pred);
        /* Wait: 等待指定秒数 */
        joy_bt_node_t* joy_scene_bt_wait_create(float seconds);
        /* ===== 组合节点 ===== */
        /* Sequence: 顺序执行全部子节点，全部成功才返回 SUCCESS */
        joy_bt_node_t* joy_scene_bt_sequence_create(joy_bt_node_t** children, int count);
        /* Selector: 顺序尝试子节点，任一成功即返回 SUCCESS */
        joy_bt_node_t* joy_scene_bt_selector_create(joy_bt_node_t** children, int count);
        /* Parallel: 并行执行，success_count 个成功即成功 */
        joy_bt_node_t* joy_scene_bt_parallel_create(joy_bt_node_t** children, int count, int success_count);
        /* Inverter: 反转子节点结果 */
        joy_bt_node_t* bt_inverter_create(joy_bt_node_t* child);
        /* Succeeder: 总是返回 SUCCESS */
        joy_bt_node_t* bt_succeeder_create(joy_bt_node_t* child);
        /* Repeater: 重复子节点 times 次（-1 无限） */
        joy_bt_node_t* bt_repeater_create(joy_bt_node_t* child, int times);
        joy_bt_node_t* joy_scene_bt_statemachine_create(joy_bt_state_slot_t* states, int count, const char* initial);
        void       joy_scene_bt_statemachine_transition(joy_bt_node_t* node, const char* state_name);


        JOY_API uint32_t joy_audio_open_device();
        JOY_API void joy_audio_close_device(uint32_t device_id);
        JOY_API bool joy_audio_create_wav(const char* filepath, uint32_t device_id);
        JOY_API bool joy_audio_create_mp3(const char* filepath, uint32_t device_id);
        JOY_API bool joy_audio_create_flac(const char* filepath, uint32_t device_id);
        JOY_API bool joy_audio_create_ogg(const char* filepath, uint32_t device_id);
        JOY_API joy_audio_shot_p joy_audio_shot_create(const char* wav_path, uint32_t device_id);
        JOY_API joy_audio_shot_p joy_audio_shot_create_mp3(const char* path, uint32_t device_id);
        JOY_API joy_audio_shot_p joy_audio_shot_create_flac(const char* path, uint32_t device_id);
        JOY_API joy_audio_shot_p joy_audio_shot_create_ogg(const char* path, uint32_t device_id);
        JOY_API void joy_audio_shot_play(joy_audio_shot_p shot);
        JOY_API void joy_audio_shot_set_gain(joy_audio_shot_p shot, float gain);
        JOY_API void joy_audio_shot_destroy(joy_audio_shot_p shot);
        JOY_API joy_audio_bgm_p joy_audio_bgm_create(const char* wav_path, uint32_t device_id);
        JOY_API joy_audio_bgm_p joy_audio_bgm_create_mp3(const char* path, uint32_t device_id);
        JOY_API joy_audio_bgm_p joy_audio_bgm_create_flac(const char* path, uint32_t device_id);
        JOY_API joy_audio_bgm_p joy_audio_bgm_create_ogg(const char* path, uint32_t device_id);
        JOY_API void joy_audio_bgm_stop(joy_audio_bgm_p bgm);
        JOY_API void joy_audio_bgm_set_gain(joy_audio_bgm_p bgm, float gain);
        JOY_API void joy_audio_bgm_destroy(joy_audio_bgm_p bgm);

#ifdef JOY_USE_PLUGINS  /* 见 JoySources.cmake 的可选适配层；源码在 jmodule.c */
        JOY_API joy_modules_p joy_script_module_create(const char* path);
        JOY_API void joy_script_module_destroy(joy_modules_p modules);
        JOY_API bool joy_script_module_query(joy_modules_p modules, const char* name, joy_module_p* mod);
        JOY_API void joy_script_module_register(joy_modules_p modules, const char* name,
                joy_module_create_t create,
                joy_module_init_t init,
                joy_module_event_t event_handler,
                joy_module_update_t update,
                joy_module_release_t release);
        JOY_API void joy_script_module_require(joy_modules_p modules, const char* name);
        JOY_API bool joy_script_modules_retire(joy_modules_p modules, const char* name, joy_module_p* mod);
        JOY_API void* joy_script_module_instance_create(joy_module_p mod);
        JOY_API void joy_script_module_instance_init(joy_module_p mod, void* inst, void* ctx, const char* param);
        JOY_API void joy_script_module_instance_event(joy_module_p mod, void* inst, void* event);
        JOY_API void joy_script_module_instance_update(joy_module_p mod, void* inst, float delta_time);
        JOY_API void joy_script_module_instance_release(joy_module_p mod, void* inst);
#endif /* JOY_USE_PLUGINS */

        JOY_API joy_box2d_world_t* joy_math_box2d_create_world(joy_vec2f_t gravity, int iterations);
        JOY_API void joy_math_box2d_clear_world(joy_box2d_world_t* world);
        JOY_API void joy_math_box2d_destroy_world(joy_box2d_world_t* world);
        JOY_API void joy_math_box2d_step(joy_box2d_world_t* world, joy_fp_t dt);
        JOY_API joy_box2d_body_t* joy_math_box2d_create_body(joy_box2d_world_t* world, joy_fp_t mass, joy_fp_t bx, joy_fp_t by);
        JOY_API void joy_math_box2d_destroy_body(joy_box2d_world_t* world, int id);
        JOY_API joy_box2d_joint_t* joy_math_box2d_create_joint(joy_box2d_world_t* world, int id1, int id2, joy_vec2f_t anchor);
        JOY_API void joy_math_box2d_destroy_joint(joy_box2d_world_t* world, joy_box2d_joint_t* joint);
        JOY_API void joy_math_box2d_foreach_bodies(joy_box2d_world_t* world, void (*iterator)(joy_box2d_body_t*));
        JOY_API void joy_math_box2d_foreach_joints(joy_box2d_world_t* world, void (*iterator)(joy_box2d_joint_t*, joy_box2d_body_t*, joy_box2d_body_t*));
        JOY_API void joy_math_box2d_foreach_arbiters(joy_box2d_world_t* world, void (*iterator)(joy_box2d_arbiter_t*));

        JOY_API void joy_script_msgpack_packer_init(joy_msgpack_packer_t* pk, void* data, joy_msgpack_write_fn cb);
        JOY_API int  joy_script_msgpack_pack_int(joy_msgpack_packer_t* pk, int d);
        JOY_API int  joy_script_msgpack_pack_int64(joy_msgpack_packer_t* pk, int64_t d);
        JOY_API int  joy_script_msgpack_pack_uint(joy_msgpack_packer_t* pk, unsigned int d);
        JOY_API int  joy_script_msgpack_pack_uint64(joy_msgpack_packer_t* pk, uint64_t d);
        JOY_API int  joy_script_msgpack_pack_nil(joy_msgpack_packer_t* pk);
        JOY_API int  joy_script_msgpack_pack_array(joy_msgpack_packer_t* pk, size_t n);
        JOY_API int  joy_script_msgpack_pack_str(joy_msgpack_packer_t* pk, size_t l);
        JOY_API int  joy_script_msgpack_pack_str_body(joy_msgpack_packer_t* pk, const void* b, size_t l);
        JOY_API void joy_script_msgpack_unpacked_init(joy_msgpack_unpacked_t* result);
        JOY_API void joy_script_msgpack_unpacked_destroy(joy_msgpack_unpacked_t* result);
        /* 返回值: >0 成功, 0 需要更多数据, <0 错误 */
        JOY_API int  joy_script_msgpack_unpack_next(joy_msgpack_unpacked_t* result, const char* data, size_t len, size_t* off);

        /* ---- joy_msgpack_object_t 不透明访问器（替代直接读结构体字段） ---- */
        JOY_API const joy_msgpack_object_t* joy_script_msgpack_unpacked_data(const joy_msgpack_unpacked_t* u);
        JOY_API joy_msgpack_object_type_t joy_script_msgpack_object_type(const joy_msgpack_object_t* o);
        JOY_API uint32_t             joy_script_msgpack_object_array_size(const joy_msgpack_object_t* o);
        JOY_API const joy_msgpack_object_t* joy_script_msgpack_object_array_at(const joy_msgpack_object_t* o, uint32_t i);
        JOY_API uint32_t             joy_script_msgpack_object_str_size(const joy_msgpack_object_t* o);
        JOY_API const char*          joy_script_msgpack_object_str_ptr(const joy_msgpack_object_t* o);
        JOY_API uint64_t             joy_script_msgpack_object_u64(const joy_msgpack_object_t* o);
        JOY_API int64_t              joy_script_msgpack_object_i64(const joy_msgpack_object_t* o);
        JOY_API bool                 joy_script_msgpack_object_boolean(const joy_msgpack_object_t* o);
        JOY_API double               joy_script_msgpack_object_f64(const joy_msgpack_object_t* o);

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

        /* 生成链路专用：joy_r3d_mesh_destroy 的单参数版本，供 IDL 的 owns 使用。
         * render_mesh 内部记了创建它的 r3d_p（owner），所以 dtor 只需一个参数，
         * 能通过 ownsIssue() 的「单参数、参数类型与 ret 严格相同」硬校验。
         * 约束：mesh 句柄不得比创建它的 r3d 上下文活得久，否则 owner 悬空。
         * 声明放在文件末尾，避免打乱 joy.idl 既有条目的 source 行号。 */
        JOY_API void joy_r3d_mesh_destroy_one(joy_render_mesh_p m);

        /* ---- 输入状态快照（B 方案 P1）：宿主把 SDL 事件翻译成标量喂入，
         * 消费方用这组纯标量 getter 查询当前状态，不必接触 SDL_Event。
         * 全部是 L0/L1，生成链可直接产出四端绑定；旧的
         * *_process_event(const void*) 暂时保留，新逻辑应走这组 API。
         * 状态是进程级单例输入流，与 joy 的单窗口模型一致。
         * 声明放在文件末尾，避免打乱 joy.idl 既有条目的 source 行号。 */
        JOY_API void  joy_input_feed_key(int scancode, bool down);
        JOY_API void  joy_input_feed_mouse_move(float x, float y);
        JOY_API void  joy_input_feed_mouse_button(int button, bool down);
        JOY_API void  joy_input_feed_wheel(float dy);
        JOY_API void  joy_input_end_frame(void);
        JOY_API void  joy_input_reset(void);
        JOY_API bool  joy_input_is_key_down(int scancode);
        JOY_API bool  joy_input_is_mouse_button_down(int button);
        JOY_API float joy_input_get_mouse_x(void);
        JOY_API float joy_input_get_mouse_y(void);
        JOY_API float joy_input_get_wheel(void);
        /* 帧级边沿：本帧是否刚按下 / 抬起，由 end_frame 清零 */
        JOY_API bool  joy_input_was_key_pressed(int scancode);
        JOY_API bool  joy_input_was_key_released(int scancode);

#ifdef __cplusplus
}
#endif

#endif /* JOY_API_H */
