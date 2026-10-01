/*
 * glTF/glb 加载 + CPU 骨骼蒙皮动画渲染测试（joy 的 jgltf 模块 + ECS 用法演示）
 *
 * - 渲染/资源：joy 模块 jgltf（joy/jgltf.c），用 cgltf 解析 glb，
 *   顶点/动画/骨骼在 CPU 上求值后 r3d_mesh_skinned 上传 GPU 绘制。
 * - ECS：jgltf_instance_t 由 RAII 包装 joy/jgltf.hpp 的 jgltf::instance 托管，
 *   构造即 create、析构即 destroy（entt 销毁实体/扩容移动自动处理）。
 *
 * 用法：
 *   gltf_sdl3_test [--auto] [robot.glb] [air.glb] [实体数量]
 *
 * 操作：
 *   空格       所有实体切换到下一个动画
 *   B          显示/隐藏骨骼线
 *   P          暂停/继续
 *   W / S      加速 / 减速
 *   A / D      缩小 / 放大
 *   F          打印 CPU 蒙皮移动顶点计数（验证）
 *   Escape     退出
 */
#include <SDL3/SDL.h>
#include <joy/joy.h>
#include <joy/jgltf.hpp>   /* RAII 包装：组件生命周期交给 ECS 托管 */
#include <entt.hpp>

#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 640

/* 用户侧组件：与 jgltf::instance 组合使用，演示多组件协同 */
struct AnimState {
        int index = 0;          /* 当前动画在资源列表中的下标 */
        int anim_count = 0;     /* 其资源可用的动画数 */
        float angle = 0.0f;     /* 绕 Y 轴自转角度，用于展示 3D 姿态 */
        float scale = 1.0f;     /* 按模型包围球半径归一化的缩放 */
        vec3_t center = { 0.0f, 0.0f, 0.0f }; /* 模型包围球中心（平移对齐原点） */
};

/* 绕 Y 轴旋转的列主序矩阵（与 mat44_t/mat44_mul 约定一致） */
static mat44_t rot_y(float a) {
        float c = cosf(a), s = sinf(a);
        mat44_t m = mat44_identity();
        m.m0 = c; m.m2 = -s;
        m.m8 = s; m.m10 = c;
        return m;
}

/* 组装模型矩阵：model = T * Ry * S * T(-center)，把实例摆到世界指定位置。
   中心偏移把模型原点平移到其包围球中心，避免大模型(如 Fox 约 26×79×155 单位)
   因坐标原点偏离视锥中心而不可见。 */
static mat44_t make_model(vec3_t pos, float yaw, float scale, vec3_t center) {
        mat44_t T = mat44_translate(pos.x, pos.y, pos.z);
        mat44_t R = rot_y(yaw);
        mat44_t S = mat44_scale(scale, scale, scale);
        mat44_t C = mat44_translate(-center.x * scale, -center.y * scale, -center.z * scale);
        return mat44_mul(T, mat44_mul(R, mat44_mul(S, C)));
}

/* 输出一张 16x16 纯白 RGBA 位图到文件（供阅读/调试，非必须可跳过） */
static void dump_scene_info(int prim_count, int node_count, int anim_count, int has_skin) {
        printf("---- joy jgltf self check ----\n");
        printf("prims=%d nodes=%d anims=%d has_skin=%d\n",
               prim_count, node_count, anim_count, has_skin);
        printf("--------------------------------\n");
}

