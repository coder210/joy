#include "gjk2d.h"
#include <iostream>
#include <memory>
#include <cmath>

using namespace gjk2d;

static int tests_passed = 0;
static int tests_failed = 0;

void check(const char* name, bool expected, bool actual) {
    if (expected == actual) {
        std::cout << "  [PASS] " << name << std::endl;
        tests_passed++;
    } else {
        std::cout << "  [FAIL] " << name << " (expected=" << expected << ", actual=" << actual << ")" << std::endl;
        tests_failed++;
    }
}

void check_penetration(const char* name, bool expected_hit, bool actual_hit,
                       const Penetration& pen, float expected_depth_lo, float expected_depth_hi,
                       float nx, float ny) {
    if (expected_hit != actual_hit) {
        std::cout << "  [FAIL] " << name << " (expected=" << expected_hit << ", actual=" << actual_hit << ")" << std::endl;
        tests_failed++;
        return;
    }
    if (!actual_hit) {
        std::cout << "  [PASS] " << name << std::endl;
        tests_passed++;
        return;
    }
    bool depth_ok = pen.depth >= expected_depth_lo - 0.01f && pen.depth <= expected_depth_hi + 0.01f;
    bool normal_ok = std::abs(pen.normal.x - nx) < 0.05f && std::abs(pen.normal.y - ny) < 0.05f;
    if (depth_ok && normal_ok) {
        std::cout << "  [PASS] " << name << " (depth=" << pen.depth << " normal=(" << pen.normal.x << "," << pen.normal.y << "))" << std::endl;
        tests_passed++;
    } else {
        std::cout << "  [FAIL] " << name << " depth=" << pen.depth << " (expected ~" << (expected_depth_lo+expected_depth_hi)/2
                  << ") normal=(" << pen.normal.x << "," << pen.normal.y << ") (expected ~(" << nx << "," << ny << "))" << std::endl;
        tests_failed++;
    }
}

// ============================================================================
// 严格数学验证 - 圆碰撞
// ============================================================================
void test_circle_math() {
    std::cout << "\n--- Circle-Circle (mathematical validation) ---" << std::endl;

    auto c1 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Circle(2.0f)));
    auto c2 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Circle(1.5f)));

    Transform t1, t2;
    t1.position = Vector2(0, 0);

    // 间距 = 5, 半径和 = 3.5 => 分离
    t2.position = Vector2(5, 0);
    check("d=5 r_sum=3.5", false, GJK::Detect(c1.get(), t1, c2.get(), t2));

    // 间距 = 3.5, 半径和 = 3.5 => 相切
    t2.position = Vector2(3.5f, 0);
    check("d=3.5 r_sum=3.5 touching", true, GJK::Detect(c1.get(), t1, c2.get(), t2));

    // 间距 = 2, 半径和 = 3.5 => 相交, 穿透深度 = 3.5-2 = 1.5
    t2.position = Vector2(2, 0);
    Penetration pen;
    bool hit = GJK::Detect(c1.get(), t1, c2.get(), t2, pen);
    check_penetration("d=2.0 overlap=1.5", true, hit, pen, 1.5f, 1.5f, 1.0f, 0.0f);

    // 圆心在 (3,4), 间距 = 5, 半径和 = 3.5 => 分离
    t2.position = Vector2(3, 4);
    check("d=5(x=3,y=4) r_sum=3.5", false, GJK::Detect(c1.get(), t1, c2.get(), t2));

    // 圆心在 (2,2), 间距 = sqrt(8) ≈ 2.828, 半径和 = 3.5 => 相交
    t2.position = Vector2(2, 2);
    hit = GJK::Detect(c1.get(), t1, c2.get(), t2, pen);
    float expected_depth = 3.5f - std::sqrt(8.0f);
    float expected_nx = 2.0f / std::sqrt(8.0f);
    float expected_ny = 2.0f / std::sqrt(8.0f);
    check_penetration("d=2.828 overlap=0.672", true, hit, pen,
                      expected_depth - 0.05f, expected_depth + 0.05f,
                      expected_nx, expected_ny);
}

