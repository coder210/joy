/* ============================================================
 * joy 示例 · C++ 游戏 DLL（native plugin 版）
 *
 * 480x320 顶视角小游戏，与 csharp / lua / js 三个示例功能完全相同：
 *   Menu → Play（WASD 移动 / Space 射击 / 敌人追击 / 碰撞掉血）→ GameOver
 *
 * 这一版把游戏逻辑外置成原生 DLL：宿主只负责 SDL 主循环、加载本 DLL 并
 * 注入 JoyApi 大函数指针表；窗口、场景、物理、绘制全在这里，且所有引擎
 * 调用都经 g_api->Xxx()，本 DLL 不链接 joy::all。它是另外三个示例的
 * 「行为参照物」——同样的固定种子、同样的运动学常量、同样的冒烟输入序列，
 * 因此冒烟输出的指纹数值应当一致。
 *
 * 逻辑内部用 double 计算：Lua / JS 只有 double，若这里用 float，
 * 两者的位置累积会产生细微漂移，冒烟指纹就对不上了。
 * ============================================================ */
#include "joy_game_hooks.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/* 与 SDL3 SDL_Scancode（USB HID）取值逐字对齐：宿主用 e->key.scancode
 * 直接索引 JoyInput.keys[512]，两侧必须一致。这里不 include 任何 SDL 头，
 * DLL 因此完全不依赖 SDL 的生成配置头；若以后要 include SDL.h，请删掉本枚举。 */
enum {
    SDL_SCANCODE_A      = 4,
    SDL_SCANCODE_D      = 7,
    SDL_SCANCODE_J      = 13,
    SDL_SCANCODE_S      = 22,
    SDL_SCANCODE_W      = 26,
    SDL_SCANCODE_RETURN = 40,
    SDL_SCANCODE_ESCAPE = 41,
    SDL_SCANCODE_SPACE  = 44,
    SDL_SCANCODE_RIGHT  = 79,
    SDL_SCANCODE_LEFT   = 80,
    SDL_SCANCODE_DOWN   = 81,
    SDL_SCANCODE_UP     = 82,
};

/* DLL 不 include joy.h，这里按引擎公开布局重放几个纯值类型与常量：
 * 仅用于 ABI 自检；跨 DLL 边界传递的只有标量，没有结构体。 */
struct vec2_t  { float x, y; };
struct jrect_t { float x, y, w, h; };
#define JR2D_BLEND_BLEND 1

/* 引擎裸指针在 JoyApi 里是强句柄薄包装；用别名让游戏逻辑主体少改，
 * 空值判断统一用 .h（uint64_t），编译期仍不能混传不同类型的句柄。 */
using jwindow_p = Jwindow;
using jr2d_p    = Jr2d;
using image_p   = Image;
using font_p    = Font;

/* ---------------- 规格常量（四份实现共用，改动需四份同步） ---------------- */
static const int   APP_W = 480;
static const int   APP_H = 320;

static const float ARENA_MARGIN      = 10.0f;
static const float ENEMY_CONTACT_DPS = 26.0f;
static const int   MAX_ENEMIES       = 24;
static const int   MAX_BULLETS       = 64;

static const float PLAYER_RADIUS        = 11.0f;
static const float PLAYER_SPEED         = 130.0f;
static const float PLAYER_MAX_HP        = 100.0f;
static const float PLAYER_FIRE_INTERVAL = 0.16f;

static const float ENEMY_RADIUS = 11.0f;

static const float BULLET_RADIUS = 3.0f;
static const float BULLET_SPEED  = 320.0f;
static const float BULLET_LIFE   = 1.5f;
static const float BULLET_DAMAGE = 1.0f;

static const int SMOKE_MENU_ENTER_A = 2;    /* 第 2~3 帧按 Enter（菜单 → 游戏） */
static const int SMOKE_MENU_ENTER_B = 4;
static const int SMOKE_OVER_ENTER_A = 126;  /* 第 126~127 帧按 Enter（结算 → 重开） */
static const int SMOKE_OVER_ENTER_B = 128;
static const int SMOKE_MOVE_X_A     = 10;   /* 10~89 帧按住右 */
static const int SMOKE_MOVE_X_B     = 90;
static const int SMOKE_MOVE_Y_A     = 10;   /* 10~59 帧按住下 */
static const int SMOKE_MOVE_Y_B     = 60;
static const int SMOKE_FIRE_A       = 20;   /* 20 帧起每 12 帧按 2 帧 */
static const int SMOKE_FIRE_PERIOD  = 12;
static const int SMOKE_FIRE_HOLD    = 2;
static const int SMOKE_KILL_FRAME   = 120;  /* 120 帧直接打死玩家 */
static const int SMOKE_ASSERT_FRAME = 150;

