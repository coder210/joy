-- 3D Ray vs Quad collision detection demo
local PIXELS_PER_METER = 100
local ray_pos, ray_dir
local quad = {}
local ren
local win

function joy.load()
    win = joy.window.create("ray-quad", 720, 480, 0)
    ren = joy.render2d.create(win)
    set_env("c07_win", win)
    set_env("c07_ren", ren)

    ray_pos = { x = joy.fp.from_float(0), y = joy.fp.from_float(0), z = joy.fp.from_float(10) }
    ray_dir = { x = joy.fp.from_float(0), y = joy.fp.from_float(0), z = joy.fp.from_float(1) }
    quad = {
        {x = joy.fp.from_float(-1), y = joy.fp.from_float(-1), z = joy.fp.from_float(0)},
        {x = joy.fp.from_float(1),  y = joy.fp.from_float(-1), z = joy.fp.from_float(0)},
        {x = joy.fp.from_float(1),  y = joy.fp.from_float(1),  z = joy.fp.from_float(0)},
        {x = joy.fp.from_float(-1), y = joy.fp.from_float(1),  z = joy.fp.from_float(0)}
    }
end

function joy.hotfix()
    win = get_env("c07_win")
    ren = get_env("c07_ren")
end

function joy.draw()
    joy.render2d.begin(ren, 0, 0, 0, 255)

    local result = joy.collision3d.ray_quad_int(ray_pos, ray_dir,
        quad[1], quad[2], quad[3], quad[4])
    if result.hit then
        joy.render2d.set_color(ren, 0, 255, 0, 255)
    else
        joy.render2d.set_color(ren, 255, 0, 0, 255)
    end

    -- draw quad (top-down 2D projection)
    local x1 = joy.fp.to_float(quad[1].x) * PIXELS_PER_METER + 360
    local y1 = joy.fp.to_float(quad[1].y) * PIXELS_PER_METER + 240
    local x2 = joy.fp.to_float(quad[2].x) * PIXELS_PER_METER + 360
    local y2 = joy.fp.to_float(quad[2].y) * PIXELS_PER_METER + 240
    local x3 = joy.fp.to_float(quad[3].x) * PIXELS_PER_METER + 360
    local y3 = joy.fp.to_float(quad[3].y) * PIXELS_PER_METER + 240
    local x4 = joy.fp.to_float(quad[4].x) * PIXELS_PER_METER + 360
    local y4 = joy.fp.to_float(quad[4].y) * PIXELS_PER_METER + 240
    joy.render2d.line(ren, x1, y1, x2, y2)
    joy.render2d.line(ren, x2, y2, x3, y3)
    joy.render2d.line(ren, x3, y3, x4, y4)
    joy.render2d.line(ren, x4, y4, x1, y1)

    joy.render2d.end_frame(ren)
end

function joy.keypressed(key, scancode, repeat_, ctrl, shift, alt)
    if key == "escape" then joy.event.quit() end
end

function joy.quit()
    if ren then joy.render2d.destroy(ren); ren = nil end
    if win then joy.window.destroy(win); win = nil end
end
