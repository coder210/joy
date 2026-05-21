/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: utils.h
Description: ͨ�÷���
Author: ydlc
Version: 1.0
Date: 2021.3.20
History:
*************************************************/
#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "jconfig.h"

typedef struct delay {
        uint64_t time;
        short timeout;
} delay_t;

typedef struct random_num random_num_t, * random_num_p;

// 定时器回调函数类型
// actual_interval: 实际经过的时间（ms）
// interval: 设定的间隔（ms）
// userdata: 用户自定义数据
typedef void (*timer_callback)(int actual_interval, int interval, void* userdata);

// 定时器对象（不透明指针）
typedef struct timer xtimer_t, * timer_p;


#ifdef __cplusplus
extern "C" {
#endif

        JOY_API char* utils_strdup(const char* str, size_t* out_sz);
        JOY_API bool utils_get_current_time(uint64_t* ms_ticks);
        JOY_API void utils_current_datetime(const char* fmt, char* buf, size_t len);
        JOY_API char* utils_read_file(const char* filename, size_t* sz);
        JOY_API bool utils_check_file(const char* filename);
        JOY_API void utils_append_file(const char* filename, const char* data);
        JOY_API char* utils_read_script(const char* file, size_t* data_size);
        JOY_API short utils_bit2short(uint8_t* buf);
        JOY_API void  utils_short2bit(uint8_t* buf, short value);
        /* c# = BitConverter.FromUInt32 */
        JOY_API int utils_bit2int(uint8_t* buf);
        /* c# = BitConverter.ToUInt32 */
        JOY_API void utils_int2bit(uint8_t* buf, int value);
        JOY_API bool utils_wait_delay(delay_t* delay, uint64_t current_time);
        JOY_API short utils_md5x(const char* input, size_t len, char* output);

        JOY_API unsigned long sdbm_hash(const char* str, int size);
        JOY_API unsigned long fnv1a_hash(const char* str, int size);
        JOY_API unsigned long jenkins_hash(const char* str, int size);
        JOY_API unsigned long murmur3_hash(const char* str);
        JOY_API unsigned long kr_hash(const char* str, int size);

        /**
         * 解码 UTF-8 字符串的第一个字符
         * @param s    输入字符串
         * @param len  字符串长度（字节数）
         * @param code 输出参数，存储解码后的 Unicode 码点 (int32_t)
         * @return     成功时返回消耗的字节数（1~4），失败返回 0
         */
        JOY_API size_t utf8_decode(const char* s, size_t len, int32_t* code);

        /**
         * 迭代遍历 UTF-8 字符串
         * @param s    输入字符串
         * @param len  字符串长度
         * @param pos  当前位置指针（输入/输出）
         * @param code 输出码点 (int32_t)
         * @return     成功返回 1，结束或错误返回 0
         */
        JOY_API int utf8_next(const char* s, size_t len,
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
        JOY_API size_t utf8_decode_all(const char* s, size_t len,
                int32_t* out_codes, size_t out_capacity);

        JOY_API int pack_int8(char* buf, int8_t value, int offset);
        JOY_API int pack_int16(char* buf, int16_t value, int offset);
        JOY_API int pack_int32(char* buf, int32_t value, int offset);
        JOY_API int pack_uint32(char* buf, uint32_t value, int offset);
        JOY_API int pack_int64(char* buf, int64_t value, int offset);
        JOY_API int unpack_int8(const char* buf, int8_t* value, int offset);
        JOY_API int unpack_int16(const char* buf, int16_t* value, int offset);
        JOY_API int unpack_int32(const char* buf, int32_t* value, int offset);
        JOY_API int unpack_uint32(const char* buf, uint32_t* value, int offset);
        JOY_API int unpack_int64(const char* buf, int64_t* value, int offset);
        JOY_API int pack_string(char* buf, const char* value, int type_size, int offset);
        JOY_API int unpack_string(const char* buf, char* value, int type_size, int offset);


        /**
         * 创建定时器
         * @param interval 触发间隔（毫秒）
         * @param cb       回调函数（为 NULL 时不触发）
         * @param userdata 用户数据，透传给回调
         * @return 定时器指针，失败返回 NULL
         */
        JOY_API timer_p timerx_create(int interval, timer_callback cb, void* userdata);

        /**
         * 销毁定时器
         * @param timer 定时器指针
         */
        JOY_API void timerx_destroy(timer_p timer);

        /**
         * 触发检查（通常放在主循环中每帧调用）
         * 内部使用 SDL_GetTicks() 获取当前时间，若达到间隔则调用回调
         * @param timer 定时器指针
         */
        JOY_API void timerx_trigger(timer_p timer);

        /**
         * 设置定时器间隔
         * @param timer    定时器指针
         * @param interval 新间隔（毫秒）
         */
        JOY_API void timerx_set_interval(timer_p timer, int interval);

        /**
         * 设置激活状态
         * @param timer  定时器指针
         * @param active true 激活，false 暂停
         */
        JOY_API void timerx_set_active(timer_p timer, bool active);

        /**
         * 重置定时器（将上次触发时间设为当前时间）
         * @param timer 定时器指针
         */
        JOY_API void timerx_reset(timer_p timer);

        /**
         * 获取自上次触发以来经过的时间（毫秒）
         * @param timer 定时器指针
         * @return 经过的毫秒数
         */
        JOY_API uint64_t timerx_get_elapsed(timer_p timer);

#ifdef __cplusplus
}
#endif


#endif // !UTILS_H
