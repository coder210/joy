local livnet = requirex "livnet"
local protocol = requirex "protocol"
local physics = requirex "physics"
local map = require "resources/map"

local constant = livnet.constant;


local context = {};

context.window = nil;
context.renderer = nil;
context.kcpc = nil;
context.opkey = constant.OPKEY_NONE;

context.frames = {};
context.frameid = 1;
context.ready = false;
context.first_frame = "";

context.fps = 0;


local function check_health(inst, dt)
        local ok, conv = kcpclient.get_conv(context.kcpc);
        if ok then
                livnet.call.invoke(inst, dt, function()
                        local str = protocol.keep_health(conv);
                        kcpclient.send(context.kcpc, str, string.len(str));
                end);
        end
end

local camera_follow = function()
        local ok, conv = kcpclient.get_conv(context.kcpc);
        if not ok then
                return;
        end
        local entity = physics.find_entity(conv);
        if entity then
                local object = livnet.world.get_object(entity.userdata.gid);
                local rect = {
                        x = object.transform.position.x * livnet.constant.PIXELS_PER_METER;
                        y = object.transform.position.y * livnet.constant.PIXELS_PER_METER;
                        width = object.transform.size.x * livnet.constant.PIXELS_PER_METER;
                        height = object.transform.size.y * livnet.constant.PIXELS_PER_METER;
                };
                livnet.camera.position.x = rect.x - (livnet.camera.size.x - rect.width) / 2;
                livnet.camera.position.y = rect.y - (livnet.camera.size.y - rect.height) / 2;
        end
end



local function handle_player_cmds(player_cmds)
        if not player_cmds then
                return;
        end
        for _, player_cmd in ipairs(player_cmds) do
                local entity = physics.find_entity(player_cmd.cn);
                --player_command(entity, player_cmd.code);
                local entity_id = entity.userdata.gid;
                local player = livnet.world.get_object(entity_id);
                player:command(player_cmd.code);
        end
end

local function handle_frame(entities, players)
        if not entities then
                return;
        end
        for _, entity in ipairs(entities) do
                if entity.type == physics.TYPE_PLAYER then
                        local player = Player.new(entity.id);
                        player:set_position(entity.px, entity.py);
                        player:set_size(entity.w, entity.h);
                        player.entity.linear_velocity.x = entity.lvx;
                        player.entity.linear_velocity.y = entity.lvy;
                        player.entity.rotation = entity.rot;
                elseif entity.type == physics.TYPE_BULLET then
                        --prefab.bullet2(entity);
                end
        end

        if not players then
                return;
        end

        for _, player in ipairs(players) do
                local entity_id = math.tointeger(player.eid);
                local entity = physics.get_entity(entity_id);
                if entity then
                        physics.add_player(player.cn, entity_id);
                end
        end
end


local function handle_player_joins(player_joins)
        if not player_joins then
                return;
        end
        for _, player_join in ipairs(player_joins) do
                local player = Player.new();
                player:set_position(player_join.px, player_join.py);
                physics.add_player(player_join.cn, player.entity.id);
        end
end


local function handle_player_leaves(player_leaves)
        if not player_leaves then
                return;
        end
        for _, player_leave in ipairs(player_leaves) do
                local entity = physics.find_entity(player_leave.cn);
                if entity then
                        livnet.world.remove_object(entity.userdata.gid);
                        physics.remove_player(player_leave.cn);
                end
        end
end


local execute_command = function(frame)
        context.frameid = frame.id;
        local checksum = physics.get_checksum();
        handle_player_joins(frame.pjs);
        handle_player_leaves(frame.pls);
        handle_player_cmds(frame.pcs);
        if frame.cs == checksum then
                --sdl.log("frameid=" .. frame.id .. " checksum=true");
        else
                sdl.log("frameid=" .. frame.id .. frame.cs .. ":" .. checksum);
        end
        --physics.move();
        --physics.collide();
end

local handle_load_scene = function(t)
        if context.ready then
                return;
        end
        if t.ok then
                context.ready = t.ok
                if t.data ~= "" then
                        context.first_frame = context.first_frame .. t.data;
                        local frame = cjson.decode(context.first_frame);
                        physics.set_entity_id(math.tointeger(frame.eid));
                        handle_frame(frame.entities, frame.players);
                        execute_command(frame);
                        context.first_frame = "";
                end
                local ok, conv = kcpclient.get_conv(context.kcpc);
                local str = protocol.player_join(conv, 1.2, 2.3);
                kcpclient.send(context.kcpc, str, string.len(str));
        else
                context.first_frame = context.first_frame .. t.data;
                local str = protocol.load_scene(t.conv, "", context.ready);
                kcpclient.send(context.kcpc, str, string.len(str));
        end
end


--------------------------------------------------------------------------------
-------------------------------------------------------------------------------
---
local program = {};

