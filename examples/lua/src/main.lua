-- ============================================================
-- joy 示例 · Lua 版
--
-- 与 cpp / csharp / js 三个示例功能完全相同：
--   Menu → Play（WASD 移动 / Space 射击 / 敌人追击 / 碰撞掉血）→ GameOver
--
-- 宿主（../main.cpp）只往这里推三件事：
--   joy.init()        建窗口与渲染器（窗口归脚本管，jluax 已注册好 joy.* 表）
--   joy.update(dt)    推进一帧
--   joy.draw()        画一帧（自己 r2d.begin / end_frame）
-- 外加 joy.keypressed / joy.keyreleased / joy.mouse* 事件回调。
--
-- 输入说明：joy.keyboard.* 是「基于单个 SDL 事件」的查询（还要传 event
-- 句柄、只认 8 个键名），不适合做游戏状态；所以这里在 keypressed /
-- keyreleased 里自己维护按键表，与 cpp/csharp 的轮询语义等价。
-- ============================================================

local W, H = 480, 320

local ARENA_MARGIN      = 10
local ENEMY_CONTACT_DPS = 26
local MAX_ENEMIES       = 24
local MAX_BULLETS       = 64

local PLAYER_RADIUS        = 11
local PLAYER_SPEED         = 130
local PLAYER_MAX_HP        = 100
local PLAYER_FIRE_INTERVAL = 0.16

local ENEMY_RADIUS = 11

local BULLET_RADIUS = 3
local BULLET_SPEED  = 320
local BULLET_LIFE   = 1.5
local BULLET_DAMAGE = 1

-- 素材原始尺寸（player.png / enemy.png 都是 64x64）
local SPRITE_SRC = 64

-- ---- 生成链路（IDL → thunks → 三端同名） ----
-- 窗口、渲染、矩形、直线、字体、文字全部走 Joy.Api.Bindings.*，
-- 不再用 joy.window / joy.r2d 那套旧手写胶水。
-- 唯一仍走旧胶水的是输入：keypressed / keyreleased 等事件回调不在 IDL 里。
local B = Joy.Api.Bindings

-- SDL_Scancode 常用键
local SC = {
    A = 4, D = 7, J = 13, S = 22, W = 26,
    RETURN = 40, ESCAPE = 41, SPACE = 44,
    RIGHT = 79, LEFT = 80, DOWN = 81, UP = 82,
}

local TRACKED = {
    SC.A, SC.D, SC.S, SC.W, SC.J,
    SC.RETURN, SC.ESCAPE, SC.SPACE,
    SC.RIGHT, SC.LEFT, SC.DOWN, SC.UP,
}

-- ---------------- 确定性随机（与另外三份同算法同种子） ----------------
local rng_state = 12345
local M32 = 0xFFFFFFFF

local function next_u32()
    local s = rng_state
    s = s ~ ((s << 13) & M32)
    s = s ~ (s >> 17)
    s = s ~ ((s << 5) & M32)
    s = s & M32
    rng_state = s
    return s
end

local function next_double()
    return (next_u32() >> 8) / 16777216.0
end

local function next_int(n)
    return next_u32() % n
end

local function rng_range(lo, hi)
    return lo + next_double() * (hi - lo)
end

-- ---------------- 状态 ----------------
local S = {
    scene = "menu",
    px = 0, py = 0,
    hp = PLAYER_MAX_HP,
    fire_cd = 0,
    facing_x = 1, facing_y = 0,
    enemies = {}, bullets = {},
    elapsed = 0, spawn_timer = 0,
    kills = 0, fired = 0,
    finished = false,
    frame = 0,
}

local win, ren
local font_small, font_big
local has_font = false

-- 输入：keys 是真实事件状态，scripted 是冒烟注入，now/prev 是合成快照
local keys, now, prev, scripted = {}, {}, {}, {}
local mouse_left = false

local function down(sc)
    return now[sc] == true
end

