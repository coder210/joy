/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-env.h
Description: 环境变量
Author: ydlc
Version: 1.0
Date: 2021.3.28
History:
*************************************************/
#ifndef LIV_ENV_H
#define LIV_ENV_H


typedef struct env env_t, * env_p;

env_p liv_env_create(void);
const char* liv_env_get(env_p env, const char* key);
void liv_env_set(env_p env, const char* key, const char* value);
void liv_env_destroy(env_p env);


#endif // !LIV_ENV_H