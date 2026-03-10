#ifndef UTF8_H
#define UTF8_H

#include <stddef.h>
#include <stdint.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

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
	JOY_API int utf8_next(const char* s, size_t len, size_t* pos, int32_t* code);

	/**
	 * 将整个 UTF-8 字符串解码为码点数组（调用者提供缓冲区）
	 * @param s            输入字符串
	 * @param len          字符串长度（字节数）
	 * @param out_codes    输出缓冲区（接收码点数组），可为 NULL 以查询所需长度
	 * @param out_capacity 输出缓冲区的容量（最多能存放的码点个数）
	 * @return             成功时返回实际写入的码点个数（若 out_codes==NULL，则返回所需个数）；
	 *                     若字符串无效或缓冲区容量不足，返回 0
	 */
	JOY_API size_t utf8_decode_all(const char* s, size_t len, int32_t* out_codes, size_t out_capacity);

#ifdef __cplusplus
}
#endif

#endif // UTF8_H