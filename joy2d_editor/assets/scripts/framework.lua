local livnet = requirex "livnet"
local ecs = livnet.ecs;
local constant = livnet.constant;

local program = {};

local window, renderer;
program.start = function(ctx)
        local ok;
        livnet.log("framework");

        sdl.set_app_metadata("framework", "1.0", "com.livnet.framework");
        ok, window, renderer = sdl.create_window_and_renderer("framework", 960, 720, 32);
        assert(ok);

        ecs:register_type("position", function()
                return { x = 0, y = 0 };
        end);
        ecs:register_type("hp", function()
                return { hp = 0 };
        end);

        program.ctx = ctx;

        local entity1 = ecs:spawn("position", "hp");
        local position = ecs:get_component(entity1, "position");
        position.x = 12;
        position.y = 20;

        local hp = ecs:get_component(entity1, "hp");
        hp.hp = 10;

        --ecs:remove_component(entity1, "hp");

        ecs:add_system("t1", function()
                local entityies = ecs:query("position");
                if #entityies > 0 then
                        position = ecs:get_component(entityies[1], "position");
                        livnet.log("<" .. position.x .. "," .. position.y .. ">");
                end
        end);

        ecs:add_system("t2", function()
                local entityies = ecs:query("position", "hp");
                if #entityies > 0 then
                        hp = ecs:get_component(entityies[1], "hp");
                        livnet.log("hp:" .. hp.hp);
                end
        end);
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
        ecs:update();
        sdl.set_render_draw_color(renderer, 255, 0, 0, 255);
        sdl.render_clear(renderer);
        sdl.render_present(renderer);
end


program.destroy = function()
        sdl.destroy_renderer(renderer);
        sdl.destroy_window(window);
        livnet.log("framework service exit");
end

return program;
