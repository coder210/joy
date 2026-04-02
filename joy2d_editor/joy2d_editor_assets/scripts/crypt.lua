local livnet = requirex "livnet"

local constant = livnet.constant;

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


local program = {};

program.start = function (ctx)
        livnet.log("=================================");
        local str = utils.des_encrypt("1111111", "liwei");
        livnet.log(str);
        str = utils.des_decrypt("1111111", str);
        livnet.log(str);

        program.ctx = ctx;

        --加密
        livnet.log("====================");

        local key = "com.livnet.liwei";

        -- local examples = {
        --         "bootstrap",
        --         "collision_circles",
        --         "collision_polygons",
        --         "collision_ray_quad",
        --         "collision_ray_rectangle",
        --         "collision_ray_rectanglex",
        --         "collision_rectangles",
        --         "config",
        --         "crypt",
        --         "gpu",
        --         "hello",
        --         "ui",
        -- };
        -- for index, example in ipairs(examples) do
        --          ok, str = utils.encrypt_file("./scripts/example/"..example..".lua", key);
        --         livnet.log(str);

        --         ok, str = utils.decrypt_file("./scripts/example/"..example..".luas", key);
        --         livnet.log(str);
        -- end

        -- local lualibs = {"common", "keycode", "livnet", "loader", "world"};
        
        -- for _, lualib in ipairs(lualibs) do
        --         ok, str = utils.encrypt_file("./lualib/"..lualib..".lua", key);
        --         livnet.log(str);

        --         ok, str = utils.decrypt_file("./lualib/"..lualib..".luas", key);
        --         livnet.log(str);
        -- end

        local adventure_files = {
                "client",
                "client_config",
                "server",
                "server_config",
        };
        for _, t in ipairs(adventure_files) do
                ok, str = utils.encrypt_file("./scripts/adventure/"..t..".lua", key);
                livnet.log(str);

                ok, str = utils.decrypt_file("./scripts/adventure/"..t..".luas", key);
                livnet.log(str);
        end

        core.quit(ctx);
end

program.event = function (event)
        local etype = event.type;
        if etype == constant.SDL_EVENT_KEY_DOWN then
		local scancode = event.key.scancode;
                handle_keydown_event(program.ctx, scancode);
        end
end


program.update = function ()

end


program.destroy = function ()
        
end

return program;