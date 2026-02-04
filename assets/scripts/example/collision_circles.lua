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
        sdl.set_app_metadata("collision2d", "1.0", "com.livnet.collision");
        ok, window, renderer = sdl.create_window_and_renderer("circles", 720, 480);
        assert(ok);

        graphics_buffer = graphics.create();

        local circle1, circle2;
        circle1 = {
                x = fp.from_float(2.2),
                y = fp.from_float(2.2),
                r = fp.from_float(0.64)
        };
        circle2 = {
                x = fp.from_float(2.35),
                y = fp.from_float(2.35),
                r = fp.from_float(0.53)
        };

        livnet.dispatch("response", function(data, sz, source, ...)
                handle_event();
                update();
                sdl.set_render_draw_color(renderer, 0, 0, 0, 255);
                sdl.render_clear(renderer);

                ok, contact = collision2df.circlesx(circle1, circle2);
                if ok then
                        sdl.set_render_draw_color(renderer, 255, 0, 0, 255);
                        --livnet.log(cjson.encode(contact));
                        local sp, ep, normal, depth;
                        depth = fp.to_float(contact.depth);
                        sp = {};
                        sp.x = fp.to_float(contact.sp.x) * PIXELS_PER_METER;
                        sp.y = fp.to_float(contact.sp.y) * PIXELS_PER_METER;
                        ep = {};
                        ep.x = fp.to_float(contact.ep.x) * PIXELS_PER_METER;
                        ep.y = fp.to_float(contact.ep.y) * PIXELS_PER_METER;
                        normal = {};
                        normal.x = sp.x + depth * fp.to_float(contact.normal.x) * PIXELS_PER_METER;
                        normal.y = sp.y + depth * fp.to_float(contact.normal.y) * PIXELS_PER_METER;

                        livnet.log(cjson.encode(normal) .. ":" .. depth);
                        sdl.render_line(renderer, sp.x, sp.y, ep.x, ep.y);
                        ---sdl.render_line(renderer, 0, 0, normal.x, normal.y);
                else
                        sdl.set_render_draw_color(renderer, 0, 255, 0, 255);
                end

                x = fp.to_float(circle1.x) * PIXELS_PER_METER;
                y =  fp.to_float(circle1.y) * PIXELS_PER_METER;
                r =  fp.to_float(circle1.r) * PIXELS_PER_METER;
                graphics.circle(renderer, graphics_buffer, x, y, r, 50, 2);

                x =  fp.to_float(circle2.x) * PIXELS_PER_METER;
                y =  fp.to_float(circle2.y) * PIXELS_PER_METER;
                r =  fp.to_float(circle2.r) * PIXELS_PER_METER;
                graphics.circle(renderer, graphics_buffer, x, y, r, 50, 2);

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