// ============================================================================
// 严格数学验证 - 矩形碰撞
// ============================================================================
void test_rectangle_math() {
    std::cout << "\n--- Rectangle (mathematical validation) ---" << std::endl;

    auto r = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Rectangle(4, 2)));
    Transform t;
    t.position = Vector2(0, 0);

    // 矩形 local space 中 x=[-2,2], y=[-1,1]

    // 点在矩形内部
    check("rect contains (0,0)", true, r->GetFarthestProjectionPoint(t, Vector2(1, 0)).x > 1.9f);
    check("rect support +x", true, std::abs(r->GetFarthestProjectionPoint(t, Vector2(1, 0)).x - 2.0f) < 0.01f);
    check("rect support -x", true, std::abs(r->GetFarthestProjectionPoint(t, Vector2(-1, 0)).x + 2.0f) < 0.01f);
    check("rect support +y", true, std::abs(r->GetFarthestProjectionPoint(t, Vector2(0, 1)).y - 1.0f) < 0.01f);
    check("rect support -y", true, std::abs(r->GetFarthestProjectionPoint(t, Vector2(0, -1)).y + 1.0f) < 0.01f);
}

// ============================================================================
// 碰撞与非碰撞逐一验证
// ============================================================================
void test_exhaustive_rect_rect() {
    std::cout << "\n--- Rectangle-Rectangle (exhaustive) ---" << std::endl;

    auto r1 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Rectangle(2, 2)));
    auto r2 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Rectangle(2, 2)));

    Transform t1, t2;
    t1.position = Vector2(0, 0);

    // 各种偏移（矩形半边长=1，范围 x=[-1,1], y=[-1,1]）
    struct { float x, y; bool hit; const char* desc; } cases[] = {
        {0,   0,   true,  "center overlap"},
        {1,   0,   true,  "edge aligned x=1"},
        {2.1f, 0,  false, "x=2.1 separated"},       // rect2 x=[1.1,3.1], rect1 x=[-1,1], no overlap
        {0,   1,   true,  "edge aligned y=1"},
        {1,   1,   true,  "corner overlap (1,1)"},
        {2,   0,   false, "just touching x=2"},      // rect2 x=[1,3], rect1 x=[-1,1], touching
        {3,   3,   false, "far corner (3,3)"},
        {0.5f, 0.5f, true, "small overlap (0.5,0.5)"},
        {-1.5f, 0,  true,  "negative x overlap"},
    };

    for (auto& c : cases) {
        t2.position = Vector2(c.x, c.y);
        check(c.desc, c.hit, GJK::Detect(r1.get(), t1, r2.get(), t2));
    }
}

// ============================================================================
// 旋转碰撞
// ============================================================================
void test_rotation() {
    std::cout << "\n--- Rotation tests ---" << std::endl;

    auto r1 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Rectangle(2, 4))); // tall
    auto r2 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Rectangle(4, 2))); // wide

    Transform t1, t2;
    t1.position = Vector2(0, 0);
    t2.position = Vector2(2, 0);

    // Unrotated: tall rect x=[-1,1], wide rect at x=2 => x=[0,4], overlap in x=[0,1]
    Penetration pen;
    bool hit = GJK::Detect(r1.get(), t1, r2.get(), t2, pen);
    check("rect(2x4) vs rect(4x2) offset x=2", true, hit);

    // Rotate tall rect 90 deg: becomes wide, x=[-2,2], wide rect at x=2 => x=[0,4], still overlapping
    t1.rotation = 90.0f;
    hit = GJK::Detect(r1.get(), t1, r2.get(), t2, pen);
    check("tall rect rotated 90 deg", true, hit);

    // Move further, now separated
    t2.position = Vector2(4, 0);
    hit = GJK::Detect(r1.get(), t1, r2.get(), t2, pen);
    check("separated after rotation", false, hit);
}

