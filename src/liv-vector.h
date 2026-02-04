/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-vector.h
Description: 动态数组结构
Author: ydlc
Version: 1.0
Date: 2025.10.28
History:
*************************************************/
#ifndef LIV_VECTOR_H
#define LIV_VECTOR_H
#include <stdlib.h>

#define VECTOR_INIT(name, type) \
typedef struct liv_vector_##name { \
        type *a; \
        int m; \
        int n; \
}liv_vector_##name##_t, *liv_vector_##name##_p; \
\
static void liv_vector_init_##name(liv_vector_##name##_p vec) \
{\
        vec->n = 0;\
        vec->m = 32;\
        vec->a = malloc(sizeof(type) * vec->m);\
}\
\
static void liv_vector_push_##name(liv_vector_##name##_p vec, type id) \
{\
        if (vec->n >= vec->m) {\
                vec->a = realloc(vec->a, vec->m * 2);\
        }\
        vec->a[vec->n++] = id;\
}\
\
static type liv_vector_a_##name(liv_vector_##name##_p vec, int i)\
{\
        if (vec->n > i) {\
                return vec->a[i];\
        }\
        else {\
                return 0;\
        }\
}\
\
static int liv_vector_size_##name(liv_vector_##name##_p vec) \
{\
        return vec->n;\
}\
\
static void liv_vector_clear_##name(liv_vector_##name##_p vec) \
{\
        vec->n = 0;\
}\
\
static void liv_vector_destroy_##name(liv_vector_##name##_p vec) \
{\
        vec->n = vec->m = 0;\
        free(vec->a);\
}\

#define liv_vector_t(name) liv_vector_##name##_t
#define liv_vector_p(name) liv_vector_##name##_p
#define liv_vector_init(name, vec) liv_vector_init_##name(vec)
#define liv_vector_push(name, vec, id) liv_vector_push_##name(vec, id)
#define liv_vector_a(name, vec, i) liv_vector_a_##name(vec, i)
#define liv_vector_size(name, vec) liv_vector_size_##name(vec)
#define liv_vector_clear(name, vec) liv_vector_clear_##name(vec)
#define liv_vector_destroy(name, vec) liv_vector_destroy_##name(vec)


#endif // !LIV_VECTOR_H