/* ---------------- 确定性随机（四份实现共用同一算法） ----------------
 * xorshift32 + 取高 24 位转 [0,1)：四种语言都能逐位复现，
 * 不用各语言的库 RNG（C# 的 System.Random 算法在其它语言里没有等价品）。 */
struct Rng {
    unsigned state;

    void seed(unsigned v) { state = v ? v : 1u; }

    unsigned next_u32(void)
    {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        return state;
    }

    double next_double(void)
    {
        return (double)(next_u32() >> 8) * (1.0 / 16777216.0);
    }

    int next_int(int n)
    {
        return (int)(next_u32() % (unsigned)n);
    }

    double range(double lo, double hi)
    {
        return lo + next_double() * (hi - lo);
    }
};

struct Enemy {
    bool   alive;
    double x, y;
    double spawn_x, spawn_y;
    double hp;
    double speed;

    bool approached(double px, double py) const
    {
        double sx = spawn_x - px, sy = spawn_y - py;
        double nx = x - px, ny = y - py;
        double before = sqrt(sx * sx + sy * sy);
        double now = sqrt(nx * nx + ny * ny);
        return now < before - 2.0;
    }
};

struct Bullet {
    bool   alive;
    double x, y, vx, vy;
    double life;
    double damage;
};

enum SceneId {
    SCENE_MENU = 0,
    SCENE_PLAY,
    SCENE_OVER,
};

struct Game {
    /* JoyApi 用强句柄（uint64_t h）代替裸指针，空值用 {} 并判 .h */
    jwindow_p window = {};
    jr2d_p    r2d = {};

    image_p img_player = {};
    image_p img_enemy = {};
    font_p  font_small = {};
    font_p  font_big = {};
    bool    has_font = false;

    SceneId scene = SCENE_MENU;

    /* 玩家 */
    double px = 0, py = 0;
    double player_hp = PLAYER_MAX_HP;
    double fire_cd = 0;
    double facing_x = 1, facing_y = 0;

    Enemy  enemies[MAX_ENEMIES];
    Bullet bullets[MAX_BULLETS];
    int    enemy_count = 0;   /* 活跃数量（数组里可能有空槽） */

    double elapsed = 0;
    double spawn_timer = 0;
    int    kills = 0;
    int    fired = 0;
    bool   finished = false;

    int    frame = 0;
    int    smoke_frames = 0;
    bool   quit = false;
    int    exit_code = 0;

    /* 输入快照：真实按键 + 冒烟脚本模拟按键合成。
     * real_keys 必须是独立的一份：若把 scripted 直接写进 keys，脚本松开后
     * keys 仍是 true，按键永远释放不掉（表现为一直按住 → 移动距离与射速全错）。 */
    bool  real_keys[512] = { false };
    bool  keys[512] = { false };
    bool  prev[512] = { false };
    bool  scripted[512] = { false };
    float mouse_x = 0, mouse_y = 0;
    bool  mouse_left = false;

    /* 冒烟观察量（对应 C# SmokeScript 的字段） */
    int    play_count = 0;
    bool   saw_over = false;
    bool   in_play = false;
    bool   enemy_moved = false;
    double start_x = 0, start_y = 0, max_moved = 0;
    int    max_fired = 0;
    int    max_kills = 0;
    int    max_enemies = 0;
    bool   killed_player = false;
    bool   done = false;
    bool   abi_ok = false;
    bool   checked_abi = false;

    Rng rng;
};

static Game* g_game = NULL;
static const JoyApi* g_api = NULL;   /* 宿主注入；DLL 内所有引擎调用都经它 */

/* ---------------- 绘制辅助（等价于 C# 的 Painter） ----------------
 * 全部经 g_api 的强签名函数指针调用，坐标一律用像素：*_f 系列是归一化比例，
 * 会经 r2d_fpx() 乘窗口宽高，传像素坐标会把精灵放大到屏幕外。
 * 图片绘制走全标量 L1 ImageDrawRect（srcrect 固定 NULL），绕开 jrect_t*
 * 无法进 joy_value 的 L3 限制；只有文字宽度仍按字号估算（jtext_measurex
 * 是 int* 出参）。绘制不参与冒烟指纹，逻辑与指纹数值不变。 */

static void set_color(jr2d_p r2d, int r, int gr, int b, int a)
{
    g_api->R2dSetColorf(g_api->ctx, r2d, (float)r / 255.0f,
                        (float)gr / 255.0f, (float)b / 255.0f,
                        (float)a / 255.0f);
}

