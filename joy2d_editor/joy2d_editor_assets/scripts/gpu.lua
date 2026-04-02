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
        local ok;
        local devices, gpu_device;

        sdl.set_app_metadata("gpu", "1.0", "com.livnet.gpu");
        ok, window, renderer = sdl.create_window_and_renderer("gpu", 720, 480);
        assert(ok);

        devices = sdl.get_gpu_devices();
        for index, value in ipairs(devices) do
                livnet.log(value);
        end

        gpu_device = sdl.create_gpu_device(true);
        sdl.claim_window_for_gpu_device(gpu_device, window);


        local vert_shader, frag_shader, pipeline;
        vert_shader = sdl.create_vertex_shader(gpu_device, "shaders/vert.spv");
        frag_shader= sdl.create_fragment_shader(gpu_device, "shaders/frag.spv");
        pipeline = sdl.create_graphics_pipeline(gpu_device, window, vert_shader, frag_shader);

        local vertices = {
                {-0.5, -0.5, 1.0, 0.0, 0.0, 0, 0},
                {0.5, -0.5, 0.0, 1.0, 0.0, 1, 0},
                {0.5, 0.5, 0.0, 0.0, 1.0, 1, 1},
                {-0.5, 0.5, 1.0, 1.0, 0.0, 0, 1},
        };
        local indices = {
                0, 1, 2, 0, 3, 2,
        };
        
        local vertex_buffer, indices_buffer, texture, sampler;
        vertex_buffer = sdl.create_and_upload_vertex_data(gpu_device, vertices);
        indices_buffer = sdl.create_and_upload_indices_data(gpu_device, indices);
        texture = sdl.create_gpu_texture(gpu_device, "textures/livnet.png");
        sampler = sdl.create_gpu_sampler(gpu_device);

        livnet.dispatch("response", function(data, sz, source, ...)
                handle_event();
                update();

                sdl.set_render_draw_color(renderer, 0, 0, 0, 255);
                sdl.render_clear(renderer);

                sdl.gpu_update(gpu_device, window, pipeline, vertex_buffer, indices_buffer, texture, sampler);
               

                sdl.render_present(renderer);
                livnet.timeout(16);
        end);
        livnet.timeout(16);

        livnet.exit_func(function()
                sdl.release_gpu_sampler(gpu_device, sampler);
                sdl.release_gpu_texture(gpu_device, texture);
                sdl.release_gpu_buffer(gpu_device, vertex_buffer);
                sdl.release_gpu_buffer(gpu_device, indices_buffer);
                sdl.release_gpu_graphics_pipeline(gpu_device, pipeline);
                sdl.release_gpu_shader(gpu_device, vert_shader);
                sdl.release_gpu_shader(gpu_device, frag_shader);
                sdl.release_window_from_gpu_device(gpu_device, window);
                sdl.destroy_gpu_device(gpu_device);

                sdl.destroy_renderer(renderer);
                sdl.destroy_window(window);
                livnet.log("gpu test service exit");
        end);
end);
