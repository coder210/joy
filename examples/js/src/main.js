/* ============================================================
 * joy 示例 · JavaScript(duktape) 版
 *
 * 与 cpp / csharp / lua 三个示例功能完全相同：
 *   Menu → Play（WASD 移动 / Space 射击 / 敌人追击 / 碰撞掉血）→ GameOver
 *
 * 与 Lua 版的两点差异（都是 jsclib 旧胶水的现状，不改引擎）：
 *   1. 模块是**全局对象** r2d / window / keyboard / console，不是 joy.r2d；
 *      只有生命周期回调挂在全局 joy 对象上（jsx_call_joy 走这条路）。
 *   2. r2d 表里没有图片相关函数（new_image / draw_image / draw_image_ex）。
 *      当前示例已全量改走生成链路：用 B.ImageCreate + B.ImageDrawRect 画
 *      真实贴图（全标量版，srcrect 固定 NULL），加载失败才退回同尺寸色块。
 *
 * 宿主（../main.cpp）只往这里推：joy.init / joy.update(dt) / joy.draw / joy.frame_end。
 * ============================================================ */

var W = 480, H = 320;

var ARENA_MARGIN = 10;
var ENEMY_CONTACT_DPS = 26;
var MAX_ENEMIES = 24;
var MAX_BULLETS = 64;

var PLAYER_RADIUS = 11;
var PLAYER_SPEED = 130;
var PLAYER_MAX_HP = 100;
var PLAYER_FIRE_INTERVAL = 0.16;

var ENEMY_RADIUS = 11;

var BULLET_RADIUS = 3;
var BULLET_SPEED = 320;
var BULLET_LIFE = 1.5;
var BULLET_DAMAGE = 1;

/* SDL_Scancode */
var SC = {
    A: 4, D: 7, J: 13, S: 22, W: 26,
    RETURN: 40, ESCAPE: 41, SPACE: 44,
    RIGHT: 79, LEFT: 80, DOWN: 81, UP: 82
};
var TRACKED = [SC.A, SC.D, SC.S, SC.W, SC.J, SC.RETURN, SC.ESCAPE, SC.SPACE,
               SC.RIGHT, SC.LEFT, SC.DOWN, SC.UP];

/* ---------------- 确定性随机（与另外三份同算法同种子） ----------------
 * JS 的位运算天然是 32 位有符号，最后 >>> 0 转无符号，与 C 的 unsigned 等价。 */
var rngState = 12345;

function nextU32() {
    var s = rngState;
    s = (s ^ (s << 13));
    s = (s ^ (s >>> 17));
    s = (s ^ (s << 5));
    s = s >>> 0;
    rngState = s;
    return s;
}

function nextDouble() {
    return (nextU32() >>> 8) / 16777216.0;
}

function nextInt(n) {
    return nextU32() % n;
}

function rngRange(lo, hi) {
    return lo + nextDouble() * (hi - lo);
}

/* ---------------- 状态 ---------------- */
var S = {
    scene: "menu",
    px: 0, py: 0,
    hp: PLAYER_MAX_HP,
    fireCd: 0,
    facingX: 1, facingY: 0,
    enemies: [], bullets: [],
    elapsed: 0, spawnTimer: 0,
    kills: 0, fired: 0,
    finished: false,
    frame: 0,
    finalKills: 0, finalSeconds: 0
};

var win = null, ren = null;
var fontSmall = null, fontBig = null;
var hasFont = false;

var keys = {}, nowKeys = {}, prevKeys = {}, scripted = {};
var mouseLeft = false;

function down(sc) { return nowKeys[sc] === true; }
function pressed(sc) { return nowKeys[sc] === true && prevKeys[sc] !== true; }

/* ---------------- 绘制辅助（全部走生成链路） ----------------
 * 窗口、渲染、矩形、直线、字体、文字都走 Joy.Api.Bindings.*，
 * 不再用 r2d / window 那套旧手写胶水。
 * 唯一仍走旧胶水的是输入：joy.keypressed / joy.keyreleased 事件回调不在 IDL 里。 */
