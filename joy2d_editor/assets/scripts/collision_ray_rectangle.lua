local livnet = requirex "livnet"


local constant = livnet.constant;

local program = {};

local window, renderer;
program.start = function(ctx)
        local ok;
        sdl.set_app_metadata("collision2d", "1.0", "com.livgame.ray_rectangle");
        ok, window, renderer = sdl.create_window_and_renderer("ray_rectangle", 720, 480, 32);
        assert(ok);

        program.ctx = ctx;
        program.graphics_buffer = graphics.create();

end

local function handle_keydown_event(ctx, scancode)
        if scancode == constant.SDL_SCANCODE_Q then
                core.quit(ctx);
        elseif scancode == constant.SDL_SCANCODE_ESCAPE then
                core.quit(ctx);
        elseif scancode == constant.SDL_SCANCODE_UP then
        elseif scancode == constant.SDL_SCANCODE_RIGHT then
        elseif scancode == constant.SDL_SCANCODE_DOWN then
        elseif scancode == constant.SDL_SCANCODE_LEFT then
        end
end

program.event = function(event)
        local etype = event.type;
        if etype == constant.SDL_EVENT_KEY_DOWN then
                local scancode = event.key.scancode;
                handle_keydown_event(program.ctx, scancode);
        end
end



program.update = function()
        sdl.set_render_draw_color(renderer, 0, 0, 0, 255);
        sdl.render_clear(renderer);


        local x1, y1, w1, h1, x2, y2;

        local origin, direction, rect;
        origin = {
                x = fp.from_float(1),
                y = fp.from_float(3),
        };
        direction = {
                x = fp.from_float(0.5),
                y = fp.from_float(-1)
        };

        rect = {
                x = fp.from_float(0),
                y = fp.from_float(0),
                w = fp.from_float(2),
                h = fp.from_float(2)
        };

        local ray_collision = collision2df.get_ray_rectangle(origin, direction, rect);
        if ray_collision.hit then
                sdl.set_render_draw_color(renderer, 255, 0, 0, 255);

                x1 = fp.to_float(origin.x) * constant.PIXELS_PER_METER;
                y1 = fp.to_float(origin.y) * constant.PIXELS_PER_METER;
                x2 = fp.to_float(ray_collision.point.x) * constant.PIXELS_PER_METER;
                y2 = fp.to_float(ray_collision.point.y) * constant.PIXELS_PER_METER;
                sdl.render_line(renderer, x1, y1, x2, y2);
        else
                sdl.set_render_draw_color(renderer, 0, 255, 0, 255);
        end

        x1 = fp.to_float(rect.x) * constant.PIXELS_PER_METER;
        y1 = fp.to_float(rect.y) * constant.PIXELS_PER_METER;
        w1 = fp.to_float(rect.w) * constant.PIXELS_PER_METER;
        h1 = fp.to_float(rect.h) * constant.PIXELS_PER_METER;
        sdl.render_rect(renderer, x1, y1, w1, h1);



        graphics.render(renderer, program.graphics_buffer);
        graphics.clear(program.graphics_buffer);

        sdl.render_present(renderer);
end



program.destroy = function()
        sdl.destroy_renderer(renderer);
        sdl.destroy_window(window);
        livnet.log("collision test service exit");
end


return program;