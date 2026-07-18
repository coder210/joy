#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H
#include <joy/jmath.h>

// 摄像机单例（挂载在 ctx->singleton_camera 实体上）
struct CameraComponent {
    vec2_t position = { 0.0f, 0.0f };  // 摄像机世界坐标(米)
};

#endif
