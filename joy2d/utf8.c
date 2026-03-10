#include "utf8.h"
#include <stddef.h>

#define MAXUNICODE 0x10FFFFu
#define iscont(c) (((c) & 0xC0) == 0x80)

/* 各长度字符的最小合法码点（防止过短表示） */
static const uint32_t limits[] = {
    ~(uint32_t)0, 0x80, 0x800, 0x10000u, 0x200000u, 0x4000000u
};

size_t utf8_decode(const char* s, size_t len, int32_t* code)
{
        if (len == 0) return 0;

        unsigned char c = (unsigned char)s[0];

        /* ASCII */
        if (c < 0x80) {
                *code = c;
                return 1;
        }

        int nbytes;
        uint32_t min;
        uint32_t res;

        /* 根据首字节判断字节数 */
        if (c < 0xC2) {
                return 0;
        }
        else if (c < 0xE0) {        /* 2字节: 110xxxxx */
                nbytes = 2;
                min = 0x80;
                res = c & 0x1F;
        }
        else if (c < 0xF0) {        /* 3字节: 1110xxxx */
                nbytes = 3;
                min = 0x800;
                res = c & 0x0F;
        }
        else if (c < 0xF5) {        /* 4字节: 11110xxx (F4 是能表示 0x10FFFF 的最大值) */
                nbytes = 4;
                min = 0x10000;
                res = c & 0x07;
        }
        else {
                return 0;                  /* 0xF5~0xFF 非法 */
        }

        if (len < (size_t)nbytes)
                return 0;

        /* 处理后续字节 */
        for (int i = 1; i < nbytes; i++) {
                unsigned char cc = (unsigned char)s[i];
                if (!iscont(cc))
                        return 0;
                res = (res << 6) | (cc & 0x3F);
        }

        /* 过短检查 */
        if (res < min)
                return 0;

        /* 最大值及代理项检查（严格模式） */
        if (res > MAXUNICODE)
                return 0;
        if (0xD800 <= res && res <= 0xDFFF)
                return 0;

        *code = (int32_t)res;
        return nbytes;
}

int utf8_next(const char* s, size_t len, size_t* pos, int32_t* code)
{
        if (*pos >= len) return 0;
        size_t consumed = utf8_decode(s + *pos, len - *pos, code);
        if (consumed == 0) return 0;
        *pos += consumed;
        return 1;
}

size_t utf8_decode_all(const char* s, size_t len, int32_t* out_codes, size_t out_capacity)
{
        size_t count = 0;
        size_t pos = 0;

        /* 第一次遍历：计算所需码点个数，同时验证字符串有效性 */
        while (pos < len) {
                int32_t cp;  /* 临时存储，无需实际值 */
                size_t consumed = utf8_decode(s + pos, len - pos, &cp);
                if (consumed == 0) {
                        return 0;  /* 无效 UTF-8 序列 */
                }
                count++;
                pos += consumed;
        }

        /* 如果 out_codes 为 NULL，则只返回所需长度（用于缓冲区预分配） */
        if (out_codes == NULL) {
                return count;
        }

        /* 检查缓冲区容量是否足够 */
        if (out_capacity < count) {
                return 0;  /* 缓冲区太小，失败 */
        }

        /* 第二次遍历：填充数组 */
        pos = 0;
        size_t idx = 0;
        while (pos < len) {
                int32_t cp;
                size_t consumed = utf8_decode(s + pos, len - pos, &cp);
                /* 此时不会失败，因为第一次已验证过 */
                out_codes[idx++] = cp;
                pos += consumed;
        }

        return count;
}