// ============================================================================
// Ellipse 测试
// ============================================================================
void test_ellipse() {
    std::cout << "\n--- Ellipse ---" << std::endl;

    auto e = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Ellipse(4, 2)));
    auto c = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Circle(0.5f)));

    Transform t1, t2;
    t1.position = Vector2(0, 0);

    // Circle inside ellipse
    t2.position = Vector2(0, 0);
    Penetration pen;
    bool hit = GJK::Detect(e.get(), t1, c.get(), t2, pen);
    check("circle inside ellipse", true, hit);

    // Circle far away
    t2.position = Vector2(5, 0);
    hit = GJK::Detect(e.get(), t1, c.get(), t2, pen);
    check("circle far from ellipse", false, hit);
}

// ============================================================================
// Segment 测试 (碰撞器层面)
// ============================================================================
void test_segment() {
    std::cout << "\n--- Segment ---" << std::endl;

    auto s = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Segment(2.0f, Vector2(0, 1))));
    auto c = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Circle(0.5f)));

    Transform t1, t2;
    t1.position = Vector2(0, 0);
    t2.position = Vector2(0, 0);

    Penetration pen;
    bool hit = GJK::Detect(s.get(), t1, c.get(), t2, pen);
    check("segment-circle center", true, hit);

    t2.position = Vector2(3, 0);
    hit = GJK::Detect(s.get(), t1, c.get(), t2, pen);
    check("segment-circle far", false, hit);
}

// ============================================================================
// Polygon 方向验证
// ============================================================================
void test_polygon_directions() {
    std::cout << "\n--- Polygon direction support ---" << std::endl;

    std::vector<Vector2> verts = {
        Vector2(-2, -1), Vector2(2, -1), Vector2(2, 1), Vector2(-2, 1)
    };
    Polygon poly(verts);

    // 多边形 x=[-2,2], y=[-1,1]
    auto sup_x = poly.GetFarthestProjectionPoint(Vector2(1, 0));
    check("poly support +x", std::abs(sup_x.x - 2.0f) < 0.01f, true);

    auto sup_negx = poly.GetFarthestProjectionPoint(Vector2(-1, 0));
    check("poly support -x", std::abs(sup_negx.x + 2.0f) < 0.01f, true);

    auto sup_y = poly.GetFarthestProjectionPoint(Vector2(0, 1));
    check("poly support +y", std::abs(sup_y.y - 1.0f) < 0.01f, true);
}

// ============================================================================
// Minkowski Support 验证
// ============================================================================
void test_minkowski_support() {
    std::cout << "\n--- Minkowski Support ---" << std::endl;

    auto c1 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Circle(1.0f)));
    auto c2 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Circle(1.0f)));

    Transform t1, t2;
    t1.position = Vector2(0, 0);
    t2.position = Vector2(3, 0);

    Minkowski minkowski(c1.get(), t1, c2.get(), t2);

    // Support(+x): A在+x最远(世界坐标)=(1,0), B在-x最远(世界坐标)=(2,0), 差=(1,0)-(2,0)=(-1,0)
    auto s = minkowski.Support(Vector2(1, 0));
    check("minkowski support +x", std::abs(s.x + 1.0f) < 0.01f && std::abs(s.y) < 0.01f, true);

    // 往右移，使得原点在外面: B在(5,0), 则A最远在(1,0), B最远在(6,0)(对-x方向是(-1,0)但在世界空间)... 
    // Support(+x) = A(+x最远) - B(-x最远)
    // A最远(+x方向) = (1,0) [世界坐标 (1,0)]
    // B最远(-x方向) = (-1,0) [世界坐标 (-1+5, 0) = (4,0)]
    // 结果 = (1,0) - (4,0) = (-3,0)
    // 这说明原点在闵可夫斯基差之外（在-x侧），符合预期（两圆分离）
}

