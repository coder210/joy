#include "utf8.h"
#include <stdlib.h>
#include <string.h>

/* Unicode 标量值的范围 */
#define MAXUNICODE 0x10FFFFu

/* 检查是否为后续字节 (10xxxxxx) */
#define iscont(c) (((c) & 0xC0) == 0x80)

/* 用于检查过短表示的最小值表（索引对应字节数） */
static const uint32_t limits[] = {
    ~(uint32_t)0,  /* 占位，不会用到 */
    0x80,          /* 2字节字符的最小值 */
    0x800,         /* 3字节字符的最小值 */
    0x10000u,      /* 4字节字符的最小值 */
    0x200000u,     /* 5字节（Lua 允许到 0x7FFFFFFF，但严格模式只用前4项） */
    0x4000000u
};

size_t utf8_decode(const char* s, size_t len, uint32_t* code) {
        if (len == 0) return 0;

        unsigned char c = (unsigned char)s[0];

        /* ASCII 字符 */
        if (c < 0x80) {
                *code = c;
                return 1;
        }

        int nbytes;      /* 本字符应有的字节数 */
        uint32_t min;    /* 最小合法码点（防止过短表示） */
        uint32_t res;    /* 累积的码点 */

        /* 根据首字节判断字节数 */
        if (c < 0xC2) {          /* 0xC0,0xC1 会构成过短的2字节表示 */
                return 0;
        }
        else if (c < 0xE0) {   /* 2字节：首字节 110xxxxx */
                nbytes = 2;
                min = 0x80;
                res = c & 0x1F;
        }
        else if (c < 0xF0) {   /* 3字节：1110xxxx */
                nbytes = 3;
                min = 0x800;
                res = c & 0x0F;
        }
        else if (c < 0xF5) {   /* 4字节：11110xxx，F4 是能编码 0x10FFFF 的最大值 */
                nbytes = 4;
                min = 0x10000;
                res = c & 0x07;
        }
        else {                 /* 0xF5~0xFF 无效 */
                return 0;
        }

        /* 检查剩余长度是否足够 */
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

        /* 最大值和代理项检查（严格模式） */
        if (res > MAXUNICODE)
                return 0;
        if (0xD800 <= res && res <= 0xDFFF)   /* 代理项 */
                return 0;

        *code = res;
        return nbytes;
}

int utf8_next(const char* s, size_t len, size_t* pos, uint32_t* code) {
        if (*pos >= len)
                return 0;

        size_t consumed = utf8_decode(s + *pos, len - *pos, code);
        if (consumed == 0) {
                /* 遇到无效序列，这里直接返回失败，不自动跳过 */
                return 0;
        }

        *pos += consumed;
        return 1;
}

uint32_t* utf8_decode_all(const char* s, size_t len, size_t* out_len) {
        uint32_t* codes = NULL;
        size_t capacity = 0;
        size_t count = 0;
        size_t pos = 0;

        while (pos < len) {
                uint32_t cp;
                size_t consumed = utf8_decode(s + pos, len - pos, &cp);
                if (consumed == 0) {
                        free(codes);
                        *out_len = 0;
                        return NULL;
                }

                if (count == capacity) {
                        size_t new_cap = capacity == 0 ? 8 : capacity * 2;
                        uint32_t* new_codes = realloc(codes, new_cap * sizeof(uint32_t));
                        if (!new_codes) {
                                free(codes);
                                *out_len = 0;
                                return NULL;
                        }
                        codes = new_codes;
                        capacity = new_cap;
                }

                codes[count++] = cp;
                pos += consumed;
        }

        *out_len = count;
        return codes;
}