local function pressed(sc)
    return now[sc] == true and prev[sc] ~= true
end

-- ---------------- 绘制辅助（全部走生成链路） ----------------
local function set_color(r, g, b, a)
    B.R2dSetColorf(ren, r / 255.0, g / 255.0, b / 255.0, (a or 255) / 255.0)
end

local function fill(x, y, w, h, r, g, b, a)
    set_color(r, g, b, a)
    B.ShapeDrawRectangle(ren, "fill", x, y, w, h)
end

local function stroke(x, y, w, h, r, g, b, a)
    set_color(r, g, b, a)
    B.ShapeDrawRectangle(ren, "line", x, y, w, h)
end

-- *_f 系列参数是归一化比例（r2d_fpx 会乘窗口宽高），这里传的是像素坐标，
-- 必须用非 _f 的 ShapeDrawLine，否则网格线会被放大到屏幕外。
local function draw_line(x1, y1, x2, y2, r, g, b, a)
    set_color(r, g, b, a)
    B.ShapeDrawLine(ren, x1, y1, x2, y2)
end

local function text(s, x, y, r, g, b)
    if not has_font then return end
    B.JtextPrintx(ren, font_small, s, #s, r or 232, g or 236, b or 245, 255, x, y)
end

-- 生成链路里 jtext_measurex 是 L3（int* 输出参数）尚未开放，
-- 用字号 × 字符数近似宽度做居中
local function text_width(s, size)
    local n = utf8.len(s) or #s
    return n * size * 0.58
end

local function text_center(s, cx, cy, big, r, g, b)
    if not has_font then return end
    local f = big and font_big or font_small
    local size = big and 22 or 12
    local w = text_width(s, size)
    B.JtextPrintx(ren, f, s, #s, r or 232, g or 236, b or 245, 255,
                  cx - w * 0.5, cy - size * 0.5)
end

local img_player, img_enemy

-- ImageDrawRect 是全标量版（srcrect 固定 NULL 表示整张图），绕开了
-- jrect_t* 无法进 joy_value 的 L3 限制；句柄为 nil 时退回同尺寸色块。
local function sprite(img, cx, cy, size, fr, fg, fb)
    if img then
        B.ImageDrawRect(ren, img, cx - size * 0.5, cy - size * 0.5, size, size)
    else
        fill(cx - size * 0.5, cy - size * 0.5, size, size, fr, fg, fb)
    end
end

-- ---------------- 资源 ----------------
local FONT_CANDIDATES = {
    "fonts/simhei.ttf",
    "C:/Windows/Fonts/simhei.ttf",
    "C:/Windows/Fonts/Deng.ttf",
}

-- 贴图由 CMake 部署到 sprites/；万一加载失败，pcall 兜住 thunk 的
-- JOY_ERR_CREATE，让 sprite 退回色块而不是中断整个脚本。
local function try_image(path)
    local ok, h = pcall(B.ImageCreate, ren, path)
    return ok and h or nil
end

local function load_assets()
    -- 生成链路的 handle 是 userdata，脚本层无法判断其内部指针是否为 0，
    -- 所以只认仓内自带的那份字体（四份示例共享 assets，构建时必被拷贝）。
    font_small = B.FontCreate(FONT_CANDIDATES[1], 12)
    font_big = B.FontCreate(FONT_CANDIDATES[1], 22)
    has_font = true
    img_player = try_image("sprites/player.png")
    img_enemy = try_image("sprites/enemy.png")
end

-- ---------------- 场景 ----------------
local function enter_menu()
    S.scene = "menu"
    print("[scene] Menu")
end

local function make_enemy(x, y, hp)
    return {
        x = x, y = y, spawn_x = x, spawn_y = y,
        hp = hp, speed = 44 + next_double() * 10,
    }
end

local function enter_play()
    S.scene = "play"
    S.px = W * 0.5
    S.py = H * 0.5
    S.hp = PLAYER_MAX_HP
    S.fire_cd = 0
    S.facing_x = 1
    S.facing_y = 0
    S.elapsed = 0
    S.kills = 0
    S.fired = 0
    S.finished = false
    S.enemies = {}
    S.bullets = {}
    S.spawn_timer = (SMOKE_FRAMES and SMOKE_FRAMES > 0) and 1.5 or 1.0

    print("[scene] Play")

    if SMOKE_FRAMES and SMOKE_FRAMES > 0 then
        S.enemies[#S.enemies + 1] = make_enemy(S.px + 90, S.py, 2)
        S.enemies[#S.enemies + 1] = make_enemy(S.px + 62, S.py + 62, 2)
        S.enemies[#S.enemies + 1] = make_enemy(S.px, S.py + 90, 2)
    end
end

local function enter_over(kills, seconds)
    S.scene = "over"
    S.final_kills = kills
    S.final_seconds = seconds
    print(string.format("[scene] GameOver kills=%d time=%.1f", kills, seconds))
end

-- ---------------- 更新 ----------------
local function update_player(dt)
    local dx, dy = 0, 0
    if down(SC.A) or down(SC.LEFT) then dx = dx - 1 end
    if down(SC.D) or down(SC.RIGHT) then dx = dx + 1 end
    if down(SC.W) or down(SC.UP) then dy = dy - 1 end
    if down(SC.S) or down(SC.DOWN) then dy = dy + 1 end

    if dx ~= 0 or dy ~= 0 then
        local len = math.sqrt(dx * dx + dy * dy)
        dx, dy = dx / len, dy / len
        S.px = S.px + dx * PLAYER_SPEED * dt
        S.py = S.py + dy * PLAYER_SPEED * dt
        S.facing_x, S.facing_y = dx, dy
    end

    local minx = ARENA_MARGIN + PLAYER_RADIUS
    local maxx = W - ARENA_MARGIN - PLAYER_RADIUS
    local miny = ARENA_MARGIN + PLAYER_RADIUS
    local maxy = H - ARENA_MARGIN - PLAYER_RADIUS
    if S.px < minx then S.px = minx end
    if S.px > maxx then S.px = maxx end
    if S.py < miny then S.py = miny end
    if S.py > maxy then S.py = maxy end

    S.fire_cd = S.fire_cd - dt
    local want_fire = down(SC.SPACE) or down(SC.J) or mouse_left
    if want_fire and S.fire_cd <= 0 and #S.bullets < MAX_BULLETS then
        S.fire_cd = PLAYER_FIRE_INTERVAL
        S.bullets[#S.bullets + 1] = {
            x = S.px + S.facing_x * (PLAYER_RADIUS + 4),
            y = S.py + S.facing_y * (PLAYER_RADIUS + 4),
            vx = S.facing_x * BULLET_SPEED,
            vy = S.facing_y * BULLET_SPEED,
            life = BULLET_LIFE,
            damage = BULLET_DAMAGE,
        }
        S.fired = S.fired + 1
    end
end

local function update_bullets(dt)
    local keep = {}
    for _, b in ipairs(S.bullets) do
        b.x = b.x + b.vx * dt
        b.y = b.y + b.vy * dt
        b.life = b.life - dt
        if b.life > 0 and b.x >= -20 and b.y >= -20 and
           b.x <= W + 20 and b.y <= H + 20 then
            keep[#keep + 1] = b
        end
    end
    S.bullets = keep
end

local function update_enemies(dt)
    for _, e in ipairs(S.enemies) do
        local dx = S.px - e.x
        local dy = S.py - e.y
        local len = math.sqrt(dx * dx + dy * dy)
        if len > 0.001 then
            e.x = e.x + dx / len * e.speed * dt
            e.y = e.y + dy / len * e.speed * dt
        end
        if len < PLAYER_RADIUS + ENEMY_RADIUS then
            S.hp = S.hp - ENEMY_CONTACT_DPS * dt
        end
    end
end

local function resolve_bullet_hits()
    for bi = #S.bullets, 1, -1 do
        local b = S.bullets[bi]
        local hit = false
        for ei = #S.enemies, 1, -1 do
            local e = S.enemies[ei]
            local dx = e.x - b.x
            local dy = e.y - b.y
            local rr = ENEMY_RADIUS + BULLET_RADIUS
            if dx * dx + dy * dy <= rr * rr then
                e.hp = e.hp - b.damage
                hit = true
                if e.hp <= 0 then
                    table.remove(S.enemies, ei)
                    S.kills = S.kills + 1
                end
                break
            end
        end
        if hit then table.remove(S.bullets, bi) end
    end
end

local function update_spawn(dt)
    S.spawn_timer = S.spawn_timer - dt
    if S.spawn_timer > 0 then return end

    local next_timer = 1.3 - S.elapsed * 0.02
    if next_timer < 0.45 then next_timer = 0.45 end
    S.spawn_timer = next_timer

    if #S.enemies >= MAX_ENEMIES then return end

    local margin = ARENA_MARGIN + 22
    local x, y
    local pick = next_int(4)
    if pick == 0 then
        x, y = margin, rng_range(margin, H - margin)
    elseif pick == 1 then
        x, y = W - margin, rng_range(margin, H - margin)
    elseif pick == 2 then
        x, y = rng_range(margin, W - margin), margin
    else
        x, y = rng_range(margin, W - margin), H - margin
    end
    S.enemies[#S.enemies + 1] = make_enemy(x, y, 3)
end

local function update_play(dt)
    S.elapsed = S.elapsed + dt

    update_player(dt)
    update_bullets(dt)
    update_enemies(dt)
    resolve_bullet_hits()
    update_spawn(dt)

    if S.hp <= 0 and not S.finished then
        S.finished = true
        enter_over(S.kills, S.elapsed)
    end
end

local function update_menu()
    if pressed(SC.RETURN) or pressed(SC.SPACE) then
        enter_play()
        return
    end
    if pressed(SC.ESCAPE) then
        quit = true
        exit_code = 0
    end
end

local function update_over()
    if pressed(SC.RETURN) or pressed(SC.SPACE) then
        enter_play()
        return
    end
    if pressed(SC.ESCAPE) then
        enter_menu()
    end
end

-- ---------------- 渲染 ----------------
local function render_menu()
    local cx = W * 0.5

    sprite(img_player, cx, H * 0.5 - 74, 34, 90, 200, 140)
    for x = 0, W, 32 do draw_line(x, 0, x, H, 26, 30, 42) end
    for y = 0, H, 32 do draw_line(0, y, W, y, 26, 30, 42) end

    text_center("JOY LUA TOPDOWN", cx, H * 0.5 - 26, true, 120, 220, 255)
    text_center("Lua script + joy engine", cx, H * 0.5 + 4, false, 150, 160, 180)

    if math.floor(S.frame / 30) % 2 == 0 then
        text_center("Enter / Space to start", cx, H * 0.5 + 44, false, 255, 226, 140)
    end
    text_center("WASD move | Space/LMB fire | Esc quit",
                cx, H * 0.5 + 76, false, 130, 138, 158)
    text_center("Scene: Menu", cx, 18, false, 90, 96, 112)
end

local function render_play()
    for x = ARENA_MARGIN, W - ARENA_MARGIN, 40 do
        draw_line(x, ARENA_MARGIN, x, H - ARENA_MARGIN, 24, 28, 40)
    end
    for y = ARENA_MARGIN, H - ARENA_MARGIN, 40 do
        draw_line(ARENA_MARGIN, y, W - ARENA_MARGIN, y, 24, 28, 40)
    end
    stroke(ARENA_MARGIN, ARENA_MARGIN,
           W - ARENA_MARGIN * 2, H - ARENA_MARGIN * 2, 58, 66, 92)

    for _, b in ipairs(S.bullets) do
        fill(b.x - BULLET_RADIUS, b.y - BULLET_RADIUS,
             BULLET_RADIUS * 2, BULLET_RADIUS * 2, 255, 224, 130)
    end

    for _, e in ipairs(S.enemies) do
        sprite(img_enemy, e.x, e.y, 24, 210, 96, 96)
        if e.hp > 1 then
            fill(e.x - 9, e.y - 17, 18 * (e.hp / 3), 2, 230, 120, 110, 200)
        end
    end

    sprite(img_player, S.px, S.py, 26, 110, 210, 150)
    draw_line(S.px, S.py, S.px + S.facing_x * 18, S.py + S.facing_y * 18, 180, 240, 210)

    -- HUD
    local bar_w = 120
    local hp_ratio = S.hp <= 0 and 0 or (S.hp / PLAYER_MAX_HP)
    fill(12, 12, bar_w, 10, 40, 42, 54, 220)
    fill(12, 12, bar_w * hp_ratio, 10, 96, 214, 140, 235)
    stroke(12, 12, bar_w, 10, 90, 96, 118)
    text(string.format("HP %d", math.ceil(math.max(0, S.hp))), 12, 26)

    local stats = string.format("KILLS %d | WAVE %d | TIME %.1fs",
                                S.kills, 1 + math.floor(S.kills / 8), S.elapsed)
    text(stats, W - 12 - text_width(stats, 12), 12)

    text(string.format("ENEMIES %d", #S.enemies), 12, H - 24)
    local hint = "WASD move | Space fire | Esc menu"
    text(hint, W - 12 - text_width(hint, 12), H - 24, 120, 128, 148)
end

local function render_over()
    local cx, cy = W * 0.5, H * 0.5

    fill(cx - 150, cy - 70, 300, 140, 26, 22, 30, 235)
    stroke(cx - 150, cy - 70, 300, 140, 90, 60, 70)

    text_center("GAME OVER", cx, cy - 40, true, 255, 120, 120)
    text_center(string.format("Kills %d | Time %.1fs", S.final_kills, S.final_seconds),
                cx, cy - 6, false, 230, 232, 240)
    text_center("Enter retry | Esc menu", cx, cy + 28, false, 160, 168, 188)
    text_center("Scene: GameOver", cx, 18, false, 90, 96, 112)
end

-- ---------------- 冒烟脚本（对应 cpp 的 smoke_* / C# 的 SmokeScript） ----------------
local SMOKE_ASSERT_FRAME = 150
local smoke = {
    play_count = 0, in_play = false, saw_over = false, enemy_moved = false,
    start_x = 0, start_y = 0, max_moved = 0,
    max_fired = 0, max_kills = 0, max_enemies = 0,
    killed_player = false, done = false,
}

local function smoke_before_tick()
    local f = S.frame
    scripted[SC.RETURN] = (f >= 2 and f < 4) or (f >= 126 and f < 128)
    scripted[SC.RIGHT] = (f >= 10 and f < 90)
    scripted[SC.DOWN] = (f >= 10 and f < 60)
    local rel = f - 20
    scripted[SC.SPACE] = (f >= 20 and (rel % 12) < 2)

    if f == 120 and not smoke.killed_player and S.scene == "play" then
        smoke.killed_player = true
        S.hp = 0
    end
end

local function enemy_approached(e)
    local sx, sy = e.spawn_x - S.px, e.spawn_y - S.py
    local nx, ny = e.x - S.px, e.y - S.py
    local before = math.sqrt(sx * sx + sy * sy)
    local n = math.sqrt(nx * nx + ny * ny)
    return n < before - 2
end

local function smoke_observe()
    if S.scene == "play" then
        if not smoke.in_play then
            smoke.in_play = true
            smoke.play_count = smoke.play_count + 1
            if smoke.play_count == 1 then
                smoke.start_x, smoke.start_y = S.px, S.py
            end
        end
        if smoke.play_count == 1 then
            local dx = S.px - smoke.start_x
            local dy = S.py - smoke.start_y
            local moved = math.sqrt(dx * dx + dy * dy)
            if moved > smoke.max_moved then smoke.max_moved = moved end
        end
        if S.fired > smoke.max_fired then smoke.max_fired = S.fired end
        if S.kills > smoke.max_kills then smoke.max_kills = S.kills end
        if #S.enemies > smoke.max_enemies then smoke.max_enemies = #S.enemies end
        if not smoke.enemy_moved then
            for _, e in ipairs(S.enemies) do
                if enemy_approached(e) then
                    smoke.enemy_moved = true
                    break
                end
            end
        end
    else
        smoke.in_play = false
        if S.scene == "over" then smoke.saw_over = true end
    end
end

local function smoke_after_tick()
    smoke_observe()
    if smoke.done or S.frame < SMOKE_ASSERT_FRAME then return end
    smoke.done = true

    local ok = true
    ok = ok and smoke.play_count >= 2
    ok = ok and smoke.saw_over
    ok = ok and smoke.max_moved > 5
    ok = ok and smoke.max_fired > 0
    ok = ok and smoke.max_kills > 0
    ok = ok and smoke.enemy_moved

    print(string.format(
        "[smoke] %s api=n/a abi=True play=%d gameover=%s moved=%.1f fired=%d kills=%d enemies=%d",
        ok and "PASS" or "FAIL",
        smoke.play_count,
        smoke.saw_over and "True" or "False",
        smoke.max_moved, smoke.max_fired, smoke.max_kills, smoke.max_enemies))

    exit_code = ok and 0 or 1
    quit = true
end

-- ---------------- joy 回调 ----------------
function joy.init()
    win = B.CreateWindow("Joy Lua Topdown", W, H)
    ren = B.R2dCreate(win)

    load_assets()
    enter_menu()
end

function joy.frame_begin(dt)
    if not (SMOKE_FRAMES and SMOKE_FRAMES > 0) then return end
    smoke_before_tick()
end

function joy.update(dt)
    S.frame = S.frame + 1

    -- 输入快照：now = scripted or keys，prev = 上一帧 now
    for _, sc in ipairs(TRACKED) do
        prev[sc] = now[sc] or false
        now[sc] = scripted[sc] or keys[sc] or false
    end

    if S.scene == "menu" then
        update_menu()
    elseif S.scene == "play" then
        update_play(dt)
    else
        update_over()
    end
end

function joy.draw()
    local bg = (S.scene == "over") and { 14, 12, 18 } or { 16, 18, 26 }
    -- R2dBegin 用「当前颜色」清屏，所以先设清屏色再 begin
    B.R2dSetColorf(ren, bg[1] / 255.0, bg[2] / 255.0, bg[3] / 255.0, 1.0)
    B.R2dBegin(ren)

    if S.scene == "menu" then
        render_menu()
    elseif S.scene == "play" then
        render_play()
    else
        render_over()
    end

    B.R2dEnd(ren)
end

function joy.frame_end(dt)
    for _, sc in ipairs(TRACKED) do scripted[sc] = nil end
    if not (SMOKE_FRAMES and SMOKE_FRAMES > 0) then return end
    smoke_after_tick()
end

function joy.keypressed(key, sc, rep, ctrl, shift, alt)
    if sc then keys[sc] = true end
end

function joy.keyreleased(key, sc)
    if sc then keys[sc] = false end
end

function joy.mousepressed(x, y, b)
    if b == 1 then mouse_left = true end
end

function joy.mousereleased(x, y, b)
    if b == 1 then mouse_left = false end
end

function joy.shutdown()
    -- 句柄由 Lua GC 经 __gc → joy_obj_release 统一释放。
    -- 这里不能显式 R2dDestroy / DestroyWindow：userdata 内部还记着指针，
    -- 显式销毁后再被 GC 释放一次就是双重释放。
end