int main(int argc, char **argv) {
        const char *robot_path = "Fox.glb";  /* 主动画模型：Khronos Fox（含骨骼蒙皮动画 + 贴图） */
        const char *air_path   = "air.glb";
        int auto_mode = 0;
        int entity_count = 4;
        int positional = 0;
        int i;

        jwindow_p window = NULL;
        jr3d_p r3d = NULL;
        jgltf::asset robot_asset;   /* RAII：析构自动 jgltf_asset_release */
        jgltf::asset air_asset;
        entt::registry registry;
        int quit = 0;
        int draw_bones = 0;
        int paused = 0;
        float time_scale = 1.0f;
        float scale_mul = 1.0f;
        float cam_yaw = 0.0f, cam_pitch = 55.0f, cam_dist = 7.0f;
        Uint64 last_ticks, fps_ticks, fps_frames = 0;
        Uint64 frame_count = 0;
        uint64_t moved_total = 0;

        setvbuf(stdout, NULL, _IONBF, 0);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "--auto") == 0) { auto_mode = 1; continue; }
                if (positional == 0) { robot_path = argv[i]; positional++; continue; }
                if (positional == 1) { air_path = argv[i]; positional++; continue; }
                if (positional == 2) { entity_count = atoi(argv[i]); positional++; continue; }
        }
        if (entity_count < 1) entity_count = 1;
        if (entity_count > 64) entity_count = 64;

        if (!SDL_Init(SDL_INIT_VIDEO)) {
                printf("SDL_Init failed: %s\n", SDL_GetError());
                return 1;
        }
        window = create_window("joy jgltf + ECS (entt)", WINDOW_WIDTH, WINDOW_HEIGHT);
        if (!window) { printf("create_window failed; SDL3 GPU unavailable\n"); SDL_Quit(); return 1; }

        /* r3d 需要着色器：工作目录下应有 shaders/lighting.vert.spv 等（资产已复制） */
        r3d = r3d_create(window, "shaders");
        if (!r3d) {
                printf("r3d_create failed (GPU unavailable or shaders missing in 'shaders')\n");
                destroy_window(window);
                SDL_Quit();
                return 1;
        }

        /* ---- 共享资源（RAII：离开作用域/重置时自动释放） ---- */
        robot_asset = jgltf::asset(r3d, robot_path);
        air_asset   = jgltf::asset(r3d, air_path);
        if (!robot_asset.valid() && !air_asset.valid()) {
                printf("[jgltf] no valid asset loaded\n");
                r3d_destroy(r3d);
                destroy_window(window);
                SDL_Quit();
                return 1;
        }
        {
                jgltf::asset *primary = robot_asset.valid() ? &robot_asset : &air_asset;
                dump_scene_info(primary->prim_count(), primary->node_count(),
                                primary->animation_count(), primary->has_skin());
                /* 用主模型的尺寸预估相机初始距离（每实例的精确缩放/中心见 ECS 装配） */
                float pd_radius = 1.0f; vec3_t pd_center;
                primary->bounds(&pd_center, &pd_radius);
                cam_dist = 5.0f * 2.5f + 2.0f;  /* 归一化后约 2.5 单位 */
                printf("[fit] primary radius=%.3f center=(%.2f,%.2f,%.2f) cam_dist=%.2f\n",
                       pd_radius, pd_center.x, pd_center.y, pd_center.z, cam_dist);
        }

        /* ---- ECS：emplace 即 create，destroy 即 destroy（无需手工管理） ---- */
        for (i = 0; i < entity_count; i++) {
                jgltf::asset *use = (i % 2 == 0) ? &robot_asset : &air_asset;
                if (!use->valid()) use = robot_asset.valid() ? &robot_asset : &air_asset;

                auto e = registry.create();
                auto &inst = registry.emplace<jgltf::instance>(e, *use); /* 构造时 create */
                auto &anim = registry.emplace<AnimState>(e);

                if (!inst.valid()) {
                        printf("[ecs] bind failed\n");
                        registry.destroy(e);   /* 析构自动 destroy */
                        continue;
                }
                if (use->animation_count() > 0) {
                        inst.set_animation(0);
                        anim.anim_count = use->animation_count();
                } else {
                        inst.set_paused(true); /* 无动画静态显示 */
                }
                inst.set_speed(0.9f + 0.1f * (i % 4));
                /* 摆放成一字排开，展示多个实例各自独立播放 */
                anim.angle = 0.0f;
                /* 每个实例用自己资源的尺寸归一化 + 中心对齐，兼容不同规格模型 */
                float rad = 1.0f;
                use->bounds(&anim.center, &rad);
                anim.scale = 2.5f / rad;
                if (use->animation_count() > 0) {
                        cam_dist = std::max(cam_dist, 5.0f * (rad * anim.scale) + 2.0f);
                }
        }
        printf("[ecs] entities: %d (RAII 组件，销毁实体自动释放)\n",
               (int) registry.view<jgltf::instance>().size());
        printf("[help] space=next anim  B=bones  P=pause  W/S=speed  A/D=scale  F=moved-vert check\n");

        last_ticks = SDL_GetTicksNS();
        fps_ticks = last_ticks;

        while (!quit) {
                SDL_Event e;
                Uint64 now;
                float dt;

                while (SDL_PollEvent(&e)) {
                        if (e.type == SDL_EVENT_QUIT) quit = 1;
                        else if (e.type == SDL_EVENT_KEY_DOWN) {
                                SDL_Keycode key = e.key.key;
                                if (key == SDLK_ESCAPE) quit = 1;
                                else if (key == SDLK_SPACE) {
                                        /* system：同时访问两个组件，所有实体切到下一个动画 */
                                        registry.view<jgltf::instance, AnimState>().each(
                                                [&](jgltf::instance &inst, AnimState &anim) {
                                                        if (anim.anim_count <= 0) return;
                                                        anim.index = (anim.index + 1) % anim.anim_count;
                                                        inst.set_animation(anim.index);
                                                });
                                        printf("[ecs] all entities -> next animation\n");
                                } else if (key == SDLK_B) draw_bones = !draw_bones;
                                else if (key == SDLK_P) paused = !paused;
                                else if (key == SDLK_A) scale_mul = std::max(0.2f, scale_mul - 0.1f);
                                else if (key == SDLK_D) scale_mul = std::min(3.0f, scale_mul + 0.1f);
                                else if (key == SDLK_W) time_scale = std::min(4.0f, time_scale + 0.25f);
                                else if (key == SDLK_S) time_scale = std::max(0.0f, time_scale - 0.25f);
                                else if (key == SDLK_F) {
                                        uint64_t mv = 0;
                                        registry.view<jgltf::instance>().each(
                                                [&](jgltf::instance &inst) { mv += inst.moved_vertices(); });
                                        printf("[check] moved_vertices(this frame)=%llu\n",
                                               (unsigned long long) mv);
                                }
                        }
                }

                now = SDL_GetTicksNS();
                dt = (float) ((double) (now - last_ticks) / 1000000000.0);
                last_ticks = now;
                if (dt > 0.1f) dt = 0.1f;

                /* ---- system 1：更新动画状态（先推进实例，再取移动计数） ---- */
                {
                        uint64_t mv = 0;
                        registry.view<jgltf::instance>().each([&](jgltf::instance &inst) {
                                if (inst.valid() && !paused) inst.update(dt * time_scale);
                                mv += inst.moved_vertices();
                        });
                        moved_total += mv;
                }

                /* ---- 渲染 ---- */
                if (!r3d_begin(r3d)) { /* GPU 暂不可用则跳过本帧 */ }
                else {
                        r3d_set_clear_color(r3d, 40, 44, 52);
                        float pitch = cam_pitch * 3.14159265f / 180.0f;
                        float cp = cosf(pitch), sp = sinf(pitch);
                        vec3_t target = { 0.0f, 0.8f, 0.0f };
                        vec3_t eye = {
                                target.x - sinf(cam_yaw) * cp * cam_dist,
                                target.y + sp * cam_dist,
                                target.z - cosf(cam_yaw) * cp * cam_dist
                        };
                        vec3_t up = { 0.0f, 1.0f, 0.0f };
                        r3d_camera_lookat(r3d, eye, target, up, 55.0f);

                        /* ---- system 2：按世界位置绘制 ---- */
                        int idx = 0;
                        registry.view<jgltf::instance, AnimState>().each(
                                [&](jgltf::instance &inst, AnimState &anim) {
                                        if (!inst.valid()) return;
                                        idx++;
                                        float stagger = 2.2f;
                                        vec3_t pos = { (idx - (entity_count + 1) * 0.5f) * stagger,
                                                       0.0f, 0.0f };
                                        mat44_t m = make_model(pos, anim.angle, scale_mul * anim.scale, anim.center);
                                        inst.draw(r3d, &m);
                                        if (draw_bones) inst.draw_bones(r3d, &m);
                                });
                        r3d_end(r3d);
                }

                frame_count++;
                if (frame_count == 5) {
                        registry.view<jgltf::instance, AnimState>().each(
                                [&](jgltf::instance &inst, AnimState &anim) {
                                        printf("[dbg] anim_idx=%d moved=%d\n",
                                               inst.animation(), inst.moved_vertices());
                                });
                }
                if (auto_mode && frame_count == 60) {
                        /* 自动验证：销毁一半实体，组件析构应自动释放且不崩溃 */
                        int before = (int) registry.view<jgltf::instance>().size();
                        for (int k = 0; k < before / 2; k++) {
                                auto v = registry.view<jgltf::instance>();
                                if (v.empty()) break;
                                registry.destroy(v.back());
                        }
                        printf("[auto] destroyed %d entities: %d -> %d\n", before / 2, before,
                               (int) registry.view<jgltf::instance>().size());
                }
                if (auto_mode && frame_count >= 180) {
                        printf("[auto] rendered %llu frames; cum. moved_vertices=%llu drawcalls=%d\n",
                               (unsigned long long) frame_count,
                               (unsigned long long) moved_total, r3d_get_draw_calls(r3d));
                        /* CPU 蒙皮移动顶点数>0 表示蒙皮确实在动、网格确实画了出来 */
                        int ok = (moved_total > 0) ? 0 : 1;
                        if (ok) printf("[auto] FAIL: no skinned vertices moved\n");
                        SDL_Quit();
                        return ok;
                }

                fps_frames++;
                if (now - fps_ticks >= 500000000ULL) {
                        float fps = (float) ((double) fps_frames / ((double) (now - fps_ticks) / 1000000000.0));
                        printf("[frame] fps=%.1f entities=%d moved=%llu\n", fps,
                               (int) registry.view<jgltf::instance>().size(),
                               (unsigned long long) moved_total);
                        fps_frames = 0;
                        fps_ticks = now;
                }
        }

        /* ---- 清理：没有任何手工 free，全部由 RAII/ECS 托管 ---- */
        registry.clear();          /* 组件析构 -> jgltf_instance_destroy（可省略） */
        robot_asset.reset();       /* 释放共享资源（可省略，析构也会做） */
        air_asset.reset();
        r3d_destroy(r3d);
        destroy_window(window);
        SDL_Quit();
        printf("[jgltf] test finished.\n");
        return 0;
}