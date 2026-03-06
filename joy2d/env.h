/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: env.h
Description: 环境变量
Author: ydlc
Version: 1.0
Date: 2021.3.28
History:
*************************************************/
#ifndef ENV_H
#define ENV_H
#include "config.h"

typedef struct env env_t, * env_p;

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API env_p env_create(void);
	JOY_API const char* env_get(env_p env, const char* key);
	JOY_API void env_set(env_p env, const char* key, const char* value);
	JOY_API void env_destroy(env_p env);

#ifdef __cplusplus
}
#endif


#endif // !ENV_H