/* 注意坐标系：*_f 系列（ShapeDrawRectangleF / ShapeDrawLineF）的参数是
 * 归一化比例，r2d_fpx() 会乘窗口宽高再画；这里传的是像素坐标，必须用
 * 非 _f 的 ShapeDrawRectangle（L2 按字段展平）/ ShapeDrawLine（L1）。
 * 否则一个 x=227、w=26 的精灵色块会被放大成 x=108960、w=12480，
 * 整个跑到屏幕外，表现为「玩家和敌人没渲染出来」。 */
static void fill_rect(float x, float y, float w, float h,
                      int r, int gr, int b, int a = 255)
{
    set_color(g_game->r2d, r, gr, b, a);
    g_api->ShapeDrawRectangle(g_api->ctx, g_game->r2d, "fill", x, y, w, h);
}

static void stroke_rect(float x, float y, float w, float h,
                        int r, int gr, int b, int a = 255)
{
    set_color(g_game->r2d, r, gr, b, a);
    g_api->ShapeDrawRectangle(g_api->ctx, g_game->r2d, "line", x, y, w, h);
}

static void draw_line(float x1, float y1, float x2, float y2,
                      int r, int gr, int b, int a = 255)
{
    set_color(g_game->r2d, r, gr, b, a);
    g_api->ShapeDrawLine(g_api->ctx, g_game->r2d, x1, y1, x2, y2);
}

/* 与 lua 示例的 utf8.len(s) * size * 0.58 一致 */
static float text_width_estimate(const char* s, int size)
{
    int n = 0;
    for (const unsigned char* p = (const unsigned char*)s; *p; ++p) {
        if ((*p & 0xC0) != 0x80) ++n;
    }
    return (float)(n * size) * 0.58f;
}

/* JtextPrintx 是 L2 展开签名：jcolor_t 按 r/g/b/a 四个 uint8_t 摊平 */
static void draw_text(const char* s, float x, float y,
                      int r = 232, int gr = 236, int b = 245)
{
    if (!g_game->font_small.h) return;
    g_api->JtextPrintx(g_api->ctx, g_game->r2d, g_game->font_small,
                       s, (int)strlen(s), (uint8_t)r, (uint8_t)gr,
                       (uint8_t)b, (uint8_t)255, x, y);
}

static void draw_text_center(const char* s, float cx, float cy, bool big = false,
                             int r = 232, int gr = 236, int b = 245)
{
    font_p f = big ? g_game->font_big : g_game->font_small;
    if (!f.h) return;
    int size = big ? 22 : 12;
    float w = text_width_estimate(s, size);
    g_api->JtextPrintx(g_api->ctx, g_game->r2d, f, s, (int)strlen(s),
                       (uint8_t)r, (uint8_t)gr, (uint8_t)b, (uint8_t)255,
                       cx - w * 0.5f, cy - (float)size * 0.5f);
}

static int text_width(const char* s)
{
    if (!g_game->font_small.h) return 0;
    return (int)text_width_estimate(s, 12);
}

/* 以 (cx, cy) 为中心画精灵：ImageDrawRect 是全标量版（srcrect 固定 NULL 表示
 * 整张图），绕开了 joy_value 无法表达 jrect_t* 的 L3 限制。缺图时仍退色块。 */
static void draw_sprite(image_p img, float cx, float cy, float size,
                        int fr, int fg, int fb)
{
    if (img.h) {
        g_api->ImageDrawRect(g_api->ctx, g_game->r2d, img,
                          cx - size * 0.5f, cy - size * 0.5f, size, size);
    } else {
        fill_rect(cx - size * 0.5f, cy - size * 0.5f, size, size, fr, fg, fb);
    }
}

static const char* pick(const char* zh, const char* en)
{
    return g_game->has_font ? zh : en;
}

/* ---------------- 资源 ---------------- */

static void load_assets(void)
{
    g_game->img_player = g_api->ImageCreate(g_api->ctx, g_game->r2d,
                                            "sprites/player.png");
    g_game->img_enemy = g_api->ImageCreate(g_api->ctx, g_game->r2d,
                                           "sprites/enemy.png");

    static const char* kFonts[] = {
        "fonts/simhei.ttf",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/Deng.ttf",
    };
    for (size_t i = 0; i < sizeof(kFonts) / sizeof(kFonts[0]); ++i) {
        font_p small = g_api->FontCreate(g_api->ctx, kFonts[i], 12);
        if (!small.h) continue;
        g_game->font_small = small;
        g_game->font_big = g_api->FontCreate(g_api->ctx, kFonts[i], 22);
        g_game->has_font = true;
        break;
    }

    if (!g_game->img_player.h) printf("[warn] missing sprites/player.png\n");
    if (!g_game->img_enemy.h) printf("[warn] missing sprites/enemy.png\n");
    if (!g_game->font_small.h) printf("[warn] no usable font, HUD text disabled\n");
}

