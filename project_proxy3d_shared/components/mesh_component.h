#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H
#include <joy/joy.h>

// 3D 渲染网格组件：实体自带的网格引用 + 颜色。
// 每个实体独立持有，创建时从共享资源池（ctx->mesh_player/mesh_enemy 等）取 mesh，
// 100 个玩家只需各自挂一个 MeshComponent，无需为每个玩家定义单独的 mesh 变量。
struct MeshComponent {
    render_mesh_p mesh = nullptr;
    jcolor_t color = { 255, 255, 255, 255 };
};

#endif