var B = Joy.Api.Bindings;

function setColor(r, g, b, a) {
    B.R2dSetColorf(ren, r / 255.0, g / 255.0, b / 255.0,
                   (a === undefined ? 255 : a) / 255.0);
}

function fill(x, y, w, h, r, g, b, a) {
    setColor(r, g, b, a);
    B.ShapeDrawRectangle(ren, "fill", x, y, w, h);
}

function stroke(x, y, w, h, r, g, b, a) {
    setColor(r, g, b, a);
    B.ShapeDrawRectangle(ren, "line", x, y, w, h);
}

// *_f 系列参数是归一化比例（r2d_fpx 会乘窗口宽高），这里传的是像素坐标，
// 必须用非 _f 的 ShapeDrawLine，否则网格线会被放大到屏幕外。
function drawLine(x1, y1, x2, y2, r, g, b, a) {
    setColor(r, g, b, a);
    B.ShapeDrawLine(ren, x1, y1, x2, y2);
}

function text(s, x, y, r, g, b) {
    if (!hasFont) return;
    B.JtextPrintx(ren, fontSmall, s, s.length,
                  r === undefined ? 232 : r,
                  g === undefined ? 236 : g,
                  b === undefined ? 245 : b, 255, x, y);
}

function textWidth(s, size) {
    return s.length * size * 0.58;
}

function textCenter(s, cx, cy, big, r, g, b) {
    if (!hasFont) return;
    var f = big ? fontBig : fontSmall;
    var size = big ? 22 : 12;
    var w = textWidth(s, size);
    B.JtextPrintx(ren, f, s, s.length,
                  r === undefined ? 232 : r,
                  g === undefined ? 236 : g,
                  b === undefined ? 245 : b, 255,
                  cx - w * 0.5, cy - size * 0.5);
}

var imgPlayer, imgEnemy;

// ImageDrawRect 是全标量版（srcrect 固定 NULL 表示整张图），绕开了
// jrect_t* 无法进 joy_value 的 L3 限制；句柄为空时退回同尺寸色块。
function sprite(img, cx, cy, size, r, g, b) {
    if (img) {
        B.ImageDrawRect(ren, img, cx - size * 0.5, cy - size * 0.5, size, size);
    } else {
        fill(cx - size * 0.5, cy - size * 0.5, size, size, r, g, b);
    }
}

/* ---------------- 资源 ---------------- */
var FONT_CANDIDATES = [
    "assets/fonts/simhei.ttf",
    "C:/Windows/Fonts/simhei.ttf",
    "C:/Windows/Fonts/Deng.ttf"
];

/* 贴图由 CMake 部署到 assets/sprites/；加载失败时兜住 duk_error，
 * 让 sprite 退回色块而不是中断整个脚本。 */
function tryImage(path) {
    try { return B.ImageCreate(ren, path); } catch (e) { return null; }
}

function loadAssets() {
    /* 生成链路的 handle 在脚本层无法判空，只认仓内自带的那份字体 */
    fontSmall = B.FontCreate(FONT_CANDIDATES[0], 12);
    fontBig = B.FontCreate(FONT_CANDIDATES[0], 22);
    hasFont = true;
    imgPlayer = tryImage("assets/sprites/player.png");
    imgEnemy = tryImage("assets/sprites/enemy.png");
}

/* ---------------- 场景 ---------------- */
function enterMenu() {
    S.scene = "menu";
    console.log("[scene] Menu");
}

function makeEnemy(x, y, hp) {
    return { x: x, y: y, spawnX: x, spawnY: y, hp: hp, speed: 44 + nextDouble() * 10 };
}

