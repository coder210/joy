// 3D Ray vs Quad collision detection demo
var PIXELS_PER_METER = 100;
var ray_pos, ray_dir;
var quad = [];
var ren;
var win;

joy.load = function()
{
    win = get_env("c07_win");
    ren = get_env("c07_ren");
    if (!ren) {
        win = window.create("ray-quad", 720, 480, 0);
        ren = render2d.create(win);
    }

    ray_pos = { x: fp.from_float(0), y: fp.from_float(0), z: fp.from_float(10) };
    ray_dir = { x: fp.from_float(0), y: fp.from_float(0), z: fp.from_float(1) };
    quad = [
        {x: fp.from_float(-1), y: fp.from_float(-1), z: fp.from_float(0)},
        {x: fp.from_float(1),  y: fp.from_float(-1), z: fp.from_float(0)},
        {x: fp.from_float(1),  y: fp.from_float(1),  z: fp.from_float(0)},
        {x: fp.from_float(-1), y: fp.from_float(1),  z: fp.from_float(0)}
    ];
};

joy.hotfix = function() {
    win = get_env("c07_win");
    ren = get_env("c07_ren");
};

joy.draw = function()
{
    render2d.set_color(ren, 0, 0, 0, 255);
    render2d.begin(ren);

    var result = collision3d.ray_quad_int(ray_pos, ray_dir,
        quad[0], quad[1], quad[2], quad[3]);
    if (result.hit) {
        render2d.set_color(ren, 0, 255, 0, 255);
    } else {
        render2d.set_color(ren, 255, 0, 0, 255);
    }

    // draw quad (top-down 2D projection)
    var x1 = fp.to_float(quad[0].x) * PIXELS_PER_METER + 360;
    var y1 = fp.to_float(quad[0].y) * PIXELS_PER_METER + 240;
    var x2 = fp.to_float(quad[1].x) * PIXELS_PER_METER + 360;
    var y2 = fp.to_float(quad[1].y) * PIXELS_PER_METER + 240;
    var x3 = fp.to_float(quad[2].x) * PIXELS_PER_METER + 360;
    var y3 = fp.to_float(quad[2].y) * PIXELS_PER_METER + 240;
    var x4 = fp.to_float(quad[3].x) * PIXELS_PER_METER + 360;
    var y4 = fp.to_float(quad[3].y) * PIXELS_PER_METER + 240;
    render2d.line(ren, x1, y1, x2, y2);
    render2d.line(ren, x2, y2, x3, y3);
    render2d.line(ren, x3, y3, x4, y4);
    render2d.line(ren, x4, y4, x1, y1);

    render2d.end_frame(ren);
};

joy.keypressed = function(key, scancode, repeat_, ctrl, shift, alt)
{
    if (key === "escape") event.quit();
};

joy.quit = function()
{
    if (ren) { render2d.destroy(ren); ren = null; }
    if (win) { window.destroy(win); win = null; }
};
