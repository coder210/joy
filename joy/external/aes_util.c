//
// Created by gj21798 on 2019/10/31.
//

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "aes.h"
#include "aes_util.h"

unsigned char*
aes_encryptx(const unsigned char *data, int in_len, unsigned int* out_len,
    const unsigned char *key, const unsigned char *iv, int key_size) {
    if (in_len <= 0 || in_len >= MAX_LEN) {
        return NULL;
    }

    if (!data) {
        return NULL;
    }

    unsigned int rest_len = in_len % AES_BLOCK_SIZE;
    unsigned int padding_len = AES_BLOCK_SIZE - rest_len;
    unsigned int src_len = in_len + padding_len;

    unsigned char *input = (unsigned char*)calloc(1, src_len);
    memcpy(input, data, in_len);
    if (padding_len > 0) {
        //        memset(input + in_len, (unsigned char) padding_len, padding_len);
        unsigned int i;
        for (i = 0; i < padding_len; i++) {
            *(input + in_len + i) = (unsigned char)padding_len;
        }
    }

    unsigned char *buff = (unsigned char*)calloc(1, src_len);
    if (!buff) {
        free(input);
        return NULL;
    }

    unsigned int key_schedule[AES_BLOCK_SIZE * 4] = { 0 };

    aes_key_setup(key, key_schedule, key_size);
    aes_encrypt_cbc(input, src_len, buff, key_schedule, key_size, iv);
    *out_len = src_len;

    //内存释放
    free(input);

    return buff;
}

unsigned char *aes_decryptx(const unsigned char *data, int in_len, unsigned int* out_len,
    const unsigned char *key, const unsigned char *iv, int key_size) {
    if (in_len <= 0 || in_len >= MAX_LEN) {
        return NULL;
    }
    if (!data) {
        return NULL;
    }

    unsigned int padding_len = 0;
    unsigned int src_len = in_len + padding_len;

    unsigned char *input = (unsigned char*)calloc(1, src_len);
    memcpy(input, data, in_len);
    if (padding_len > 0) {
        //        memset(input + in_len, (unsigned char) padding_len, padding_len);
        unsigned int i;
        for (i = 0; i < padding_len; i++) {
            *(input + in_len + i) = (unsigned char)padding_len;
        }
    }

    unsigned char *buff = (unsigned char*)calloc(1, src_len);
    if (!buff) {
        free(input);
        return NULL;
    }

    unsigned int key_schedule[AES_BLOCK_SIZE * 4] = { 0 };

    aes_key_setup(key, key_schedule, key_size);
    aes_decrypt_cbc(input, src_len, buff, key_schedule, key_size, iv);

    unsigned char *ptr = buff;
    ptr += (src_len - 1);
    padding_len = (unsigned int)*ptr;
    if (padding_len > 0 && padding_len <= AES_BLOCK_SIZE) {
        src_len -= padding_len;
    }

    *out_len = src_len;

    //内存释放
    free(input);

    return buff;
}


// 128
unsigned char *aes_encrypt128x(const unsigned char *in, int in_len, unsigned int* out_len,
    const unsigned char *key, const unsigned char *iv) {
    assert(strlen(key) == 16);
    unsigned char *buff = aes_encryptx(in, in_len, out_len, key, iv, 128);
    return buff;
}

unsigned char *aes_decrypt128x(const unsigned char *data, int in_len, unsigned int* out_len,
    const unsigned char *key, const unsigned char *iv) {
    assert(strlen(key) == 16);
    unsigned char *buff = aes_decryptx(data, in_len, out_len, key, iv, 128);
    return buff;
}

unsigned char *
aes_encrypt256x(const unsigned char *in, int in_len, unsigned int* out_len,
    const unsigned char *key, const unsigned char *iv) {
    assert(strlen(key) == 32);
    unsigned char *buff = aes_encryptx(in, in_len, out_len, key, iv, 256);
    return buff;
}

unsigned char *
aes_decrypt256x(const unsigned char *data, int in_len, unsigned int* out_len,
    const unsigned char *key, const unsigned char *iv) {
    assert(strlen(key) == 32);
    unsigned char *buff = aes_decryptx(data, in_len, out_len, key, iv, 256);
    return buff;
}