/* ---------------- 场景切换 ---------------- */

static void enter_menu(void)
{
    g_game->scene = SCENE_MENU;
    printf("[scene] Menu\n");
    fflush(stdout);
}

static void enter_play(void)
{
    Game* g = g_game;
    g->scene = SCENE_PLAY;
    g->px = APP_W * 0.5;
    g->py = APP_H * 0.5;
    g->player_hp = PLAYER_MAX_HP;
    g->fire_cd = 0;
    g->facing_x = 1;
    g->facing_y = 0;
    g->elapsed = 0;
    g->kills = 0;
    g->fired = 0;
    g->finished = false;
    g->enemy_count = 0;
    memset(g->enemies, 0, sizeof(g->enemies));
    memset(g->bullets, 0, sizeof(g->bullets));
    g->spawn_timer = g->smoke_frames > 0 ? 1.5 : 1.0;

    printf("[scene] Play\n");

    /* 冒烟：三个低血量敌人摆在正右 / 右下 / 正下，脚本的斜向射击能命中 */
    if (g->smoke_frames > 0) {
        for (int i = 0; i < 3; ++i) {
            double ex = 0, ey = 0;
            if (i == 0)      { ex = g->px + 90.0; ey = g->py; }
            else if (i == 1) { ex = g->px + 62.0; ey = g->py + 62.0; }
            else             { ex = g->px;        ey = g->py + 90.0; }
            Enemy* e = &g->enemies[g->enemy_count++];
            e->alive = true;
            e->x = e->spawn_x = ex;
            e->y = e->spawn_y = ey;
            e->hp = 2.0;
            e->speed = 44.0 + g->rng.next_double() * 10.0;
        }
    }
    fflush(stdout);
}

static void enter_over(int kills, double seconds)
{
    g_game->scene = SCENE_OVER;
    printf("[scene] GameOver kills=%d time=%.1f\n", kills, seconds);
    fflush(stdout);
}

/* ---------------- 更新 ---------------- */

static void update_player(double dt)
{
    Game* g = g_game;
    double dx = 0, dy = 0;

    if (g->keys[SDL_SCANCODE_A] || g->keys[SDL_SCANCODE_LEFT])  dx -= 1;
    if (g->keys[SDL_SCANCODE_D] || g->keys[SDL_SCANCODE_RIGHT]) dx += 1;
    if (g->keys[SDL_SCANCODE_W] || g->keys[SDL_SCANCODE_UP])    dy -= 1;
    if (g->keys[SDL_SCANCODE_S] || g->keys[SDL_SCANCODE_DOWN])  dy += 1;

    if (dx != 0 || dy != 0) {
        double len = sqrt(dx * dx + dy * dy);
        dx /= len;
        dy /= len;
        g->px += dx * PLAYER_SPEED * dt;
        g->py += dy * PLAYER_SPEED * dt;
        g->facing_x = dx;
        g->facing_y = dy;
    }

    double min_x = ARENA_MARGIN + PLAYER_RADIUS;
    double max_x = APP_W - ARENA_MARGIN - PLAYER_RADIUS;
    double min_y = ARENA_MARGIN + PLAYER_RADIUS;
    double max_y = APP_H - ARENA_MARGIN - PLAYER_RADIUS;
    if (g->px < min_x) g->px = min_x;
    if (g->px > max_x) g->px = max_x;
    if (g->py < min_y) g->py = min_y;
    if (g->py > max_y) g->py = max_y;

    g->fire_cd -= dt;
    bool want_fire = g->keys[SDL_SCANCODE_SPACE] || g->keys[SDL_SCANCODE_J] || g->mouse_left;
    if (want_fire && g->fire_cd <= 0) {
        g->fire_cd = PLAYER_FIRE_INTERVAL;
        for (int i = 0; i < MAX_BULLETS; ++i) {
            if (g->bullets[i].alive) continue;
            Bullet* b = &g->bullets[i];
            b->alive = true;
            b->x = g->px + g->facing_x * (PLAYER_RADIUS + 4.0);
            b->y = g->py + g->facing_y * (PLAYER_RADIUS + 4.0);
            b->vx = g->facing_x * BULLET_SPEED;
            b->vy = g->facing_y * BULLET_SPEED;
            b->life = BULLET_LIFE;
            b->damage = BULLET_DAMAGE;
            g->fired++;
            break;
        }
    }
}

static void update_bullets(double dt)
{
    Game* g = g_game;
    for (int i = 0; i < MAX_BULLETS; ++i) {
        Bullet* b = &g->bullets[i];
        if (!b->alive) continue;
        b->x += b->vx * dt;
        b->y += b->vy * dt;
        b->life -= dt;
        if (b->life <= 0 || b->x < -20 || b->y < -20 ||
            b->x > APP_W + 20 || b->y > APP_H + 20) {
            b->alive = false;
        }
    }
}

