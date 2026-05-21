//
// Created by gj21798 on 2019/10/31.
//

#ifndef AESWITHPUREC_AES_UTIL_H
#define AESWITHPUREC_AES_UTIL_H

#define MAX_LEN (2*1024*1024)
//#define AES_KEY_SIZE 256

// "keysize" is the length in bits of "key", must be 128, 192, or 256.
// 128, length 16
// 256, length 32

unsigned char *aes_encryptx(const unsigned char *in, int in_len, unsigned int* out_len,
    const unsigned char *key, const unsigned char *iv, int key_size);

unsigned char *aes_decryptx(const unsigned char *in, int in_len, unsigned int* out_len,
    const unsigned char *key, const unsigned char *iv, int key_size);


// 128
unsigned char *aes_encrypt128x(const unsigned char *in, int in_len, unsigned int* out_len,
    const unsigned char *key, const unsigned char *iv);

unsigned char *aes_decrypt128x(const unsigned char *data, int in_len, unsigned int* out_len,
    const unsigned char *key, const unsigned char *iv);


// 256
unsigned char *aes_encrypt256x(const unsigned char *in, int in_len, unsigned int* out_len,
    const unsigned char *key, const unsigned char *iv);

unsigned char *aes_decrypt256x(const unsigned char *data, int in_len, unsigned int* out_len,
    const unsigned char *key, const unsigned char *iv);


#endif //AESWITHPUREC_AES_UTIL_H