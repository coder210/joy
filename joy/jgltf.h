/******************************************************************************
 * jgltf.h —— joy 的 glTF/glb 加载 + CPU 骨骼蒙皮动画 + 绘制模块（说明性头）
 *
 * 模块职责：
 *   1) 加载：读取 .glb/.gltf 文件。兼容 THREE.GLTFExporter 导出的带 UTF-8 BOM
 *      与头部杂散字节的"畸形 glb"——加载时先按 norm_test 的算法在内存中归一化
 *      重建出一份 clean glb，再用 cgltf 解析并加载 buffer。
 *   2) 提取：把每个 mesh primitive 的 POSITION/NORMAL/TEXCOORD_0（+ 索引）读成
 *      归一化的 lit 顶点布局（pos.xyz + normal.xyz + uv.xy，每顶点 8 个 float）；
 *      若挂 skin 还读 JOINTS_0/WEIGHTS_0 与 inverse_bind_matrices、joints→节点映射。
 *   3) 蒙皮动画：实例每帧对每个被采样的节点求局部 TRS（平移/缩放线性、旋转 slerp），
 *      自上而下合成节点 world 矩阵，对每个蒙皮顶点按
 *          final = Σ_joint w_j · (inverseBind[j] · world[j]) · pos
 *      做 CPU 蒙皮（法线用同样的 3x3 移位矩阵近似），再 r3d_mesh_skinned_update 覆盖写
 *      GPU 缓冲。
 *
 * 两种结构（与 jspine 保持一致）：
 *   jgltf_asset_t    共享资源，引用计数，多实例复用；
 *   jgltf_instance_t 每实体一份，持有自己的 GPU 蒙皮缓冲与播放状态，可各自播不同动画。
 *
 * C API 声明位于 joy/joy.h（#ifdef JOY_USE_GLTF 块内，JOY_API 导出），本头仅作
 * 模块说明与聚合 include；C++ RAII 包装见 joy/jgltf.hpp。
 *
 * 编译开关：joy_add_sources(<target> GLTF) 会定义 JOY_USE_GLTF 并把 jgltf.c 与其
 * 引用的 external/cgltf/cgltf.h（CGLTF_IMPLEMENTATION 在 jgltf.c 内 define 一次）编入 target。
 *****************************************************************************/

#ifndef JOY_JGLTF_H
#define JOY_JGLTF_H

/* glTF 模块为可选：定义 JOY_USE_GLTF 时 joy.h 会提供本模块全部 C API */
#include <joy/joy.h>

#endif /* JOY_JGLTF_H */