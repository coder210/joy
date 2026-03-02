/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: env.h
Description: 环境变量
Author: ydlc
Version: 1.0
Date: 2021.3.28
History:
*************************************************/
#ifndef CORE_ENV_H
#define CORE_ENV_H


typedef struct env env_t, * env_p;

env_p env_create(void);
const char* env_get(env_p env, const char* key);
void env_set(env_p env, const char* key, const char* value);
void env_destroy(env_p env);


#endif // !CORE_ENV_H