static void update_enemies(double dt)
{
    Game* g = g_game;
    for (int i = 0; i < MAX_ENEMIES; ++i) {
        Enemy* e = &g->enemies[i];
        if (!e->alive) continue;
        double dx = g->px - e->x;
        double dy = g->py - e->y;
        double len = sqrt(dx * dx + dy * dy);
        if (len > 0.001) {
            e->x += dx / len * e->speed * dt;
            e->y += dy / len * e->speed * dt;
        }
        double touch = PLAYER_RADIUS + ENEMY_RADIUS;
        if (len < touch) g->player_hp -= ENEMY_CONTACT_DPS * dt;
    }
}

static void resolve_bullet_hits(void)
{
    Game* g = g_game;
    for (int bi = 0; bi < MAX_BULLETS; ++bi) {
        Bullet* b = &g->bullets[bi];
        if (!b->alive) continue;
        for (int ei = 0; ei < MAX_ENEMIES; ++ei) {
            Enemy* e = &g->enemies[ei];
            if (!e->alive) continue;
            double dx = e->x - b->x;
            double dy = e->y - b->y;
            double rr = ENEMY_RADIUS + BULLET_RADIUS;
            if (dx * dx + dy * dy > rr * rr) continue;

            e->hp -= b->damage;
            b->alive = false;
            if (e->hp <= 0) {
                e->alive = false;
                g->enemy_count--;
                g->kills++;
            }
            break;
        }
    }
}

static void spawn_enemy(void)
{
    Game* g = g_game;
    if (g->enemy_count >= MAX_ENEMIES) return;

    double margin = ARENA_MARGIN + 22.0;
    double x = 0, y = 0;
    switch (g->rng.next_int(4)) {
    case 0:
        x = margin;
        y = g->rng.range(margin, APP_H - margin);
        break;
    case 1:
        x = APP_W - margin;
        y = g->rng.range(margin, APP_H - margin);
        break;
    case 2:
        x = g->rng.range(margin, APP_W - margin);
        y = margin;
        break;
    default:
        x = g->rng.range(margin, APP_W - margin);
        y = APP_H - margin;
        break;
    }

    for (int i = 0; i < MAX_ENEMIES; ++i) {
        Enemy* e = &g->enemies[i];
        if (e->alive) continue;
        e->alive = true;
        e->x = e->spawn_x = x;
        e->y = e->spawn_y = y;
        e->hp = 3.0;
        e->speed = 44.0 + g->rng.next_double() * 10.0;
        g->enemy_count++;
        break;
    }
}

static void update_spawn(double dt)
{
    Game* g = g_game;
    g->spawn_timer -= dt;
    if (g->spawn_timer > 0) return;

    double next = 1.3 - g->elapsed * 0.02;
    g->spawn_timer = next < 0.45 ? 0.45 : next;
    spawn_enemy();
}

static void update_play(double dt)
{
    Game* g = g_game;
    g->elapsed += dt;

    update_player(dt);
    update_bullets(dt);
    update_enemies(dt);
    resolve_bullet_hits();
    update_spawn(dt);

    if (g->player_hp <= 0 && !g->finished) {
        g->finished = true;
        enter_over(g->kills, g->elapsed);
    }
}

static void update_menu(double dt)
{
    Game* g = g_game;
    (void)dt;
    if (g->prev[SDL_SCANCODE_RETURN] == false && g->keys[SDL_SCANCODE_RETURN]) {
        enter_play();
        return;
    }
    if (g->prev[SDL_SCANCODE_SPACE] == false && g->keys[SDL_SCANCODE_SPACE]) {
        enter_play();
        return;
    }
    if (g->prev[SDL_SCANCODE_ESCAPE] == false && g->keys[SDL_SCANCODE_ESCAPE]) {
        g->quit = true;
    }
}

static void update_over(double dt)
{
    Game* g = g_game;
    (void)dt;
    bool ret = (!g->prev[SDL_SCANCODE_RETURN] && g->keys[SDL_SCANCODE_RETURN]) ||
               (!g->prev[SDL_SCANCODE_SPACE] && g->keys[SDL_SCANCODE_SPACE]);
    if (ret) {
        enter_play();
        return;
    }
    if (!g->prev[SDL_SCANCODE_ESCAPE] && g->keys[SDL_SCANCODE_ESCAPE]) {
        enter_menu();
    }
}

/* ---------------- 渲染 ---------------- */

