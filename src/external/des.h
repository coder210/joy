#ifndef DES_H
#define DES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define SMALL_CHUNK 256

void des_main_ks(uint32_t SK[32], const uint8_t key[8]);
void des_crypt(const uint32_t SK[32], const uint8_t input[8], uint8_t output[8]);

#endif // !DES_H


