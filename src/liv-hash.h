#ifndef LIV_HASH
#define LIV_HASH
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned long sdbm_hash(const char* str, int size)
{
        unsigned long hash = 0;
        int c;
        while ((c = *str++)) {
                hash = c + (hash << 6) + (hash << 16) - hash;
        }
        return hash % size;
}

static unsigned long fnv1a_hash(const char* str, int size)
{
        unsigned long hash = 2166136261u;  // FNV_offset_basis
        int c;
        while ((c = *str++)) {
                hash ^= c;
                hash *= 16777619u;  // FNV_prime
        }
        return hash % size;
}


static unsigned long jenkins_hash(const char* str, int size)
{
        unsigned long hash = 0;
        int c;
        while ((c = *str++)) {
                hash += c;
                hash += (hash << 10);
                hash ^= (hash >> 6);
        }
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        return hash % size;
}

static unsigned long murmur3_hash(const char* str)
{
        unsigned long hash = 0;
        const unsigned int seed = 0x9747b28c;
        const unsigned int c1 = 0xcc9e2d51;
        const unsigned int c2 = 0x1b873593;

        const unsigned char* data = (const unsigned char*)str;
        int len = strlen(str);

        while (len >= 4) {
                unsigned int k = *(unsigned int*)data;

                k *= c1;
                k = (k << 15) | (k >> 17);
                k *= c2;

                hash ^= k;
                hash = (hash << 13) | (hash >> 19);
                hash = hash * 5 + 0xe6546b64;

                data += 4;
                len -= 4;
        }

        unsigned int k = 0;
        switch (len) {
        case 3: k ^= data[2] << 16;
        case 2: k ^= data[1] << 8;
        case 1: k ^= data[0];
                k *= c1;
                k = (k << 15) | (k >> 17);
                k *= c2;
                hash ^= k;
        }

        hash ^= strlen(str);
        hash ^= hash >> 16;
        hash *= 0x85ebca6b;
        hash ^= hash >> 13;
        hash *= 0xc2b2ae35;
        hash ^= hash >> 16;

        return hash;
}


static unsigned long kr_hash(const char* str, int size)
{
        unsigned long hash = 0;
        int c;
        while ((c = *str++)) {
                hash = c + hash * 31;
        }
        return hash % size;
}

#endif // !LIV_HASH

