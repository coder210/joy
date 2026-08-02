#ifndef STATIC_MESH_COMPONENT_H
#define STATIC_MESH_COMPONENT_H
#include <joy/joy.h>

// 静态碰撞体/装饰物的渲染信息（服务器本地渲染，不发给客户端）
struct StaticMeshComponent {
    render_mesh_p mesh = nullptr;      // 渲染用 mesh（cube/sphere/ramp）
    jcolor_t color = { 255, 255, 255, 255 };
    float roll = 0.0f;                 // 绕 Z 轴旋转角（弧度），斜放长方体斜坡用；绕 Y 用 TransformComponent::yaw
};

#endif