function enterPlay() {
    S.scene = "play";
    S.px = W * 0.5;
    S.py = H * 0.5;
    S.hp = PLAYER_MAX_HP;
    S.fireCd = 0;
    S.facingX = 1;
    S.facingY = 0;
    S.elapsed = 0;
    S.kills = 0;
    S.fired = 0;
    S.finished = false;
    S.enemies = [];
    S.bullets = [];
    S.spawnTimer = (SMOKE_FRAMES > 0) ? 1.5 : 1.0;

    console.log("[scene] Play");

    if (SMOKE_FRAMES > 0) {
        S.enemies.push(makeEnemy(S.px + 90, S.py, 2));
        S.enemies.push(makeEnemy(S.px + 62, S.py + 62, 2));
        S.enemies.push(makeEnemy(S.px, S.py + 90, 2));
    }
}

function enterOver(kills, seconds) {
    S.scene = "over";
    S.finalKills = kills;
    S.finalSeconds = seconds;
    console.log("[scene] GameOver kills=" + kills + " time=" + seconds.toFixed(1));
}

/* ---------------- 更新 ---------------- */
function updatePlayer(dt) {
    var dx = 0, dy = 0;
    if (down(SC.A) || down(SC.LEFT)) dx -= 1;
    if (down(SC.D) || down(SC.RIGHT)) dx += 1;
    if (down(SC.W) || down(SC.UP)) dy -= 1;
    if (down(SC.S) || down(SC.DOWN)) dy += 1;

    if (dx !== 0 || dy !== 0) {
        var len = Math.sqrt(dx * dx + dy * dy);
        dx /= len;
        dy /= len;
        S.px += dx * PLAYER_SPEED * dt;
        S.py += dy * PLAYER_SPEED * dt;
        S.facingX = dx;
        S.facingY = dy;
    }

    var minx = ARENA_MARGIN + PLAYER_RADIUS;
    var maxx = W - ARENA_MARGIN - PLAYER_RADIUS;
    var miny = ARENA_MARGIN + PLAYER_RADIUS;
    var maxy = H - ARENA_MARGIN - PLAYER_RADIUS;
    if (S.px < minx) S.px = minx;
    if (S.px > maxx) S.px = maxx;
    if (S.py < miny) S.py = miny;
    if (S.py > maxy) S.py = maxy;

    S.fireCd -= dt;
    var wantFire = down(SC.SPACE) || down(SC.J) || mouseLeft;
    if (wantFire && S.fireCd <= 0 && S.bullets.length < MAX_BULLETS) {
        S.fireCd = PLAYER_FIRE_INTERVAL;
        S.bullets.push({
            x: S.px + S.facingX * (PLAYER_RADIUS + 4),
            y: S.py + S.facingY * (PLAYER_RADIUS + 4),
            vx: S.facingX * BULLET_SPEED,
            vy: S.facingY * BULLET_SPEED,
            life: BULLET_LIFE,
            damage: BULLET_DAMAGE
        });
        S.fired++;
    }
}

function updateBullets(dt) {
    var keep = [];
    for (var i = 0; i < S.bullets.length; i++) {
        var b = S.bullets[i];
        b.x += b.vx * dt;
        b.y += b.vy * dt;
        b.life -= dt;
        if (b.life > 0 && b.x >= -20 && b.y >= -20 && b.x <= W + 20 && b.y <= H + 20) {
            keep.push(b);
        }
    }
    S.bullets = keep;
}

function updateEnemies(dt) {
    for (var i = 0; i < S.enemies.length; i++) {
        var e = S.enemies[i];
        var dx = S.px - e.x;
        var dy = S.py - e.y;
        var len = Math.sqrt(dx * dx + dy * dy);
        if (len > 0.001) {
            e.x += dx / len * e.speed * dt;
            e.y += dy / len * e.speed * dt;
        }
        if (len < PLAYER_RADIUS + ENEMY_RADIUS) {
            S.hp -= ENEMY_CONTACT_DPS * dt;
        }
    }
}

