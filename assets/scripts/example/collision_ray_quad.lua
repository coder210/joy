local livnet = requirex "livnet"


local PIXELS_PER_METER = 100;

local SDL_EVENT_KEY_DOWN = 768;
local SDL_SCANCODE_ESCAPE = 41;
local SDL_SCANCODE_Q = 41;
local SDL_SCANCODE_RIGHT = 79;
local SDL_SCANCODE_UP = 82;
local SDL_SCANCODE_LEFT = 80;
local SDL_SCANCODE_DOWN = 81;

local window, renderer;


local function exit_all()
        livnet.exit();
        local bootstrap_handle = livnet.query("bootstrap");
        livnet.kill(bootstrap_handle);
        livnet.log("bootstrap_handle:" .. bootstrap_handle);
end

local function handle_keydown_event(scancode)
        if scancode == SDL_SCANCODE_Q then
                exit_all();
        elseif scancode == SDL_SCANCODE_ESCAPE then
                exit_all();
        elseif scancode == SDL_SCANCODE_UP then
        elseif scancode == SDL_SCANCODE_RIGHT then
        elseif scancode == SDL_SCANCODE_DOWN then
        elseif scancode == SDL_SCANCODE_LEFT then
        end
end


local function handle_event()
        local ok, ev;
        while true do
                ok, ev = sdl.poll_event();
                if ok then
                        local etype = sdl.get_event_type(ev);
                        local scancode = sdl.get_event_scancode(ev);
                        if etype == SDL_EVENT_KEY_DOWN then
                                handle_keydown_event(scancode);
                        end
                else
                        break;
                end
        end
end

local function update()

end


livnet.start(function()
        local graphics_buffer, contact;
        local ok;
        sdl.set_app_metadata("collision2d", "1.0", "com.livgame.collision2d");
        ok, window, renderer = sdl.create_window_and_renderer("collision2d", 720, 480);
        assert(ok);

        graphics_buffer = graphics.create();

        local ray_pos, ray_dir, p1, p2, p3, p4;
        ray_pos = {};
        ray_dir = {};
        p1 = {};
        p2 = {};
        p3 = {};
        p4 = {};

        ray_pos.x = fp.from_float(0.5);
        ray_pos.y = fp.from_float(0.5);
        ray_pos.z = fp.from_float(1);
        ray_dir.x = fp.from_float(0);
        ray_dir.y = fp.from_float(0);
        ray_dir.z = fp.from_float(-1);

        local start_pos = {};
        start_pos.x = 0;
        start_pos.y = 0;

        p1.x = fp.from_float(start_pos.x + 0);
        p1.y = fp.from_float(start_pos.y + 0);
        p1.z = fp.from_float(0);

        p2.x = fp.from_float(start_pos.x + 1);
        p2.y = fp.from_float(start_pos.y + 0);
        p2.z = fp.from_float(0);
        
        p3.x = fp.from_float(start_pos.x + 1);
        p3.y = fp.from_float(start_pos.y + 1);
        p3.z = fp.from_float(0);

        p4.x = fp.from_float(start_pos.x + 0);
        p4.y = fp.from_float(start_pos.y + 1);
        p4.z = fp.from_float(0);


        livnet.dispatch("response", function(data, sz, source, ...)
                handle_event();
                update();
                sdl.set_render_draw_color(renderer, 0, 0, 0, 255);
                sdl.render_clear(renderer);

                local result = collision3df.ray_quad(ray_pos, ray_dir, p1, p2, p3, p4);
                if result.hit then
                        sdl.set_render_draw_color(renderer, 255, 0, 0, 255);
                else
                        sdl.set_render_draw_color(renderer, 0, 255, 0, 255);
                end


                local x2, y2, x3, y3;
                x2 = fp.to_float(ray_pos.x) * PIXELS_PER_METER;
                y2 = fp.to_float(ray_pos.y) * PIXELS_PER_METER;
                x3 = x2 + fp.to_float(ray_dir.x) * PIXELS_PER_METER;
                y3 = y2 + fp.to_float(ray_dir.y) * PIXELS_PER_METER;
                sdl.render_line(renderer, x2, y2, x3, y3);

                x2 = fp.to_float(p1.x) * PIXELS_PER_METER;
                y2 = fp.to_float(p1.y) * PIXELS_PER_METER;
                x3 = fp.to_float(p2.x) * PIXELS_PER_METER;
                y3 = fp.to_float(p2.y) * PIXELS_PER_METER;
                sdl.render_line(renderer, x2, y2, x3, y3);

                x2 = fp.to_float(p2.x) * PIXELS_PER_METER;
                y2 = fp.to_float(p2.y) * PIXELS_PER_METER;
                x3 = fp.to_float(p3.x) * PIXELS_PER_METER;
                y3 = fp.to_float(p3.y) * PIXELS_PER_METER;
                sdl.render_line(renderer, x2, y2, x3, y3);
                
                x2 = fp.to_float(p3.x) * PIXELS_PER_METER;
                y2 = fp.to_float(p3.y) * PIXELS_PER_METER;
                x3 = fp.to_float(p4.x) * PIXELS_PER_METER;
                y3 = fp.to_float(p4.y) * PIXELS_PER_METER;
                sdl.render_line(renderer, x2, y2, x3, y3);

                x2 = fp.to_float(p4.x) * PIXELS_PER_METER;
                y2 = fp.to_float(p4.y) * PIXELS_PER_METER;
                x3 = fp.to_float(p1.x) * PIXELS_PER_METER;
                y3 = fp.to_float(p1.y) * PIXELS_PER_METER;
                sdl.render_line(renderer, x2, y2, x3, y3);
                --graphics.rectr(renderer, graphics_buffer, x1, y1, w1, h1, 0, 2);

                graphics.render(renderer, graphics_buffer);
                graphics.clear(graphics_buffer);

                sdl.render_present(renderer);
                livnet.timeout(16);
        end);
        livnet.timeout(16);

        livnet.exit_func(function()
                graphics.destroy(graphics_buffer);
                sdl.destroy_renderer(renderer);
                sdl.destroy_window(window);
                livnet.log("collision test service exit");
        end);
end);
