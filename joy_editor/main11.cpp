//// main11.cpp — Tilemap + 玩家碰撞 + 四朝向攻击
//#include <cstdio>
//#include <cstdlib>
//#include <cmath>
//#include <string>
//#include <vector>
//#include "SDL3/SDL.h"
//#include <joy/jtilemap.h>
//#include <joy/jtilemap_render.h>
//#include <joy/janimations.h>
//#include <joy/jcore.h>
//
//static const int WINDOW_W = 1024, WINDOW_H = 760;
//static SDL_Window*   gWindow  = nullptr;
//static SDL_Renderer* gRenderer = nullptr;
//static jtilemap_p    gTilemap = nullptr;
//static bool          gRunning = true;
//static game_timer_t         gTimer;
//static simple_fps_counter_p gFps = nullptr;
//
//// ======================== 玩家 ========================
//struct Player { float x,y, vx,vy, w,h, speed, vis_w,vis_h; };
//static Player gPlayer = { 300, 300, 0,0, 36,52, 200.0f, 192,192 };
//
//static const char* SPRITE = "joy2d_editor_textures/knights/troops/warrior/warrior_blue.png";
//// warrio_blue.tsx: 行0=idle, 1=run, 2=attack_right, 4=attack_bottom, 6=attack_top
//static sprite_animation_p gAnimIdle, gAnimWalk;
//static sprite_animation_p gAnimAtk[4]; // 0=下 1=左 2=右 3=上
//static int    gPlayerDir   = 0;         // 0=下 1=左 2=右 3=上
//static bool   gAttacking   = false;
//static float  gAtkTimer    = 0;
//static float  gAtkDuration = 0.6f;     // 4帧×0.15s
//
//// ======================== 碰撞墙体 ========================
//struct Wall { float x,y,w,h; };
//static std::vector<Wall> gWalls;
//static void cache_walls(jtilemap_p tm) {
//    auto* ol = jtilemap_find_object_layer(tm, "collision");
//    if (!ol) return;
//    for (size_t i = 0; i < kv_size(ol->objects); i++) {
//        auto* obj = &kv_A(ol->objects, i);
//        if (obj->shape == JTILEMAP_SHAPE_RECT && obj->visible)
//            gWalls.push_back({ obj->x, obj->y, obj->width, obj->height });
//    }
//    log_info("cached %zu walls", gWalls.size());
//}
//static bool overlap(float ax,float ay,float aw,float ah, float bx,float by,float bw,float bh) {
//    return ax < bx+bw && ax+aw > bx && ay < by+bh && ay+ah > by;
//}
//static void resolve_collision(float& px, float& py) {
//    for (auto& w : gWalls) {
//        if (!overlap(px, py, gPlayer.w, gPlayer.h, w.x,w.y,w.w,w.h)) continue;
//        float l = (px+gPlayer.w)-w.x, r = (w.x+w.w)-px;
//        float t = (py+gPlayer.h)-w.y, b = (w.y+w.h)-py;
//        float mx = l<r?l:r, my = t<b?t:b;
//        if (mx < my) px += (l<r) ? -l : r;
//        else         py += (t<b) ? -t : b;
//    }
//}
//
//// ======================== 初始化动画 ========================
//static sprite_animation_p make_anim(int row, int nframes, float sx) {
//    auto a = sprite_animation_create(gRenderer);
//    for (int c = 0; c < nframes; c++)
//        sprite_animation_add_clip(a, SPRITE, 0.15f, { (float)(c*192), (float)(row*192), 192,192 });
//    sprite_animation_set_scale(a, sx, 1.0f);
//    return a;
//}
//
//static void init_player_anim() {
//    // idle:行0 run:行1（4方向共用，左右翻转）
//    gAnimIdle = make_anim(0, 6, 1);
//    gAnimWalk = make_anim(1, 6, 1);
//
//    // 攻击: 右=行2(attack_right) 下=行4(attack_bottom) 上=行6(attack_top)
//    // 左=行2 水平翻转
//    gAnimAtk[0] = make_anim(4, 6,  1);  // 下
//    gAnimAtk[1] = make_anim(2, 6, -1);  // 左（翻转）
//    gAnimAtk[2] = make_anim(2, 6,  1);  // 右
//    gAnimAtk[3] = make_anim(6, 6,  1);  // 上
//}
//
//// ======================== 回调 ========================
//static bool app_init(void) {
//    if (!SDL_Init(SDL_INIT_VIDEO)) { log_error("SDL_Init: %s", SDL_GetError()); return false; }
//    if (!SDL_CreateWindowAndRenderer("tilemap", WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE, &gWindow, &gRenderer)) {
//        log_error("SDL_Window: %s", SDL_GetError()); SDL_Quit(); return false;
//    }
//    { auto base = SDL_GetBasePath();
//      std::string path = std::string(base ? base : "") + "joy2d_editor_tilemap/map.lua";
//      gTilemap = jtilemap_load_file(path.c_str());
//      if (!gTilemap) { log_error("load map failed"); return false; }
//      log_info("map %dx%d", gTilemap->width, gTilemap->height); }
//
//    jtilemap_render_init(gRenderer, NULL, NULL);
//    cache_walls(gTilemap);
//    init_player_anim();
//    game_timer_init(&gTimer); game_timer_set_target_fps(&gTimer, 60);
//    gFps = simple_fps_create();
//    return true;
//}
//
//static bool app_event(const SDL_Event* e) {
//    if (e->type == SDL_EVENT_QUIT) return false;
//    if (e->type == SDL_EVENT_KEY_DOWN) {
//        auto sc = e->key.scancode;
//        if (sc == SDL_SCANCODE_ESCAPE || sc == SDL_SCANCODE_Q) return false;
//
//        // 攻击：不可移动时也能攻击
//        if (sc == SDL_SCANCODE_SPACE || sc == SDL_SCANCODE_J) {
//            gAttacking = true; gAtkTimer = 0; return true;
//        }
//
//        if (gAttacking) return true; // 攻击中禁止移动
//        if (sc == SDL_SCANCODE_W || sc == SDL_SCANCODE_UP)       { gPlayer.vy = -1; gPlayerDir = 3; }
//        else if (sc == SDL_SCANCODE_S || sc == SDL_SCANCODE_DOWN) { gPlayer.vy = 1;  gPlayerDir = 0; }
//        else if (sc == SDL_SCANCODE_A || sc == SDL_SCANCODE_LEFT) { gPlayer.vx = -1; gPlayerDir = 1; }
//        else if (sc == SDL_SCANCODE_D || sc == SDL_SCANCODE_RIGHT){ gPlayer.vx = 1;  gPlayerDir = 2; }
//    }
//    if (!gAttacking && e->type == SDL_EVENT_KEY_UP) {
//        auto sc = e->key.scancode;
//        if (sc == SDL_SCANCODE_W || sc == SDL_SCANCODE_UP)       gPlayer.vy = 0;
//        else if (sc == SDL_SCANCODE_S || sc == SDL_SCANCODE_DOWN)  gPlayer.vy = 0;
//        else if (sc == SDL_SCANCODE_A || sc == SDL_SCANCODE_LEFT)  gPlayer.vx = 0;
//        else if (sc == SDL_SCANCODE_D || sc == SDL_SCANCODE_RIGHT) gPlayer.vx = 0;
//    }
//    return true;
//}
//
//static bool app_iterate(void) {
//    game_timer_update(&gTimer); simple_fps_update(gFps);
//    float dt = game_timer_get_unscaled_delta_time(&gTimer);
//
//    // ===== 攻击计时 =====
//    if (gAttacking) {
//        gAtkTimer += dt;
//        if (gAtkTimer >= gAtkDuration) gAttacking = false;
//    }
//
//    // ===== 移动（攻击中禁止） =====
//    if (!gAttacking) {
//        float nx = gPlayer.x + gPlayer.vx * gPlayer.speed * dt;
//        float ny = gPlayer.y + gPlayer.vy * gPlayer.speed * dt;
//        resolve_collision(nx, ny);
//        gPlayer.x = nx; gPlayer.y = ny;
//    }
//
//    // ===== 摄像机 =====
//    float cam_x = gPlayer.x + gPlayer.vis_w/2 - WINDOW_W/2;
//    float cam_y = gPlayer.y + gPlayer.vis_h/2 - WINDOW_H/2;
//
//    // ===== 选择动画 =====
//    sprite_animation_p cur;
//    float sx = 1;
//    if (gAttacking) {
//        cur = gAnimAtk[gPlayerDir];
//        sx = (gPlayerDir == 1) ? -1.0f : 1.0f; // 左方向翻转
//    } else {
//        float len2 = gPlayer.vx*gPlayer.vx + gPlayer.vy*gPlayer.vy;
//        cur = (len2 > 0) ? gAnimWalk : gAnimIdle;
//        sx = (gPlayerDir == 1) ? -1.0f : 1.0f;
//    }
//    sprite_animation_set_scale(cur, sx, 1.0f);
//    sprite_animation_update(cur, dt);
//
//    float screen_x = gPlayer.x - cam_x - (gPlayer.vis_w - gPlayer.w)/2;
//    float screen_y = gPlayer.y - cam_y - (gPlayer.vis_h - gPlayer.h)/2;
//    sprite_animation_set_position(cur, screen_x, screen_y);
//
//    // ===== 渲染 =====
//    SDL_SetRenderDrawColor(gRenderer, 71,171,169,255);
//    SDL_RenderClear(gRenderer);
//    jtilemap_render_all(gTilemap, cam_x, cam_y, SDL_GetTicks());
//    sprite_animation_draw(cur, nullptr);
//
//    { char t[128]; snprintf(t,sizeof(t), "tilemap - FPS:%d", simple_fps_value(gFps));
//      SDL_SetWindowTitle(gWindow, t); }
//    SDL_RenderPresent(gRenderer);
//    return true;
//}
//
//static void app_quit(void) {
//    auto d = [](auto p) { if(p) sprite_animation_destroy(p); };
//    d(gAnimIdle); d(gAnimWalk);
//    for (int i=0;i<4;i++) d(gAnimAtk[i]);
//    jtilemap_render_destroy();
//    if (gTilemap) jtilemap_destroy(gTilemap);
//    if (gFps) simple_fps_destory(gFps);
//    if (gRenderer) SDL_DestroyRenderer(gRenderer);
//    if (gWindow) SDL_DestroyWindow(gWindow);
//    SDL_Quit();
//}
//
//int main(int argc, char* argv[]) {
//    (void)argc; (void)argv;
//    if (!app_init()) return -1;
//    SDL_Event e;
//    while (gRunning) {
//        while (SDL_PollEvent(&e)) { if (!app_event(&e)) { gRunning = false; break; } }
//        if (gRunning) app_iterate();
//    }
//    app_quit();
//    return 0;
//}
