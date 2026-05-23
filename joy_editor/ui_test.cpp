//// ui_test.cpp — UI 组件交互式测试
//// 编译: cmake -DBUILD_UI_TEST=ON ..
//// 展示并测试全部 jui 组件: Label, Button, Checkbox, RadioButton,
//// Combobox, Datagrid, Joystick
//#define SDL_MAIN_USE_CALLBACKS 1
//#include <cstdio>
//#include <cstdlib>
//#include <cstring>
//#include <cmath>
//#include <string>
//#include <SDL3/SDL.h>
//#include <SDL3/SDL_main.h>
//#include <joy/jui.h>
//#include <joy/jtext.h>
//
//// ======================== 常量 ========================
//static const int WINDOW_W = 1200, WINDOW_H = 800;
//static SDL_Window*   gWindow  = nullptr;
//static SDL_Renderer* gRenderer = nullptr;
//static font_p        gFont = nullptr;
//static font_p        gFontSmall = nullptr;
//
//// ======================== 缓存纹理(仅初始化时创建一次) ========================
//static text_texture_p gTxtButtons    = nullptr;
//static text_texture_p gTxtCheckboxes = nullptr;
//static text_texture_p gTxtRadio      = nullptr;
//static text_texture_p gTxtCombobox   = nullptr;
//static text_texture_p gTxtDatagrid   = nullptr;
//static text_texture_p gTxtJoystick   = nullptr;
//static text_texture_p gTxtHelp       = nullptr;
//
//// ======================== 状态 ========================
//static button_p   gBtn1          = nullptr;
//static button_p   gBtn2          = nullptr;
//static button_p   gBtn3          = nullptr;
//static checkbox_p gCb1           = nullptr;
//static checkbox_p gCb2           = nullptr;
//static checkbox_p gCb3           = nullptr;
//static radio_button_p gRadio1    = nullptr;
//static radio_button_p gRadio2    = nullptr;
//static radio_button_p gRadio3    = nullptr;
//static radio_button_group_p gRadioGroup = nullptr;
//static combobox_p gCombo         = nullptr;
//static datagrid_p gGrid          = nullptr;
//static joystick_p gJoystick      = nullptr;
//
//static int gBtn1ClickCount = 0;
//static int gBtn3ClickCount = 0;
//
//// ======================== 文本辅助 ========================
//static void draw_text(const char* text, float x, float y, SDL_Color color) {
//    if (!gFont) return;
//    text_texture_p tt = text_createx(gFont, text, (int)SDL_strlen(text), color);
//    if (tt) { text_print(gRenderer, tt, x, y); text_destroy(tt); }
//}
//
//static void draw_text_small(const char* text, float x, float y, SDL_Color color) {
//    if (!gFontSmall) return;
//    text_texture_p tt = text_createx(gFontSmall, text, (int)SDL_strlen(text), color);
//    if (tt) { text_print(gRenderer, tt, x, y); text_destroy(tt); }
//}
//
//// ======================== 回调 ========================
//static void on_btn1_press(button_p, void*) { gBtn1ClickCount++; }
//static void on_btn1_release(button_p, void*) {}
//static void on_btn3_press(button_p, void*) { gBtn3ClickCount++; }
//static void on_btn3_release(button_p, void*) {}
//
//// ======================== 初始化 ========================
//SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
//    (void)argc; (void)argv;
//    *appstate = nullptr;
//
//    if (!SDL_Init(SDL_INIT_VIDEO)) { SDL_Log("SDL_Init: %s", SDL_GetError()); return SDL_APP_FAILURE; }
//    if (!SDL_CreateWindowAndRenderer("UI Test", WINDOW_W, WINDOW_H, 0, &gWindow, &gRenderer)) {
//        SDL_Log("SDL_Window: %s", SDL_GetError()); return SDL_APP_FAILURE;
//    }
//
//    // 加载字体
//    {
//        auto base = SDL_GetBasePath();
//        std::string path = std::string(base ? base : "") + "joy2d_editor_fonts/arial.ttf";
//        gFont = font_create(gRenderer, path.c_str(), 20);
//        gFontSmall = font_create(gRenderer, path.c_str(), 14);
//    }
//
//    // ---------- 预创建静态文字纹理(避免每帧创建/销毁) ----------
//    SDL_Color white = { 220, 220, 220, 255 };
//    SDL_Color help  = { 100, 100, 120, 255 };
//    gTxtButtons    = text_createx(gFont, "=== Buttons ===", 14, white);
//    gTxtCheckboxes = text_createx(gFont, "=== Checkboxes ===", 17, white);
//    gTxtRadio      = text_createx(gFont, "=== Radio Buttons ===", 20, white);
//    gTxtCombobox   = text_createx(gFont, "=== Combobox ===", 15, white);
//    gTxtDatagrid   = text_createx(gFont, "=== Datagrid ===", 14, white);
//    gTxtJoystick   = text_createx(gFont, "=== Joystick ===", 14, white);
//    gTxtHelp       = text_createx(gFontSmall,
//        "Q/ESC: Exit | Click to interact with UI components", 53, help);
//
//    // ---------- Button ----------
//    gBtn1 = button_create(gRenderer, 140, 40);
//    gBtn1->position.x = 30; gBtn1->position.y = 40;
//    button_set_callback(gBtn1, on_btn1_press, on_btn1_release, nullptr);
//    {
//        SDL_Color c = { 255,255,255,255 };
//        const char* txt = "Click Me";
//        button_set_textx(gBtn1, gFont, txt, (int)SDL_strlen(txt), c);
//    }
//
//    gBtn2 = button_create(gRenderer, 140, 40);
//    gBtn2->position.x = 30; gBtn2->position.y = 100;
//    button_set_normal_color(gBtn2, { 180, 180, 80, 255 });
//    button_set_hover_color(gBtn2, { 220, 220, 120, 255 });
//    button_set_pressed_color(gBtn2, { 120, 120, 40, 255 });
//    {
//        SDL_Color c = { 255,255,255,255 };
//        const char* txt = "Styled Btn";
//        button_set_textx(gBtn2, gFont, txt, (int)SDL_strlen(txt), c);
//    }
//
//    gBtn3 = button_create(gRenderer, 140, 40);
//    gBtn3->position.x = 30; gBtn3->position.y = 160;
//    button_set_callback(gBtn3, on_btn3_press, on_btn3_release, nullptr);
//    button_set_normal_color(gBtn3, { 80, 180, 80, 255 });
//    {
//        SDL_Color c = { 255,255,255,255 };
//        const char* txt = "Count: 0";
//        button_set_textx(gBtn3, gFont, txt, (int)SDL_strlen(txt), c);
//    }
//
//    // ---------- Checkbox ----------
//    gCb1 = checkbox_create(30, 240, 20, "Option A");
//    gCb2 = checkbox_create(30, 280, 20, "Option B");
//    gCb3 = checkbox_create(30, 320, 20, "Disabled");
//    gCb3->enabled = false;
//
//    // ---------- Radio Button ----------
//    gRadioGroup = radio_button_group_create();
//    gRadio1 = radio_button_create(30, 380, 18, "Choice 1", 0);
//    gRadio2 = radio_button_create(30, 420, 18, "Choice 2", 0);
//    gRadio3 = radio_button_create(30, 460, 18, "Choice 3", 0);
//    radio_button_group_add_button(gRadioGroup, gRadio1);
//    radio_button_group_add_button(gRadioGroup, gRadio2);
//    radio_button_group_add_button(gRadioGroup, gRadio3);
//    radio_button_group_set_selected(gRadioGroup, 0);
//
//    // ---------- Combobox ----------
//    gCombo = combobox_create(gRenderer, { 30, 520, 160, 30 });
//    {
//        SDL_Color combo_col = { 0,0,0,255 };
//        combobox_add_item(gCombo, NULL, 0, NULL, 0, combo_col);
//        combobox_add_item(gCombo, NULL, 0, NULL, 0, combo_col);
//        combobox_add_item(gCombo, NULL, 0, NULL, 0, combo_col);
//    }
//
//    // ---------- Datagrid ----------
//    gGrid = datagrid_create(gRenderer, { 280, 40, 500, 200 }, 3, true, gFontSmall);
//    {
//        const char* hdrs[] = { "Name", "Age", "City", nullptr };
//        SDL_Color c = { 0,0,0,255 };
//        for (int i = 0; i < 3 && hdrs[i]; i++) {
//            if (gGrid->headers[i]) text_destroy(gGrid->headers[i]);
//            gGrid->headers[i] = text_createx(gFontSmall, hdrs[i], (int)SDL_strlen(hdrs[i]), c);
//        }
//    }
//    // 添加行 (C 级 API 直接操作 data)
//    {
//        auto add = [](datagrid_p g, const char* a, const char* b, const char* c) {
//            g->row_count++;
//            g->data = (int***)SDL_realloc(g->data, g->row_count * sizeof(int**));
//            g->data[g->row_count-1] = (int**)SDL_malloc(3 * sizeof(int*));
//            g->data[g->row_count-1][0] = (int*)SDL_strdup(a);
//            g->data[g->row_count-1][1] = (int*)SDL_strdup(b);
//            g->data[g->row_count-1][2] = (int*)SDL_strdup(c);
//        };
//        add(gGrid, "Alice",  "28", "NYC");
//        add(gGrid, "Bob",    "35", "LA");
//        add(gGrid, "Charlie","22", "SF");
//        add(gGrid, "Diana",  "31", "Chicago");
//        add(gGrid, "Eve",    "19", "Seattle");
//        add(gGrid, "Frank",  "42", "Boston");
//        add(gGrid, "Grace",  "27", "Austin");
//        add(gGrid, "Henry",  "33", "Denver");
//    }
//    gGrid->visible_rows = (gGrid->height - gGrid->row_height) / gGrid->row_height;
//
//    // ---------- Joystick ----------
//    gJoystick = joystick_create(gRenderer, 900, 600, 80);
//
//    SDL_Log("UI Test initialized");
//    return SDL_APP_CONTINUE;
//}
//
//// ======================== 事件 ========================
//SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
//    (void)appstate;
//
//    // 按钮事件(回调在函数内部触发)
//    button_handle_event(gBtn1, event);
//    button_handle_event(gBtn2, event);
//    button_handle_event(gBtn3, event);
//
//    // 复选框事件
//    checkbox_handle_event(gCb1, event, gRenderer);
//    checkbox_handle_event(gCb2, event, gRenderer);
//    if (gCb3->enabled) checkbox_handle_event(gCb3, event, gRenderer);
//
//    // 单选组事件
//    radio_button_group_handle_event(gRadioGroup, event, gRenderer);
//
//    // 下拉框事件
//    combobox_handle_event(gCombo, event);
//
//    // 摇杆事件
//    joystick_handle_event(gJoystick, event);
//
//    // 更新按钮3文本 (点击计数)
//    {
//        char buf[64];
//        SDL_snprintf(buf, sizeof(buf), "Count: %d", gBtn3ClickCount);
//        SDL_Color c = { 255,255,255,255 };
//        button_set_textx(gBtn3, gFont, buf, (int)SDL_strlen(buf), c);
//    }
//
//    if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
//    if (event->type == SDL_EVENT_KEY_DOWN) {
//        if (event->key.scancode == SDL_SCANCODE_ESCAPE ||
//            event->key.scancode == SDL_SCANCODE_Q)
//            return SDL_APP_SUCCESS;
//    }
//    return SDL_APP_CONTINUE;
//}
//
//// ======================== 渲染 ========================
//SDL_AppResult SDL_AppIterate(void* appstate) {
//    (void)appstate;
//    SDL_SetRenderDrawColor(gRenderer, 40, 40, 50, 255);
//    SDL_RenderClear(gRenderer);
//
//    // ===== 区域标题(使用缓存纹理,不每帧创建) =====
//    SDL_Color dim = { 160, 160, 160, 255 };
//    if (gTxtButtons)    text_print(gRenderer, gTxtButtons, 30, 10);
//    if (gTxtCheckboxes) text_print(gRenderer, gTxtCheckboxes, 30, 220);
//    if (gTxtRadio)      text_print(gRenderer, gTxtRadio, 30, 360);
//    if (gTxtCombobox)   text_print(gRenderer, gTxtCombobox, 30, 500);
//    if (gTxtDatagrid)   text_print(gRenderer, gTxtDatagrid, 280, 10);
//    if (gTxtJoystick)   text_print(gRenderer, gTxtJoystick, 860, 500);
//    if (gTxtHelp)       text_print(gRenderer, gTxtHelp, 30, 760);
//
//    char info[128];
//    SDL_snprintf(info, sizeof(info), "Btn1 clicks: %d", gBtn1ClickCount);
//    draw_text_small(info, 180, 50, dim);
//
//    // ===== 按钮 =====
//    button_draw(gBtn1);
//    button_draw(gBtn2);
//    button_draw(gBtn3);
//
//    // ===== 复选框 =====
//    checkbox_draw(gCb1, gRenderer, gFontSmall);
//    checkbox_draw(gCb2, gRenderer, gFontSmall);
//    checkbox_draw(gCb3, gRenderer, gFontSmall);
//
//    // ===== 单选按钮 =====
//    radio_button_group_draw(gRadioGroup, gRenderer);
//
//    // ===== 下拉框 =====
//    combobox_draw(gCombo);
//
//    // ===== 数据表格 =====
//    datagrid_draw(gGrid, gRenderer);
//
//    // ===== 摇杆 =====
//    joystick_draw(gJoystick);
//    {
//        SDL_FPoint dir = joystick_get_direction(gJoystick);
//        float mag = joystick_get_magnitude(gJoystick);
//        char buf[80];
//        SDL_snprintf(buf, sizeof(buf), "Dir: (%.2f, %.2f) Mag: %.2f", dir.x, dir.y, mag);
//        draw_text_small(buf, 860, 470, dim);
//    }
//
//    SDL_RenderPresent(gRenderer);
//    return SDL_APP_CONTINUE;
//}
//
//// ======================== 清理 ========================
//void SDL_AppQuit(void* appstate, SDL_AppResult result) {
//    (void)appstate;
//    if (gJoystick) joystick_destroy(gJoystick);
//    if (gGrid) datagrid_destroy(gGrid);
//    if (gCombo) combobox_destroy(gCombo);
//    if (gRadioGroup) radio_button_group_destroy(gRadioGroup);
//    if (gRadio1) radio_button_destroy(gRadio1);
//    if (gRadio2) radio_button_destroy(gRadio2);
//    if (gRadio3) radio_button_destroy(gRadio3);
//    if (gCb1) checkbox_destroy(gCb1);
//    if (gCb2) checkbox_destroy(gCb2);
//    if (gCb3) checkbox_destroy(gCb3);
//    if (gBtn1) button_destroy(gBtn1);
//    if (gBtn2) button_destroy(gBtn2);
//    if (gBtn3) button_destroy(gBtn3);
//    // 释放缓存纹理
//    auto del_tex = [](auto& t) { if (t) { text_destroy(t); t = nullptr; } };
//    del_tex(gTxtButtons); del_tex(gTxtCheckboxes); del_tex(gTxtRadio);
//    del_tex(gTxtCombobox); del_tex(gTxtDatagrid); del_tex(gTxtJoystick);
//    del_tex(gTxtHelp);
//    if (gFont) font_destroy(gFont);
//    if (gFontSmall) font_destroy(gFontSmall);
//    if (gRenderer) SDL_DestroyRenderer(gRenderer);
//    if (gWindow) SDL_DestroyWindow(gWindow);
//}
