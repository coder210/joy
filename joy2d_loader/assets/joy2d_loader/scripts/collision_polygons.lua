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


local function draw(entities)
        sdl.set_render_draw_color(renderer, 0, 0, 0, 255);
        sdl.render_clear(renderer);
        -- for id, entity in pairs(entities) do
        --         sdl.set_render_draw_color(renderer, 0, 128, 0, 255);
        --         local x = fp.to_float(entity.position.x) * PIXELS_PER_METER;
        --         local y = fp.to_float(entity.position.y) * PIXELS_PER_METER;
        --         local w = fp.to_float(entity.width) * PIXELS_PER_METER;
        --         local h = fp.to_float(entity.height) * PIXELS_PER_METER;
        --         sdl.render_fill_rect(renderer, x, y, w, h);
        -- end
        sdl.render_present(renderer);
end


livnet.start(function()
        local graphics_buffer, contact;
        local ok;
        sdl.set_app_metadata("collision2df2d", "1.0", "com.livgame.collision2df2d");
        ok, window, renderer = sdl.create_window_and_renderer("collision2df2d", 720, 480);
        assert(ok);

        graphics_buffer = graphics.create();

        local a_vertices, b_vertices;
        a_vertices = {
                {
                        x = fp.from_float(1),
                        y = fp.from_float(1)
                },
                {
                        x = fp.from_float(2),
                        y = fp.from_float(1)
                },
                {
                        x = fp.from_float(2),
                        y = fp.from_float(2)
                },
                {
                        x = fp.from_float(1),
                        y = fp.from_float(2)
                }
        };
        b_vertices = {
                {
                        x = fp.from_float(1.75),
                        y = fp.from_float(1.75)
                },
                {
                        x = fp.from_float(3),
                        y = fp.from_float(1.5)
                },
                {
                        x = fp.from_float(3),
                        y = fp.from_float(3)
                },
                {
                        x = fp.from_float(1.5),
                        y = fp.from_float(3)
                }
        };

        livnet.dispatch("response", function(data, sz, source, ...)
                handle_event();
                update();
                sdl.set_render_draw_color(renderer, 0, 0, 0, 255);
                sdl.render_clear(renderer);

                local n, contact = collision2df.polygons(a_vertices, b_vertices);
                if n > 0 then
                        livnet.log("ok:");
                        sdl.set_render_draw_color(renderer, 255, 0, 0, 255);
                else
                        
                        livnet.log("no ok");
                        sdl.set_render_draw_color(renderer, 0, 255, 0, 255);
                end

                local a_vertices_r = {};
                for index, value in ipairs(a_vertices) do
                        table.insert(a_vertices_r, {
                                x = fp.to_float(value.x) * PIXELS_PER_METER,
                                y = fp.to_float(value.y) * PIXELS_PER_METER
                        });
                end

                local b_vertices_r = {};
                for index, value in ipairs(b_vertices) do
                        table.insert(b_vertices_r, {
                                x = fp.to_float(value.x) * PIXELS_PER_METER,
                                y = fp.to_float(value.y) * PIXELS_PER_METER
                        });
                end

                
                graphics.polygon(renderer, graphics_buffer, a_vertices_r, 2);
                graphics.polygon(renderer, graphics_buffer, b_vertices_r, 2);

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
                livnet.log("collision2df test service exit");
        end);
end);
