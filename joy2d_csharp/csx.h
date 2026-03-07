/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: csx.h
Description: c#
Author: ydlc
Version: 1.0
Date: 2021.3.28
History:
*************************************************/
#ifndef CSX_H
#define CSX_H
#include <stdbool.h>
#include "joy2d/config.h"

typedef struct csx csx_t, *csx_p;
#ifdef __cplusplus
extern "C" {
#endif
        // 创建 Mono 运行时域
        JOY_API csx_p csx_create(void);

        // 加载程序集文件（.dll），保存到内部列表
        JOY_API bool csx_load_assembly(csx_p csx, const char* filename);

        // 在所有已加载的程序集中查找类
        JOY_API MonoClass* csx_find_class(csx_p csx, const char* namespace_name, const char* class_name);

        // 调用静态方法（无参数示例，可根据需要扩展）
        JOY_API MonoObject* csx_invoke_static_method(csx_p csx, const char* namespace_name, const char* class_name, const char* method_name, void** args);

        // 注册内部调用（C 函数映射到 C# 方法）
        JOY_API void csx_register_internal_call(const char* name, const void* method);

        // 释放资源
        JOY_API void csx_release(csx_p csx);
#ifdef __cplusplus
}
#endif
#endif // !CSX_H