function resolveBulletHits() {
    for (var bi = S.bullets.length - 1; bi >= 0; bi--) {
        var b = S.bullets[bi];
        var hit = false;
        for (var ei = S.enemies.length - 1; ei >= 0; ei--) {
            var e = S.enemies[ei];
            var dx = e.x - b.x;
            var dy = e.y - b.y;
            var rr = ENEMY_RADIUS + BULLET_RADIUS;
            if (dx * dx + dy * dy > rr * rr) continue;

            e.hp -= b.damage;
            hit = true;
            if (e.hp <= 0) {
                S.enemies.splice(ei, 1);
                S.kills++;
            }
            break;
        }
        if (hit) S.bullets.splice(bi, 1);
    }
}

function updateSpawn(dt) {
    S.spawnTimer -= dt;
    if (S.spawnTimer > 0) return;

    S.spawnTimer = Math.max(0.45, 1.3 - S.elapsed * 0.02);
    if (S.enemies.length >= MAX_ENEMIES) return;

    var margin = ARENA_MARGIN + 22;
    var x, y;
    switch (nextInt(4)) {
    case 0: x = margin; y = rngRange(margin, H - margin); break;
    case 1: x = W - margin; y = rngRange(margin, H - margin); break;
    case 2: x = rngRange(margin, W - margin); y = margin; break;
    default: x = rngRange(margin, W - margin); y = H - margin; break;
    }
    S.enemies.push(makeEnemy(x, y, 3));
}

function updatePlay(dt) {
    S.elapsed += dt;

    updatePlayer(dt);
    updateBullets(dt);
    updateEnemies(dt);
    resolveBulletHits();
    updateSpawn(dt);

    if (S.hp <= 0 && !S.finished) {
        S.finished = true;
        enterOver(S.kills, S.elapsed);
    }
}

function updateMenu() {
    if (pressed(SC.RETURN) || pressed(SC.SPACE)) { enterPlay(); return; }
    if (pressed(SC.ESCAPE)) { quit = true; exit_code = 0; }
}

function updateOver() {
    if (pressed(SC.RETURN) || pressed(SC.SPACE)) { enterPlay(); return; }
    if (pressed(SC.ESCAPE)) { enterMenu(); }
}

/* ---------------- 渲染 ---------------- */
function renderMenu() {
    var cx = W * 0.5;

    for (var x = 0; x <= W; x += 32) drawLine(x, 0, x, H, 26, 30, 42);
    for (var y = 0; y <= H; y += 32) drawLine(0, y, W, y, 26, 30, 42);

    sprite(imgPlayer, cx, H * 0.5 - 74, 34, 90, 200, 140);
    textCenter("JOY JS TOPDOWN", cx, H * 0.5 - 26, true, 120, 220, 255);
    textCenter("JavaScript + joy engine", cx, H * 0.5 + 4, false, 150, 160, 180);

    if (Math.floor(S.frame / 30) % 2 === 0) {
        textCenter("Enter / Space to start", cx, H * 0.5 + 44, false, 255, 226, 140);
    }
    textCenter("WASD move | Space/LMB fire | Esc quit",
               cx, H * 0.5 + 76, false, 130, 138, 158);
    textCenter("Scene: Menu", cx, 18, false, 90, 96, 112);
}