static void render_menu(void)
{
    Game* g = g_game;
    float w = (float)APP_W, h = (float)APP_H;
    float cx = w * 0.5f;

    fill_rect(0, 0, w, h, 16, 18, 26);
    for (float x = 0; x <= w; x += 32.0f) draw_line(x, 0, x, h, 26, 30, 42);
    for (float y = 0; y <= h; y += 32.0f) draw_line(0, y, w, y, 26, 30, 42);

    draw_sprite(g->img_player, cx, h * 0.5f - 74.0f, 34.0f, 90, 200, 140);
    draw_text_center("JOY CPP TOPDOWN", cx, h * 0.5f - 26.0f, true, 120, 220, 255);
    draw_text_center(pick("C++ 直接调 joy 引擎", "C++ calls joy engine directly"),
                     cx, h * 0.5f + 4.0f, false, 150, 160, 180);

    if ((g->frame / 30) % 2 == 0) {
        draw_text_center(pick("Enter / Space 开始", "Enter / Space to start"),
                         cx, h * 0.5f + 44.0f, false, 255, 226, 140);
    }
    draw_text_center(pick("WASD 移动 · Space/鼠标左键 射击 · Esc 退出",
                          "WASD move | Space/LMB fire | Esc quit"),
                     cx, h * 0.5f + 76.0f, false, 130, 138, 158);
    draw_text_center(pick("场景：Menu", "Scene: Menu"), cx, 18.0f, false, 90, 96, 112);
}

static void render_play(void)
{
    Game* g = g_game;
    float w = (float)APP_W, h = (float)APP_H;

    fill_rect(0, 0, w, h, 16, 18, 26);

    for (float x = ARENA_MARGIN; x <= w - ARENA_MARGIN; x += 40.0f) {
        draw_line(x, ARENA_MARGIN, x, h - ARENA_MARGIN, 24, 28, 40);
    }
    for (float y = ARENA_MARGIN; y <= h - ARENA_MARGIN; y += 40.0f) {
        draw_line(ARENA_MARGIN, y, w - ARENA_MARGIN, y, 24, 28, 40);
    }
    stroke_rect(ARENA_MARGIN, ARENA_MARGIN,
                w - ARENA_MARGIN * 2.0f, h - ARENA_MARGIN * 2.0f, 58, 66, 92);

    for (int i = 0; i < MAX_BULLETS; ++i) {
        Bullet* b = &g->bullets[i];
        if (!b->alive) continue;
        fill_rect((float)b->x - BULLET_RADIUS, (float)b->y - BULLET_RADIUS,
                  BULLET_RADIUS * 2.0f, BULLET_RADIUS * 2.0f, 255, 224, 130);
    }

    for (int i = 0; i < MAX_ENEMIES; ++i) {
        Enemy* e = &g->enemies[i];
        if (!e->alive) continue;
        draw_sprite(g->img_enemy, (float)e->x, (float)e->y, 24.0f, 210, 96, 96);
        if (e->hp > 1.0) {
            fill_rect((float)e->x - 9.0f, (float)e->y - 17.0f,
                      18.0f * (float)(e->hp / 3.0), 2.0f, 230, 120, 110, 200);
        }
    }

    draw_sprite(g->img_player, (float)g->px, (float)g->py, 26.0f, 110, 210, 150);
    draw_line((float)g->px, (float)g->py,
              (float)(g->px + g->facing_x * 18.0), (float)(g->py + g->facing_y * 18.0),
              180, 240, 210);

    /* HUD */
    float bar_w = 120.0f;
    float hp_ratio = g->player_hp <= 0 ? 0.0f : (float)(g->player_hp / PLAYER_MAX_HP);
    fill_rect(12, 12, bar_w, 10, 40, 42, 54, 220);
    fill_rect(12, 12, bar_w * hp_ratio, 10, 96, 214, 140, 235);
    stroke_rect(12, 12, bar_w, 10, 90, 96, 118);

    char buf[128];
    snprintf(buf, sizeof(buf), "HP %d", (int)ceil(g->player_hp > 0 ? g->player_hp : 0));
    draw_text(buf, 12, 26);

    double secs = g->elapsed;
    int wave = 1 + g->kills / 8;
    snprintf(buf, sizeof(buf), pick("击杀 %d  ·  波次 %d  ·  存活 %.1fs",
                                    "KILLS %d | WAVE %d | TIME %.1fs"),
             g->kills, wave, secs);
    int sw = text_width(buf);
    draw_text(buf, w - 12.0f - (float)sw, 12.0f);

    snprintf(buf, sizeof(buf), pick("敌人 %d", "ENEMIES %d"), g->enemy_count);
    draw_text(buf, 12, h - 24.0f);

    const char* hint = pick("WASD 移动 · Space 射击 · Esc 菜单",
                            "WASD move | Space fire | Esc menu");
    int hw = text_width(hint);
    draw_text(hint, w - 12.0f - (float)hw, h - 24.0f, 120, 128, 148);
}

