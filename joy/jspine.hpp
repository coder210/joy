/******************************************************************************
 * jspine.hpp —— jspine 的 C++ RAII 包装（可选头文件，仅供 C++/ECS 项目 include）
 *
 * 目的：让 Spine 资源与实例完全由 ECS/RAII 托管，不再需要手工 init/free：
 *   - jspine::asset    持有共享资源，析构自动 jspine_asset_free
 *   - jspine::instance 组件：构造即 jspine_instance_init，析构即 jspine_instance_free
 *     entt 创建实体（emplace）、销毁实体（destroy/clear）、storage 扩容移动组件
 *     都会自动走构造/析构/移动，无需调用方干预。
 *
 * C 层的 jspine_instance_t 仍是纯 POD（便于纯 C 使用与序列化），
 * 这里只是把它包进有生命周期的 C++ 类型；底层字段通过 data()/ptr() 直接访问。
 *****************************************************************************/

#ifndef JOY_JSPINE_HPP
#define JOY_JSPINE_HPP

#include <joy/joy.h>

#ifdef JOY_USE_SPINE

#include <utility>

namespace jspine {

/* ============ 共享资源（atlas + skeletonData） ============ */
class asset {
public:
        asset() = default;
        asset(jr2d_p r2d, const char *atlas_path, const char *skeleton_path, float scale = 1.0f)
                : ptr_(jspine_asset_load(r2d, atlas_path, skeleton_path, scale)) {}

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
        jspine_asset_p get() const { return ptr_; }
        void reset() {
                if (ptr_) {
                        jspine_asset_free(ptr_);
                        ptr_ = nullptr;
                }
        }

        int animation_count() const { return ptr_ ? jspine_asset_animation_count(ptr_) : 0; }
        const char *animation_name(int index) const {
                return ptr_ ? jspine_asset_animation_name(ptr_, index) : nullptr;
        }
        float animation_duration(int index) const {
                return ptr_ ? jspine_asset_animation_duration(ptr_, index) : 0.0f;
        }

private:
        jspine_asset_p ptr_ = nullptr;
};

/* ============ 实例组件 ============ */
class instance {
public:
        instance() = default; /* entt 需要可默认构造：此时未绑定资源，valid()==false */

        explicit instance(jspine_asset_p a) { bind(a); }
        explicit instance(const asset &a) { bind(a.get()); }

        ~instance() { jspine_instance_free(&inst_); }

        instance(const instance &) = delete;            /* 含裸指针，禁止拷贝 */
        instance &operator=(const instance &) = delete;

        /* entt 在 storage 扩容时会移动组件：转移所有权并把源置空，避免双重释放 */
        instance(instance &&other) noexcept { move_from(other); }
        instance &operator=(instance &&other) noexcept {
                if (this != &other) {
                        jspine_instance_free(&inst_);
                        move_from(other);
                }
                return *this;
        }

        bool bind(jspine_asset_p a) {
                jspine_instance_free(&inst_);           /* 允许重新绑定 */
                return a ? jspine_instance_init(&inst_, a) : false;
        }
        bool valid() const { return inst_.skeleton != nullptr; }

        /* 底层 POD：system 里可直接改字段（x/y/scale/alpha/tint/zorder...） */
        jspine_instance_t &data() { return inst_; }
        const jspine_instance_t &data() const { return inst_; }
        jspine_instance_p ptr() { return &inst_; }

        /* 常用操作转发 */
        void set_animation(int track, const char *name, bool loop) {
                jspine_instance_set_animation(&inst_, track, name, loop);
        }
        void add_animation(int track, const char *name, bool loop, float delay) {
                jspine_instance_add_animation(&inst_, track, name, loop, delay);
        }
        void update(float dt) { jspine_instance_update(&inst_, dt); }
        void sync() { jspine_instance_sync(&inst_); }
        void draw(jr2d_p r2d) { jspine_instance_draw(r2d, &inst_); }
        void set_position(float x, float y) { jspine_instance_set_position(&inst_, x, y); }
        void set_scale(float sx, float sy) { jspine_instance_set_scale(&inst_, sx, sy); }
        void set_rotation(float degrees) { jspine_instance_set_rotation(&inst_, degrees); }
        void set_flip_x(bool flip) { jspine_instance_set_flip_x(&inst_, flip); }
        void set_alpha(float alpha) { jspine_instance_set_alpha(&inst_, alpha); }
        void set_tint(jcolor_t tint) { jspine_instance_set_tint(&inst_, tint); }

private:
        void move_from(instance &other) {
                inst_ = other.inst_;                    /* POD 浅拷贝 = 指针所有权转移 */
                other.inst_ = jspine_instance_t{};      /* 源置空，析构时不会释放资源 */
        }

        jspine_instance_t inst_{};                      /* 零初始化 */
};

} // namespace jspine

#endif /* JOY_USE_SPINE */

#endif /* JOY_JSPINE_HPP */