program.start = function(ctx)
        sdl.log("client");

        local ip = core.get_env(ctx, "ip");
        local port = math.tointeger(core.get_env(ctx, "port"));

        program.ctx = ctx;
        program.check_health_inst = livnet.call.new(3000);

        context.kcpc = kcpclient.create(ip, port);
        --context.sprites = sprite_batch.create(1024);

        program.inst = livnet.call.new(66);

        program.last_time = sdl.get_ticks();
        program.dt = 0;
        program.total_time = 0;
        program.frame_count = 0;

        local ok, width, height;
        sdl.set_app_metadata("client2d", "1.0", "com.livlivnet.client2d");
        ok, context.window, context.renderer = sdl.create_window_and_renderer("client2d", 720, 480, 32);
        sdl.set_render_logical_presentation(context.renderer, 720, 480, 1);
        assert(ok);
        ok, width, height = sdl.get_render_logical_presentation(context.renderer);
        livnet.camera.size = {
                x = width,
                y = height,
        };

        --livnet.load_tilemap(context.renderer, map);

        sdl.log(string.format("<%d,%d>", livnet.camera.size.x, livnet.camera.size.y));
end

program.event = function (event)
        local lua_event = sdl.get_event(event);
        local etype = lua_event.type;
        if etype == livnet.constant.SDL_EVENT_KEY_DOWN then
                if lua_event.key.scancode == livnet.constant.SDL_SCANCODE_Q then
                        core.quit(program.ctx);
                elseif lua_event.key.scancode == livnet.constant.SDL_SCANCODE_ESCAPE then
                        core.quit(program.ctx);
                elseif lua_event.key.scancode == livnet.constant.SDL_SCANCODE_UP then
                        context.opkey = livnet.constant.OPKEY_UP;
                elseif lua_event.key.scancode == livnet.constant.SDL_SCANCODE_RIGHT then
                        context.opkey = livnet.constant.OPKEY_RIGHT;
                elseif lua_event.key.scancode == livnet.constant.SDL_SCANCODE_DOWN then
                        context.opkey = livnet.constant.OPKEY_DOWN;
                elseif lua_event.key.scancode == livnet.constant.SDL_SCANCODE_LEFT then
                        context.opkey = livnet.constant.OPKEY_LEFT;
                elseif lua_event.key.scancode == livnet.constant.SDL_SCANCODE_SPACE then
                        context.opkey = livnet.constant.OPKEY_SHOOT;
                end
        else if etype == livnet.constant.SDL_EVENT_KEY_UP then
                        context.opkey = livnet.constant.OPKEY_NONE;
                end
        end
end

program.update = function()
        local current_time = sdl.get_ticks();
        program.dt = current_time - program.last_time;
        kcpclient.update(context.kcpc);
        check_health(program.check_health_inst, program.dt);

        livnet.call.invoke(program.inst, program.dt, function(dt, interval)
                local result, msg = kcpclient.poll_message(context.kcpc);
                if result then
                        if msg.type == 0 then
                                local ok, conv = kcpclient.get_conv(context.kcpc);
                                sdl.log("kcp eventcb connected:" .. conv);
                                local str = protocol.ready(conv);
                                kcpclient.send(context.kcpc, str, string.len(str));
                        elseif msg.type == 1 then
                                local ok, conv = kcpclient.get_conv(context.kcpc);
                                sdl.log("disconnected:" .. conv);
                        elseif msg.type == 2 then
                                --sdl.log("etype:" .. event.data);
                                local frame = cjson.decode(msg.data);
                                if frame.cmd == constant.CMD_LOAD_SCENE then
                                        handle_load_scene(frame);
                                elseif frame.cmd == constant.CMD_FRAME then
                                        execute_command(frame);
                                end
                        end
                end

                local kcpc = context.kcpc;
                if not context.ready then
                        return;
                end

                -- if context.opkey == constant.OPKEY_NONE then
                --         return;
                -- end

                local ok, conv = kcpclient.get_conv(kcpc);
                if not ok then
                        return;
                end

                local str = protocol.player_command(conv, context.opkey);
                kcpclient.send(kcpc, str);
                --context.opkey = constant.OPKEY_NONE;
        end);

        livnet.world.update(program.dt);

        
        camera_follow();

        program.total_time = program.total_time + program.dt;
        if program.total_time >= 1000 then
                context.fps = program.frame_count;
                program.frame_count = 0;
                program.total_time = program.total_time - 1000;
        else
                program.frame_count = program.frame_count + 1;
        end

        sdl.set_render_draw_color(context.renderer, 0, 92, 92, 255);
        sdl.render_clear(context.renderer);
        sdl.set_render_scale(context.renderer, 1, 1);

        --sprite_batch.render(context.sprites);
        --livnet.world.render(context.renderer, context.sprites);

        sdl.set_render_draw_color(context.renderer, 0, 0, 0, 255);
        sdl.set_render_scale(context.renderer, 2, 2);
        local str = string.format("fps:%d, frameid:%d", context.fps, context.frameid);
        sdl.render_debug_text(context.renderer, 0, 0, str);

        -- local local_player = {};
        
        -- local str = string.format("<%f,%f> hp:%02d", local_player.transform.position.x, local_player.transform.position.y, local_player.hp);
        -- sdl.render_debug_text(renderer, 0, 30, str);
        sdl.render_present(context.renderer);


        program.last_time = current_time;
end


program.destroy = function()
        kcpclient.destroy(context.kcpc);
        sdl.destroy_renderer(context.renderer);
        sdl.destroy_window(context.window);
        --sprite_batch.destroy(context.sprites);
        sdl.log("server exit");
end


return program;