static void render_over(void)
{
    Game* g = g_game;
    float w = (float)APP_W, h = (float)APP_H;
    float cx = w * 0.5f, cy = h * 0.5f;

    fill_rect(0, 0, w, h, 14, 12, 18);
    fill_rect(cx - 150.0f, cy - 70.0f, 300.0f, 140.0f, 26, 22, 30, 235);
    stroke_rect(cx - 150.0f, cy - 70.0f, 300.0f, 140.0f, 90, 60, 70);

    draw_text_center("GAME OVER", cx, cy - 40.0f, true, 255, 120, 120);

    char buf[128];
    snprintf(buf, sizeof(buf), pick("击杀 %d · 存活 %.1fs", "Kills %d | Time %.1fs"),
             g->kills, g->elapsed);
    draw_text_center(buf, cx, cy - 6.0f, false, 230, 232, 240);
    draw_text_center(pick("Enter 重来 · Esc 回菜单", "Enter retry | Esc menu"),
                     cx, cy + 28.0f, false, 160, 168, 188);
    draw_text_center(pick("场景：GameOver", "Scene: GameOver"), cx, 18.0f, false, 90, 96, 112);
}

/* ---------------- 冒烟脚本（对应 C# SmokeScript） ---------------- */

static float abi_vec2_sum(vec2_t v) { return v.x + v.y; }
static float abi_rect_area(jrect_t r) { return r.w * r.h; }

static void smoke_before_tick(void)
{
    Game* g = g_game;
    int f = g->frame;

    bool enter = (f >= SMOKE_MENU_ENTER_A && f < SMOKE_MENU_ENTER_B) ||
                 (f >= SMOKE_OVER_ENTER_A && f < SMOKE_OVER_ENTER_B);
    g->scripted[SDL_SCANCODE_RETURN] = enter;

    g->scripted[SDL_SCANCODE_RIGHT] = (f >= SMOKE_MOVE_X_A && f < SMOKE_MOVE_X_B);
    g->scripted[SDL_SCANCODE_DOWN] = (f >= SMOKE_MOVE_Y_A && f < SMOKE_MOVE_Y_B);

    int rel = f - SMOKE_FIRE_A;
    g->scripted[SDL_SCANCODE_SPACE] = (f >= SMOKE_FIRE_A && rel % SMOKE_FIRE_PERIOD < SMOKE_FIRE_HOLD);

    if (f == SMOKE_KILL_FRAME && !g->killed_player && g->scene == SCENE_PLAY) {
        g->killed_player = true;
        g->player_hp = 0;
    }
}

static void smoke_observe(void)
{
    Game* g = g_game;
    if (g->scene == SCENE_PLAY) {
        if (!g->in_play) {
            g->in_play = true;
            g->play_count++;
            if (g->play_count == 1) {
                g->start_x = g->px;
                g->start_y = g->py;
            }
        }
        if (g->play_count == 1) {
            double dx = g->px - g->start_x;
            double dy = g->py - g->start_y;
            double moved = sqrt(dx * dx + dy * dy);
            if (moved > g->max_moved) g->max_moved = moved;
        }
        if (g->fired > g->max_fired) g->max_fired = g->fired;
        if (g->kills > g->max_kills) g->max_kills = g->kills;
        if (g->enemy_count > g->max_enemies) g->max_enemies = g->enemy_count;
        if (!g->enemy_moved) {
            for (int i = 0; i < MAX_ENEMIES; ++i) {
                if (g->enemies[i].alive && g->enemies[i].approached(g->px, g->py)) {
                    g->enemy_moved = true;
                    break;
                }
            }
        }
    } else {
        g->in_play = false;
        if (g->scene == SCENE_OVER) g->saw_over = true;
    }
}

static void smoke_after_tick(void)
{
    Game* g = g_game;

    if (!g->checked_abi) {
        g->checked_abi = true;
        vec2_t v = { 1.5f, 2.5f };
        jrect_t r = { 0, 0, 4, 5 };
        g->abi_ok = fabs(abi_vec2_sum(v) - 4.0f) < 0.001f &&
                    fabs(abi_rect_area(r) - 20.0f) < 0.001f;
    }

    smoke_observe();
    if (g->done || g->frame < SMOKE_ASSERT_FRAME) return;
    g->done = true;

    bool ok = true;
    ok = ok && g->abi_ok;
    ok = ok && g->play_count >= 2;
    ok = ok && g->saw_over;
    ok = ok && g->max_moved > 5.0;
    ok = ok && g->max_fired > 0;
    ok = ok && g->max_kills > 0;
    ok = ok && g->enemy_moved;

    printf("[smoke] %s api=n/a abi=%s play=%d gameover=%s moved=%.1f fired=%d kills=%d enemies=%d\n",
           ok ? "PASS" : "FAIL",
           g->abi_ok ? "True" : "False",
           g->play_count,
           g->saw_over ? "True" : "False",
           g->max_moved, g->max_fired, g->max_kills, g->max_enemies);
    fflush(stdout);

    g->exit_code = ok ? 0 : 1;
    g->quit = true;
}