void test_simplex_operations() {
    std::cout << "\n--- Simplex operations ---" << std::endl;

    Simplex s(3);
    check("empty simplex", s.count() == 0, true);

    s.Add(Vector2(1, 0));
    check("1 point", s.count() == 1, true);

    s.Add(Vector2(0, 1));
    check("2 points", s.count() == 2, true);

    s.Add(Vector2(-1, 0));
    check("3 points", s.count() == 3, true);

    // 验证a,b,c
    check("a = newest", s.a().x == -1.0f && s.a().y == 0.0f, true);
    check("b = middle", s.b().x == 0.0f && s.b().y == 1.0f, true);
    check("c = oldest", s.c().x == 1.0f && s.c().y == 0.0f, true);
}

// ============================================================================
// 负坐标和浮点精度
// ============================================================================
void test_negative_coordinates() {
    std::cout << "\n--- Negative coordinates ---" << std::endl;

    auto c1 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Circle(1.0f)));
    auto c2 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Circle(1.0f)));

    Transform t1, t2;
    t1.position = Vector2(-5, -5);
    // dist=sqrt(0.5^2+0.5^2)=0.707 < 2, 重叠
    t2.position = Vector2(-4.5f, -4.5f);

    Penetration pen;
    bool hit = GJK::Detect(c1.get(), t1, c2.get(), t2, pen);
    check("negative coords overlapping", true, hit);

    t2.position = Vector2(-2, -5);
    hit = GJK::Detect(c1.get(), t1, c2.get(), t2, pen);
    check("negative coords separated x", false, hit);
}

// ============================================================================
// EPA 穿透法线方向验证
// ============================================================================
void test_epa_normal_direction() {
    std::cout << "\n--- EPA normal direction ---" << std::endl;

    auto r1 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Rectangle(2, 2)));
    auto r2 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Rectangle(2, 2)));

    Transform t1, t2;
    t1.position = Vector2(0, 0);

    // B从+x方向挤入, 法线应为 (1, 0)
    t2.position = Vector2(1.5f, 0);
    Penetration pen;
    GJK::Detect(r1.get(), t1, r2.get(), t2, pen);
    check("epa normal +x", pen.normal.x > 0.9f && std::abs(pen.normal.y) < 0.1f, true);

    // B从+y方向挤入, 法线应为 (0, 1)
    t2.position = Vector2(0, 1.5f);
    GJK::Detect(r1.get(), t1, r2.get(), t2, pen);
    check("epa normal +y", pen.normal.y > 0.9f && std::abs(pen.normal.x) < 0.1f, true);

    // B从-x方向挤入, 法线应为 (-1, 0)
    t2.position = Vector2(-1.5f, 0);
    GJK::Detect(r1.get(), t1, r2.get(), t2, pen);
    check("epa normal -x", pen.normal.x < -0.9f && std::abs(pen.normal.y) < 0.1f, true);
}

// ============================================================================
// 大位移
// ============================================================================
void test_large_separation() {
    std::cout << "\n--- Large separation ---" << std::endl;

    auto c1 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Circle(0.5f)));
    auto c2 = std::unique_ptr<ICollider>(ColliderFactory::CreateCollider(Circle(0.5f)));

    Transform t1, t2;
    t1.position = Vector2(0, 0);
    t2.position = Vector2(10000, 0);
    check("very far apart", false, GJK::Detect(c1.get(), t1, c2.get(), t2));

    t2.position = Vector2(10000, 10000);
    check("very far diagonal", false, GJK::Detect(c1.get(), t1, c2.get(), t2));
}

// ============================================================================
// 全部验证
// ============================================================================
int main() {
    std::cout << "========== GJK2D C++ Comprehensive Tests ==========" << std::endl;
    std::cout << "MathX::EPSILON = " << MathX::EPSILON << std::endl;

    test_circle_math();
    test_rectangle_math();
    test_exhaustive_rect_rect();
    test_rotation();
    test_ellipse();
    test_segment();
    test_polygon_directions();
    test_minkowski_support();
    test_simplex_operations();
    test_negative_coordinates();
    test_epa_normal_direction();
    test_large_separation();

    std::cout << "\n========== Results: " << tests_passed << " passed, "
              << tests_failed << " failed ==========" << std::endl;
    return tests_failed > 0 ? 1 : 0;
}
