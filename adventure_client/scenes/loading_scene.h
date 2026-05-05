#ifndef LOADING_SCENE_H
#define LOADING_SCENE_H
#include <string>
#include <joy2d/jscene.h>
#include "../context.h"

// ============================================================
// 场景基类 - 所有场景继承此类
// 将 C 风格 jscene API 封装为 C++ 虚函数模式
// ============================================================
class BaseScene {
public:
    virtual ~BaseScene();

    // 禁止拷贝
    BaseScene(const BaseScene&) = delete;
    BaseScene& operator=(const BaseScene&) = delete;

    // --- jscene 原始句柄 ---
    scene_p get_scene() const { return m_scene; }

    // --- 子类覆写这些虚函数（默认空实现）---
    virtual void OnStart()   {}
    virtual void OnUpdate(float dt) {}
    virtual void OnRender()  {}
    virtual void OnDestroy() {}

protected:
    // 只允许子类构造
    explicit BaseScene(const char* name);

private:
    // C 回调桥接 → 虚函数
    static void Bridge_OnLoad(scene_p s);
    static void Bridge_OnUpdate(scene_p s, float dt);
    static void Bridge_OnRender(scene_p s);
    static void Bridge_OnDestroy(scene_p s);

protected:
    scene_p m_scene;
};

// ============================================================
// LoadingScene - 加载场景
// ============================================================
class LoadingScene : public BaseScene {
public:
    LoadingScene();
    ~LoadingScene() override;

protected:
    void OnStart() override;
    void OnUpdate(float dt) override;
    void OnRender() override;

private:
    float m_progress = 0.0f;
    Context2* m_ctx;
};

#endif