/* ---------------- 输入快照 ---------------- */

static void input_begin_frame(void)
{
    Game* g = g_game;
    memcpy(g->prev, g->keys, sizeof(g->keys));
    for (int i = 0; i < 512; ++i) {
        g->keys[i] = g->scripted[i] || g->real_keys[i];
    }
}

static void input_end_frame(void)
{
    Game* g = g_game;
    memset(g->scripted, 0, sizeof(g->scripted));
}

/* ---------------- 游戏 DLL 对外契约实现 ---------------- */

static bool game_init(void* game, const JoyApi* api, int smoke_frames)
{
    Game* g = (Game*)game;
    g_api = api;
    g->rng.seed(12345u);
    g->smoke_frames = smoke_frames;

    g->window = api->CreateWindow(api->ctx, "Joy CPP Topdown", APP_W, APP_H);
    if (!g->window.h) {
        fprintf(stderr, "[host] create_window failed\n");
        return false;
    }
    g->r2d = api->R2dCreate(api->ctx, g->window);
    if (!g->r2d.h) {
        fprintf(stderr, "[host] r2d_create failed\n");
        return false;
    }
    api->R2dSetBlendMode(api->ctx, g->r2d, JR2D_BLEND_BLEND);

    load_assets();

    printf("[host] init %dx%d smoke=%d\n", APP_W, APP_H, g->smoke_frames);
    fflush(stdout);

    enter_menu();
    return true;
}

static void game_on_frame(void* game, double dt, const JoyInput* input,
                          JoyFrameResult* out)
{
    Game* g = (Game*)game;
    out->quit = false;
    out->exit_code = 0;
    out->frame = g->frame;

    /* 宿主把 SDL 事件翻成纯 C 快照；这里再与冒烟脚本的 scripted 按键合成 */
    memcpy(g->real_keys, input->keys, sizeof(g->real_keys));
    g->mouse_x = input->mouse_x;
    g->mouse_y = input->mouse_y;
    g->mouse_left = input->mouse_left;

    if (g->smoke_frames > 0) smoke_before_tick();

    g->frame++;
    input_begin_frame();

    switch (g->scene) {
    case SCENE_MENU: update_menu(dt); break;
    case SCENE_PLAY: update_play(dt); break;
    case SCENE_OVER: update_over(dt); break;
    }

    g_api->R2dBegin(g_api->ctx, g->r2d);
    switch (g->scene) {
    case SCENE_MENU: render_menu(); break;
    case SCENE_PLAY: render_play(); break;
    case SCENE_OVER: render_over(); break;
    }
    g_api->R2dEnd(g_api->ctx, g->r2d);

    input_end_frame();

    if (g->smoke_frames > 0) {
        smoke_after_tick();
        if (g->quit || g->frame >= g->smoke_frames) {
            out->quit = true;
            out->exit_code = g->exit_code;
        }
    } else if (g->quit) {
        out->quit = true;
        out->exit_code = g->exit_code;
    }
    out->frame = g->frame;
}

static void game_shutdown(void* game)
{
    Game* g = (Game*)game;
    if (!g || !g_api) return;

    if (g->img_player.h) g_api->ImageDestroy(g_api->ctx, g->img_player);
    if (g->img_enemy.h)  g_api->ImageDestroy(g_api->ctx, g->img_enemy);
    /* font 由 joy 内部缓存管理，这里不显式销毁（与 C# 侧一致：句柄随进程退出回收） */

    if (g->r2d.h)    g_api->R2dDestroy(g_api->ctx, g->r2d);
    if (g->window.h) g_api->DestroyWindow(g_api->ctx, g->window);
    g_api = NULL;
}

static JoyGameHooks s_hooks = {
    JOY_GAME_ABI,
    (uint32_t)sizeof(JoyGameHooks),
    NULL,
    game_init,
    game_on_frame,
    game_shutdown,
};

/* 唯一导出符号：宿主用 SDL_LoadFunction("joy_game_get_hooks") 取它 */
extern "C" const JoyGameHooks* joy_game_get_hooks(void)
{
    static Game s_game;       /* DLL 私有状态，生命周期随模块 */
    g_game = &s_game;
    s_hooks.game = g_game;
    return &s_hooks;
}