function renderPlay() {
    for (var x = ARENA_MARGIN; x <= W - ARENA_MARGIN; x += 40) {
        drawLine(x, ARENA_MARGIN, x, H - ARENA_MARGIN, 24, 28, 40);
    }
    for (var y = ARENA_MARGIN; y <= H - ARENA_MARGIN; y += 40) {
        drawLine(ARENA_MARGIN, y, W - ARENA_MARGIN, y, 24, 28, 40);
    }
    stroke(ARENA_MARGIN, ARENA_MARGIN,
           W - ARENA_MARGIN * 2, H - ARENA_MARGIN * 2, 58, 66, 92);

    for (var i = 0; i < S.bullets.length; i++) {
        var b = S.bullets[i];
        fill(b.x - BULLET_RADIUS, b.y - BULLET_RADIUS,
             BULLET_RADIUS * 2, BULLET_RADIUS * 2, 255, 224, 130);
    }

    for (var j = 0; j < S.enemies.length; j++) {
        var e = S.enemies[j];
        sprite(imgEnemy, e.x, e.y, 24, 210, 96, 96);
        if (e.hp > 1) {
            fill(e.x - 9, e.y - 17, 18 * (e.hp / 3), 2, 230, 120, 110, 200);
        }
    }

    sprite(imgPlayer, S.px, S.py, 26, 110, 210, 150);
    drawLine(S.px, S.py, S.px + S.facingX * 18, S.py + S.facingY * 18, 180, 240, 210);

    /* HUD */
    var barW = 120;
    var hpRatio = S.hp <= 0 ? 0 : (S.hp / PLAYER_MAX_HP);
    fill(12, 12, barW, 10, 40, 42, 54, 220);
    fill(12, 12, barW * hpRatio, 10, 96, 214, 140, 235);
    stroke(12, 12, barW, 10, 90, 96, 118);
    text("HP " + Math.ceil(Math.max(0, S.hp)), 12, 26);

    var stats = "KILLS " + S.kills + " | WAVE " + (1 + Math.floor(S.kills / 8)) +
                " | TIME " + S.elapsed.toFixed(1) + "s";
    text(stats, W - 12 - textWidth(stats, 12), 12);

    text("ENEMIES " + S.enemies.length, 12, H - 24);
    var hint = "WASD move | Space fire | Esc menu";
    text(hint, W - 12 - textWidth(hint, 12), H - 24, 120, 128, 148);
}

function renderOver() {
    var cx = W * 0.5, cy = H * 0.5;

    fill(cx - 150, cy - 70, 300, 140, 26, 22, 30, 235);
    stroke(cx - 150, cy - 70, 300, 140, 90, 60, 70);

    textCenter("GAME OVER", cx, cy - 40, true, 255, 120, 120);
    textCenter("Kills " + S.finalKills + " | Time " + S.finalSeconds.toFixed(1) + "s",
               cx, cy - 6, false, 230, 232, 240);
    textCenter("Enter retry | Esc menu", cx, cy + 28, false, 160, 168, 188);
    textCenter("Scene: GameOver", cx, 18, false, 90, 96, 112);
}

/* ---------------- 冒烟脚本（对应 cpp 的 smoke_*） ---------------- */
var SMOKE_ASSERT_FRAME = 150;
var smoke = {
    playCount: 0, inPlay: false, sawOver: false, enemyMoved: false,
    startX: 0, startY: 0, maxMoved: 0,
    maxFired: 0, maxKills: 0, maxEnemies: 0,
    killedPlayer: false, done: false
};

function smokeBeforeTick() {
    var f = S.frame;
    scripted[SC.RETURN] = (f >= 2 && f < 4) || (f >= 126 && f < 128);
    scripted[SC.RIGHT] = (f >= 10 && f < 90);
    scripted[SC.DOWN] = (f >= 10 && f < 60);
    var rel = f - 20;
    scripted[SC.SPACE] = (f >= 20 && (rel % 12) < 2);

    if (f === 120 && !smoke.killedPlayer && S.scene === "play") {
        smoke.killedPlayer = true;
        S.hp = 0;
    }
}

function enemyApproached(e) {
    var sx = e.spawnX - S.px, sy = e.spawnY - S.py;
    var nx = e.x - S.px, ny = e.y - S.py;
    var before = Math.sqrt(sx * sx + sy * sy);
    var n = Math.sqrt(nx * nx + ny * ny);
    return n < before - 2;
}

