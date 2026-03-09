#ifndef UTF8_H
#define UTF8_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

        /**
         * 解码 UTF-8 字符串的第一个字符
         * @param s    输入字符串（不以 '\0' 结尾也可以，由 len 指定长度）
         * @param len  字符串长度（字节数）
         * @param code 输出参数，存储解码后的 Unicode 码点
         * @return     成功时返回消耗的字节数（1~4），失败（无效序列或长度不足）返回 0
         */
        size_t utf8_decode(const char* s, size_t len, uint32_t* code);

        /**
         * 迭代遍历 UTF-8 字符串
         * 用法：
         *   size_t pos = 0;
         *   uint32_t cp;
         *   while (utf8_next(str, str_len, &pos, &cp)) {
         *       // 处理码点 cp
         *   }
         * @param s    输入字符串
         * @param len  字符串长度
         * @param pos  输入时为当前读取位置（从 0 开始），输出时为下一个字符的起始位置
         * @param code 输出码点
         * @return     成功解码一个字符返回 1，已到达末尾或遇到无效序列返回 0
         */
        int utf8_next(const char* s, size_t len, size_t* pos, uint32_t* code);

        /**
         * 将整个 UTF-8 字符串解码为码点数组（动态分配）
         * @param s        输入字符串
         * @param len      字符串长度
         * @param out_len  输出数组长度（码点个数）
         * @return         成功时返回新分配的 uint32_t 数组，调用者需 free；
         *                 若字符串无效或内存不足返回 NULL，out_len 被置为 0
         */
        uint32_t* utf8_decode_all(const char* s, size_t len, size_t* out_len);

#ifdef __cplusplus
}
#endif

#endif // UTF8_H