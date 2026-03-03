local livnet = requirex "livnet"
local game = requirex "game"
local map = requirex "map"


local PIXELS_PER_METER = 100;
local SDL_EVENT_KEY_DOWN = 768;
local SDL_SCANCODE_ESCAPE = 41;
local SDL_SCANCODE_Q = 41;
local SDL_SCANCODE_RIGHT = 79;
local SDL_SCANCODE_UP = 82;
local SDL_SCANCODE_LEFT = 80;
local SDL_SCANCODE_DOWN = 81;

local window, renderer;

local camera = {};
camera.position = {x=-100,y=-100};

local function exit_all()
        livnet.exit();
end

local function handle_keydown_event(scancode)
        if scancode == SDL_SCANCODE_Q then
                exit_all();
        elseif scancode == SDL_SCANCODE_ESCAPE then
                exit_all();
        elseif scancode == SDL_SCANCODE_UP then
                camera.position.y = camera.position.y - 10;
        elseif scancode == SDL_SCANCODE_RIGHT then
                camera.position.x = camera.position.x + 10;
        elseif scancode == SDL_SCANCODE_DOWN then
                camera.position.y = camera.position.y + 10;
        elseif scancode == SDL_SCANCODE_LEFT then
                camera.position.x = camera.position.x - 10;
        end
end


livnet.start(function()
        livnet.log("ui");

        livnet.alias("tilemap");

        local ok, width, height;

        sdl.set_app_metadata("tilemap", "1.0", "com.livnet.tilemap");
        ok, window, renderer = sdl.create_window_and_renderer("tilemap", 1024, 760);
        assert(ok);

        livnet.log(map.luaversion);
        livnet.log("width:" .. map.width * map.tilewidth);
        livnet.log("height:" .. map.height * map.tileheight);


        ok, width, height = sdl.get_window_size(window);

        camera.size = {x=width,y=height};

        livnet.dispatch("sdl_event", function(data, sz, source, ...)
                local etype = sdl.get_event_type(data);
                local scancode = sdl.get_event_scancode(data);
                if etype == SDL_EVENT_KEY_DOWN then
                        handle_keydown_event(scancode);
                end
        end)

        game.load_tilemap(map);

        local last_time = sdl.get_ticks();
        local total_time = 0;
        local dt = 0;
        local frame_count = 0;
        local fps = 0;
        livnet.dispatch("response", function(data, sz, source, ...)
                sdl.set_render_draw_color(renderer, 71, 171, 169, 255);
                sdl.render_clear(renderer);

                local current_time = sdl.get_ticks();
                dt = current_time - last_time;
                total_time = total_time + dt;
                if total_time >= 1000 then
                        fps = frame_count;
                        frame_count = 0;
                        total_time = total_time - 1000;
                else
                        frame_count = frame_count + 1;
                end

                last_time = current_time;

                sdl.set_render_draw_color(renderer, 0, 0, 0, 255);
                local str = string.format("fps:%d", fps);
                sdl.render_debug_text(renderer, 0, 0, str);

                
                sdl.set_render_draw_color(renderer, 255, 0, 0, 255);
                local current_time = utils.current_time();

                game.render_tilemap(camera, renderer, current_time);

                sdl.render_present(renderer);
                livnet.timeout(1);
                --core.sendname("tilemap", livnet.PTYPE_RESPONSE, "");
        end);
        livnet.timeout(1);
        --core.sendname("tilemap", livnet.PTYPE_RESPONSE, "");

        livnet.exit_func(function()
                --sdl.destroy_texture(texture1);
                sdl.destroy_renderer(renderer);
                sdl.destroy_window(window);
                livnet.log("tilemap service exit");
        end);
end);
