/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: vector.h
Description: 动态数组结构
Author: ydlc
Version: 1.0
Date: 2025.10.28
History:
*************************************************/
#ifndef VECTOR_H
#define VECTOR_H
#include <stdlib.h>

#define VECTOR_INIT(name, type) \
typedef struct vector_##name { \
        type *a; \
        int m; \
        int n; \
}vector_##name##_t, *vector_##name##_p; \
\
static void vector_init_##name(vector_##name##_p vec) \
{\
        vec->n = 0;\
        vec->m = 32;\
        vec->a = (type *)malloc(sizeof(type) * vec->m);\
}\
\
static void vector_push_##name(vector_##name##_p vec, type id) \
{\
        if (vec->n >= vec->m) {\
                vec->a = (type *)realloc(vec->a, vec->m * 2);\
        }\
        vec->a[vec->n++] = id;\
}\
\
static type vector_a_##name(vector_##name##_p vec, int i)\
{\
        if (vec->n > i) {\
                return vec->a[i];\
        }\
        else {\
                return 0;\
        }\
}\
\
static int vector_size_##name(vector_##name##_p vec) \
{\
        return vec->n;\
}\
\
static void vector_clear_##name(vector_##name##_p vec) \
{\
        vec->n = 0;\
}\
\
static void vector_destroy_##name(vector_##name##_p vec) \
{\
        vec->n = vec->m = 0;\
        free(vec->a);\
}\

#define vector_t(name) vector_##name##_t
#define vector_p(name) vector_##name##_p
#define vector_init(name, vec) vector_init_##name(vec)
#define vector_push(name, vec, id) vector_push_##name(vec, id)
#define vector_a(name, vec, i) vector_a_##name(vec, i)
#define vector_size(name, vec) vector_size_##name(vec)
#define vector_clear(name, vec) vector_clear_##name(vec)
#define vector_destroy(name, vec) vector_destroy_##name(vec)


#endif // !CORE_VECTOR_H

