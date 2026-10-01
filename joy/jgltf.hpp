/******************************************************************************
 * jgltf.hpp —— jgltf 的 C++ RAII 包装（可选头文件，仅供 C++/ECS 项目 include）
 *
 * 目的：让 glTF 资源与实例完全由 ECS/RAII 托管，不再需要手工管理引用计数：
 *   - jgltf::asset    持有共享资源（引用计数），析构自动 jgltf_asset_release
 *   - jgltf::instance 组件：构造即 jgltf_instance_create，析构即 jgltf_instance_destroy
 *     entt 创建实体（emplace）、销毁实体（destroy/clear）、storage 扩容移动组件
 *     都会自动走构造/析构/移动，无需调用方干预。
 *
 * C 层的 jgltf_asset_t / jgltf_instance_t 仍是纯 POD（便于纯 C 使用），
 * 这里只是把指针包进有生命周期的 C++ 类型；底层实例通过 get()/ptr() 直接访问。
 *****************************************************************************/

#ifndef JOY_JGLTF_HPP
#define JOY_JGLTF_HPP

#include <joy/joy.h>

#ifdef JOY_USE_GLTF

#include <utility>

namespace jgltf {

/* ============ 共享资源（引用计数） ============ */
class asset {
public:
        asset() = default;
        asset(jr3d_p r3d, const char *path) : ptr_(jgltf_asset_load(r3d, path)) {}

        ~asset() { reset(); }

        asset(const asset &) = delete;
        asset &operator=(const asset &) = delete;

        asset(asset &&other) noexcept : ptr_(other.ptr_) { other.ptr_ = nullptr; }
        asset &operator=(asset &&other) noexcept {
                if (this != &other) {
                        reset();
                        ptr_ = other.ptr_;
                        other.ptr_ = nullptr;
                }
                return *this;
        }

        bool valid() const { return ptr_ != nullptr; }
        jgltf_asset_p get() const { return ptr_; }

        void reset() {
                if (ptr_) {
                        jgltf_asset_release(ptr_);
                        ptr_ = nullptr;
                }
        }

        int animation_count() const { return jgltf_asset_animation_count(ptr_); }
        const char *animation_name(int index) const {
                return jgltf_asset_animation_name(ptr_, index);
        }
        float animation_duration(int index) const {
                return jgltf_asset_animation_duration(ptr_, index);
        }
        int node_count() const { return jgltf_asset_node_count(ptr_); }
        int prim_count() const { return jgltf_asset_prim_count(ptr_); }
        int has_skin() const { return jgltf_asset_has_skin(ptr_); }
        void bounds(vec3_t *center, float *radius) const {
                jgltf_asset_bounds(ptr_, center, radius);
        }

private:
        jgltf_asset_p ptr_ = nullptr;
};

/* ============ 实例组件（每实体一份，持有独立 GPU 蒙皮缓冲） ============ */
class instance {
public:
        instance() = default; /* entt 需要可默认构造：此时未绑定资源，valid()==false */

        explicit instance(jgltf_asset_p a) { bind(a); }
        explicit instance(const asset &a) { bind(a.get()); }

        ~instance() { destroy(); }

        instance(const instance &) = delete;
        instance &operator=(const instance &) = delete;

        /* entt 在 storage 扩容时移动组件：转移所有权并把源置空，避免双重释放 */
        instance(instance &&other) noexcept : ptr_(other.ptr_) { other.ptr_ = nullptr; }
        instance &operator=(instance &&other) noexcept {
                if (this != &other) {
                        destroy();
                        ptr_ = other.ptr_;
                        other.ptr_ = nullptr;
                }
                return *this;
        }

        bool bind(jgltf_asset_p a) {
                destroy();                 /* 允许重新绑定 */
                if (!a) return false;
                ptr_ = jgltf_instance_create(a);
                return ptr_ != nullptr;
        }
        bool valid() const { return ptr_ != nullptr; }
        jgltf_instance_p ptr() const { return ptr_; }

        void set_animation(int index) { jgltf_instance_set_animation(ptr_, index); }
        int  animation() const { return jgltf_instance_animation(ptr_); }
        void set_speed(float speed) { jgltf_instance_set_speed(ptr_, speed); }
        void set_paused(bool paused) { jgltf_instance_set_paused(ptr_, paused); }
        void update(float dt) { jgltf_instance_update(ptr_, dt); }
        void draw(jr3d_p r3d, const mat44_t *model) { jgltf_instance_draw(r3d, ptr_, model); }
        void draw_bones(jr3d_p r3d, const mat44_t *model) {
                jgltf_instance_draw_bones(r3d, ptr_, model);
        }
        int moved_vertices() const { return jgltf_instance_moved_vertices(ptr_); }

private:
        void destroy() {
                if (ptr_) {
                        jgltf_instance_destroy(ptr_);
                        ptr_ = nullptr;
                }
        }

        jgltf_instance_p ptr_ = nullptr;
};

} // namespace jgltf

#endif /* JOY_USE_GLTF */

#endif /* JOY_JGLTF_HPP */