function smokeObserve() {
    if (S.scene === "play") {
        if (!smoke.inPlay) {
            smoke.inPlay = true;
            smoke.playCount++;
            if (smoke.playCount === 1) {
                smoke.startX = S.px;
                smoke.startY = S.py;
            }
        }
        if (smoke.playCount === 1) {
            var dx = S.px - smoke.startX;
            var dy = S.py - smoke.startY;
            var moved = Math.sqrt(dx * dx + dy * dy);
            if (moved > smoke.maxMoved) smoke.maxMoved = moved;
        }
        if (S.fired > smoke.maxFired) smoke.maxFired = S.fired;
        if (S.kills > smoke.maxKills) smoke.maxKills = S.kills;
        if (S.enemies.length > smoke.maxEnemies) smoke.maxEnemies = S.enemies.length;
        if (!smoke.enemyMoved) {
            for (var i = 0; i < S.enemies.length; i++) {
                if (enemyApproached(S.enemies[i])) { smoke.enemyMoved = true; break; }
            }
        }
    } else {
        smoke.inPlay = false;
        if (S.scene === "over") smoke.sawOver = true;
    }
}

function smokeAfterTick() {
    smokeObserve();
    if (smoke.done || S.frame < SMOKE_ASSERT_FRAME) return;
    smoke.done = true;

    var ok = true;
    ok = ok && smoke.playCount >= 2;
    ok = ok && smoke.sawOver;
    ok = ok && smoke.maxMoved > 5;
    ok = ok && smoke.maxFired > 0;
    ok = ok && smoke.maxKills > 0;
    ok = ok && smoke.enemyMoved;

    console.log("[smoke] " + (ok ? "PASS" : "FAIL") +
        " api=n/a abi=True play=" + smoke.playCount +
        " gameover=" + (smoke.sawOver ? "True" : "False") +
        " moved=" + smoke.maxMoved.toFixed(1) +
        " fired=" + smoke.maxFired +
        " kills=" + smoke.maxKills +
        " enemies=" + smoke.maxEnemies);

    exit_code = ok ? 0 : 1;
    quit = true;
}

/* ---------------- joy 回调 ---------------- */
joy.init = function () {
    win = B.CreateWindow("Joy JS Topdown", W, H);
    ren = B.R2dCreate(win);

    loadAssets();
    enterMenu();
};

joy.frame_begin = function () {
    if (!(SMOKE_FRAMES > 0)) return;
    smokeBeforeTick();
};

joy.update = function (dt) {
    S.frame++;

    for (var i = 0; i < TRACKED.length; i++) {
        var sc = TRACKED[i];
        prevKeys[sc] = nowKeys[sc] || false;
        nowKeys[sc] = scripted[sc] || keys[sc] || false;
    }

    if (S.scene === "menu") updateMenu();
    else if (S.scene === "play") updatePlay(dt);
    else updateOver();
};

joy.draw = function () {
    var bg = (S.scene === "over") ? [14, 12, 18] : [16, 18, 26];
    /* R2dBegin 用「当前颜色」清屏，所以先设清屏色再 begin */
    B.R2dSetColorf(ren, bg[0] / 255.0, bg[1] / 255.0, bg[2] / 255.0, 1.0);
    B.R2dBegin(ren);

    if (S.scene === "menu") renderMenu();
    else if (S.scene === "play") renderPlay();
    else renderOver();

    B.R2dEnd(ren);
};

joy.frame_end = function () {
    for (var i = 0; i < TRACKED.length; i++) scripted[TRACKED[i]] = false;
    if (!(SMOKE_FRAMES > 0)) return;
    smokeAfterTick();
};

joy.keypressed = function (key, sc) { if (sc !== undefined) keys[sc] = true; };
joy.keyreleased = function (key, sc) { if (sc !== undefined) keys[sc] = false; };
joy.mousepressed = function (x, y, b) { if (b === 1) mouseLeft = true; };
joy.mousereleased = function (x, y, b) { if (b === 1) mouseLeft = false; };

joy.shutdown = function () {
    /* 句柄由 duktape finalizer 统一释放；
       显式 destroy 会被 finalizer 再释放一次（双重释放）。 */
};
