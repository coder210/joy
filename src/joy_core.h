/******************************************************************************
 * src/joy_core.h —— core 域声明头（joy_core.c 的配套头）
 * 职责：核心 / 系统工具 / 定时与编码
 * 方向：本头是该域声明的归属地，由 include/joy/japi.h 聚合包含；
 *       本头只依赖基础类型头，不再 include 聚合门面 joy.h，避免循环包含。
 * 生成链：本头是 gen_bindings.js 的 DECL_SOURCES 之一，改动声明后必须跑 --scan。
 ******************************************************************************/
#ifndef JOY_CORE_IMPL_H
#define JOY_CORE_IMPL_H

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


        /* ---- 原 input 域（已并回 core） ---- */
        /* ---- 输入状态快照（B 方案 P1）：宿主把 SDL 事件翻译成标量喂入，
         * 消费方用这组纯标量 getter 查询当前状态，不必接触 SDL_Event。
         * 全部是 L0/L1，生成链可直接产出四端绑定；旧的
         * *_process_event(const void*) 暂时保留，新逻辑应走这组 API。
         * 状态是进程级单例输入流，与 joy 的单窗口模型一致。
         * input 域已并回 core，声明随 src/joy_core.h 聚合；source 行号由 --scan 刷新。 */
        JOY_API void  joy_core_input_feed_key(int scancode, bool down);
        JOY_API void  joy_core_input_feed_mouse_move(float x, float y);
        JOY_API void  joy_core_input_feed_mouse_button(int button, bool down);
        JOY_API void  joy_core_input_feed_wheel(float dy);
        JOY_API void  joy_core_input_end_frame(void);
        JOY_API void  joy_core_input_reset(void);
        JOY_API bool  joy_core_input_is_key_down(int scancode);
        JOY_API bool  joy_core_input_is_mouse_button_down(int button);
        JOY_API float joy_core_input_get_mouse_x(void);
        JOY_API float joy_core_input_get_mouse_y(void);
        JOY_API float joy_core_input_get_wheel(void);
        /* 帧级边沿：本帧是否刚按下 / 抬起，由 end_frame 清零 */
        JOY_API bool  joy_core_input_was_key_pressed(int scancode);
        JOY_API bool  joy_core_input_was_key_released(int scancode);

#ifdef __cplusplus
}
#endif

#endif /* JOY_CORE_IMPL_H */
