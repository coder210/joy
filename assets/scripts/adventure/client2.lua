local app = {};

app.keycodes = {
    NONE = 0x0,
    UP = 0x1,
    DOWN = 0x2,
    LEFT = 0x3,
    RIGHT = 0x4,
    ENTER = 0xD,
    ESC = 0x1B,
    SPACE = 0x20
}

app.convert_to_codepoints = function(str)
    local codepoints = {};
    for pos, char in utf8.codes(str) do
        table.insert(codepoints, char);
    end
    return codepoints;
end

app.grad_size = 50;

app.fonts = {};
app.texts = {};
app.buttons = {};


ecs.setex = function(world, entity_id, typename, json_object)
    ecs.set(world, entity_id, typename, cjson.encode(json_object));
end


local init_ui = function()
    app.fonts.simhei_font = graphics.create_font(app.renderer, "resources/fonts/simhei.ttf", app.grad_size * 0.4);
    app.texts.hello_text = graphics.create_text(app.fonts.simhei_font, app.convert_to_codepoints("上"), 255, 255, 255,
        255);
    app.texts.fps_text = graphics.create_text(app.fonts.simhei_font, app.convert_to_codepoints("fps:"), 255, 255, 255, 255);

    app.buttons.move_up_btn = ui.button.create(app.renderer, app.grad_size, (app.map_size.y - 3) * app.grad_size,
        app.grad_size, app.grad_size);
    ui.button.set_text(app.buttons.move_up_btn, app.fonts.simhei_font, app.convert_to_codepoints("上"), 0, 255, 0, 255);

    app.buttons.move_down_btn = ui.button.create(app.renderer, app.grad_size, (app.map_size.y - 1) * app.grad_size,
        app.grad_size, app.grad_size);
    ui.button.set_text(app.buttons.move_down_btn, app.fonts.simhei_font, app.convert_to_codepoints("下"), 0, 255, 0,
        255);

    app.buttons.move_left_btn = ui.button.create(app.renderer, 0, (app.map_size.y - 2) * app.grad_size, app.grad_size,
        app.grad_size);
    ui.button.set_text(app.buttons.move_left_btn, app.fonts.simhei_font, app.convert_to_codepoints("左"), 0, 255, 0,
        255);

    app.buttons.move_right_btn = ui.button.create(app.renderer, 2 * app.grad_size, (app.map_size.y - 2) * app.grad_size,
        app.grad_size, app.grad_size);
    ui.button.set_text(app.buttons.move_right_btn, app.fonts.simhei_font, app.convert_to_codepoints("右"), 0, 255, 0,
        255);

    app.left_joystick = joystick.create(app.renderer, 20, 20, 40);
end

local function handle_player_joins(game_world, world, player_joins)
    if not player_joins then
        return;
    end
    for _, player_join in ipairs(player_joins) do
        local body = world2df.create_rigidbody(world, mathx.from_float(1.0), mathx.from_float(1.0));
        local entityid = rigidbody.get_id(body);
        rigidbody.set_position(body, player_join.position_x, player_join.position_y);
        world2df.add_connection(world, player_join.conv, entityid);

        local entity_id = ecs.spawn(app.game_world);
        ecs.setex(app.game_world, entity_id, "position", {
            x = mathx.to_float(player_join.position_x),
            y = mathx.to_float(player_join.position_y)
        });
        ecs.setex(app.game_world, entity_id, "size", {
            x = 1.0,
            y = 1.0
        });
        ecs.setex(app.game_world, entity_id, "conv", {
            conv = player_join.conv
        });
    end
end

local function handle_player_leaves(world, player_leaves)
    if not player_leaves then
        return;
    end
    for _, player_leave in ipairs(player_leaves) do
        local ok, entityid = world2df.get_entity_id(world, player_leave.conv);
        if ok then
            world2df.remove_connection(world, player_leave.conv);
            world2df.destroy_rigidbody(world, entityid);
        end
    end
end

local function handle_creating_emenies(world, creating_emenies)
    if not creating_emenies then
        return;
    end
    for _, creating_emeny in ipairs(creating_emenies) do
        local body = world2df.create_rigidbody(world, creating_emeny.width, creating_emeny.height);
        rigidbody.set_linear_velocity(body, creating_emeny.linear_velocity_x, creating_emeny.linear_velocity_y);
        rigidbody.set_position(body, creating_emeny.position_x, creating_emeny.position_y);
        world2df.add_emeny(world, rigidbody.get_id(body));
    end
end


local message = function(data)
    local t = s2c.deserialize(data);
    if t.cmd == 3 then
        core.log(app.ctx, "S2C_CMD_LOADING:" .. t.frame_id);
        if t.ok then
            if not client.cmd_loading1(app.client, app.world, t.ok, t.data, t.frame_id) then
                return;
            end

            -- 判断添加了玩家,敌人的数量
            world2df.foreach_connection(app.world, function(conv, body_id)
                local body = world2df.get_rigidbody(app.world, conv);
                local x, y = rigidbody.get_position(body);
                local width = rigidbody.get_width(body);
                local height = rigidbody.get_height(body);

                local entity_id = ecs.spawn(app.game_world);
                ecs.setex(app.game_world, entity_id, "position", {
                    x = x,
                    y = y
                });
                ecs.setex(app.game_world, entity_id, "size", {
                    x = width,
                    y = height
                });
                ecs.setex(app.game_world, entity_id, "conv", {
                    conv = conv
                });
            end);

            local str = client.create_player_join(1.2, 2.3);
            net.kcpclient.send(app.kcpclient, str, string.len(str));

            -- 保存第一帧
            local world_data = world2df.serialize(app.world);
            local world_checksum = world2df.checksum(app.world);
            client.add_world(app.client, t.frame_id, world_data);
            core.log(app.ctx, "cmd_loading:" .. world_checksum);
        else
            local str = client.cmd_loading2(app.client, t.ok, t.data);
            if str then
                net.kcpclient.send(app.kcpclient, str, string.len(str));
            end
        end
    elseif t.cmd == 4 then
        core.log(app.ctx, "==================frame_id:" .. t.frame_id .. "===================");
        core.log(app.ctx, cjson.encode(t));
        client.set_server_frameid(app.client, t.frame_id);

        local world_checksum = world2df.checksum(app.world);

        if t.checksum == world_checksum then
            core.log(app.ctx, "S2C_CMD_COMMAND:" .. t.checksum .. ":" .. world_checksum);
        else
            core.error(app.ctx, "S2C_CMD_COMMAND:" .. t.checksum .. ":" .. world_checksum);
        end

        if #t.player_leaves > 0 then
            handle_player_leaves(app.world, t.player_leaves);
        end
        if #t.player_joins > 0 then
            handle_player_joins(app.game_world, app.world, t.player_joins);
        end
        if #t.creating_emenies > 0 then
            handle_creating_emenies(app.world, t.creating_emenies);
        end

        if #t.player_inputs > 0 then
            -- 保存服务器帧
            client.add_command(app.client, t.player_inputs);
        end
        
        
        -- local world_data = world2df.serialize(app.world);
        -- local world_checksum = world2df.checksum(app.world);
        -- client.add_world(app.client, t.frame_id, world_data);


        --是否要回滚t.frameid
        local local_inputs = client.get_local_inputs(app.client, t.frame_id);
        if #t.player_inputs ~= #local_inputs then
            core.error(app.ctx, "不同:" .. cjson.encode(t.player_inputs) .. "|" .. cjson.encode(local_inputs));
            client.rollback(app.client, app.world, t.frame_id);
            return;
        else
            for _, server_input in ipairs(t.player_inputs) do
                for _, local_input in ipairs(local_inputs) do
                    if server_input.conv == local_input.conv then
                        if server_input.keycode ~= local_input.keycode then
                            core.error(app.ctx, "不同:" .. cjson.encode(t.player_inputs) .. "|" .. cjson.encode(local_inputs));
                            client.rollback(app.client, app.world, t.frame_id);
                            return;
                        end
                    end
                end
            end
            core.log(app.ctx, "相同" .. cjson.encode(t.player_inputs) .. "|" .. cjson.encode(local_inputs));
            return;
        end
        


        -- world2df.update_emeny(app.world, mathx.from_float(app.map_size.x), mathx.from_float(app.map_size.y));
    end

end

local test = function()
    local result, msg = net.kcpclient.poll(app.kcpclient);
    if result then
        if msg.type == 0 then
            local ok, conv = net.kcpclient.get_conv(app.kcpclient);
            core.log(app.ctx, "kcp eventcb connected:" .. conv);
            local str = c2s.serialize_ready();
            net.kcpclient.send(app.kcpclient, str, string.len(str));
        elseif msg.type == 1 then
            local ok, conv = net.kcpclient.get_conv(app.kcpclient);
            core.log(app.ctx, "disconnected:" .. conv);
        elseif msg.type == 2 then
            -- sdl.log("etype:" .. event.data);
            message(msg.data);
        end
    end
end

app.start = function(ctx)
    local ok, width, height;
    app.ctx = ctx;
    core.log(app.ctx, "client start");
    sdl.set_app_metadata("server2d", "1.0", "com.livnet.client2d");
    app.map_size = {
        x = 8,
        y = 6
    };
    ok, app.window, app.renderer = window.create_with_renderer("client2d", app.map_size.x * app.grad_size,
        app.map_size.y * app.grad_size, 32);
    window.set_icon(app.window, "./resources/textures/livnet.bmp");
    graphics.set_logical_presentation(app.renderer, app.map_size.x * app.grad_size, app.map_size.y * app.grad_size, 1);
    ok, width, height = graphics.get_logical_presentation(app.renderer);
    app.camera = {
        x = width,
        y = height
    };

    -- ecs
    app.game_world = ecs.create();
    ecs.define(app.game_world, "position", 256);
    ecs.define(app.game_world, "velocity", 256);
    ecs.define(app.game_world, "size", 256);
    ecs.define(app.game_world, "conv", 256);

    local ip = core.get_env(app.ctx, "ip");
    local port = math.tointeger(core.get_env(app.ctx, "port"));
    app.world = world2df.create();
    app.kcpclient = net.kcpclient.create(ip, port);
    app.client = client.create();
    app.keycode = app.keycodes.NONE;

    app.fps = profiler.simple_fps.create();

    init_ui();

    -- move
    ecs.register(app.game_world, function(game_world, dt)
        local entities = ecs.query(game_world, {"position", "conv"});
        for i = 1, #entities do
            local position_str = ecs.get(game_world, entities[i], "position");
            local conv_str = ecs.get(game_world, entities[i], "conv");
            local position = cjson.decode(position_str);
            local conv = cjson.decode(conv_str);

            -- core.log(app.ctx, "position_str:" .. position_str);

            local body = world2df.get_rigidbody(app.world, conv.conv);
            if body then
                local x, y = rigidbody.get_position(body);
                position = vec2.lerp(position, {
                    x = x,
                    y = y
                }, dt * 10);
            end
            ecs.setex(game_world, entities[i], "position", {
                x = position.x,
                y = position.y
            });
        end
    end);

    ecs.register(app.game_world, function(game_world, dt)
        local entities = ecs.query(game_world, {"position", "size"});
        for i = 1, #entities do
            local position_str = ecs.get(game_world, entities[i], "position");
            local size_str = ecs.get(game_world, entities[i], "size");
            local position = cjson.decode(position_str);
            local size = cjson.decode(size_str);
            graphics.rectangle(app.renderer, "fill", position.x * app.grad_size, position.y * app.grad_size,
                size.x * app.grad_size, size.y * app.grad_size);
        end
    end);

    app.my_timer = timer.create(66, function(dt, interval)
        

        local ok, conv = net.kcpclient.get_conv(app.kcpclient);
        if not ok then
            return;
        end

        if not client.is_ready(app.client) then
            return;
        end
      

        if app.keycode == app.keycodes.NONE then
            --return;
        end

        -- 本地预测
        client.predict(app.client, app.world, conv, app.keycode);

        -- 同步到服务器
        local str = c2s.serialize_player_input(app.keycode);
        net.kcpclient.send(app.kcpclient, str, string.len(str));
    end);

    app.heartbeat_timer = timer.create(3000, function(dt, interval)
        local ok, conv = net.kcpclient.get_conv(app.kcpclient);
        if not ok then
            return;
        end
        local str = c2s.serialize_heartbeat();
        net.kcpclient.send(app.kcpclient, str, string.len(str));
    end);
end

app.event = function(event)
    if keyboard.is_down(event, "Q") then
        core.quit(app.ctx);
    elseif keyboard.is_down(event, "ESCAPE") then
        core.quit(app.ctx);
    elseif keyboard.is_down(event, "A") then
        -- core.quit(app.ctx);
        -- kcpclient.send(app.kcpclient, "hello, server", string.len("hello, server"));
    elseif keyboard.is_down(event, "UP") then
        app.keycode = app.keycodes.UP;
    elseif keyboard.is_down(event, "RIGHT") then
        app.keycode = app.keycodes.RIGHT;
    elseif keyboard.is_down(event, "DOWN") then
        app.keycode = app.keycodes.DOWN;
    elseif keyboard.is_down(event, "LEFT") then
        app.keycode = app.keycodes.LEFT;
    elseif keyboard.is_down(event, "SPACE") then
        app.keycode = app.keycodes.SPACE;
    elseif keyboard.is_keyup(event) then
        app.keycode = app.keycodes.NONE;
    end

    joystick.handle_event(app.left_joystick, event);

    ui.button.listen(app.buttons.move_up_btn, event);
    ui.button.listen(app.buttons.move_down_btn, event);
    ui.button.listen(app.buttons.move_left_btn, event);
    ui.button.listen(app.buttons.move_right_btn, event);

    if ui.button.is_clicked(app.buttons.move_up_btn) then
        app.keycode = app.keycodes.UP;
    elseif ui.button.is_clicked(app.buttons.move_down_btn) then
        app.keycode = app.keycodes.DOWN;
    elseif ui.button.is_clicked(app.buttons.move_left_btn) then
        app.keycode = app.keycodes.LEFT;
    elseif ui.button.is_clicked(app.buttons.move_right_btn) then
        app.keycode = app.keycodes.RIGHT;
    end


end

local draw_ui = function()
    ui.button.draw(app.buttons.move_up_btn);
    ui.button.draw(app.buttons.move_down_btn);
    ui.button.draw(app.buttons.move_left_btn);
    ui.button.draw(app.buttons.move_right_btn);
    joystick.draw(app.left_joystick);
end


app.update = function(dt)
    local fps = profiler.simple_fps.update(app.fps);
    net.kcpclient.update(app.kcpclient);
    test();

    timer.trigger(app.my_timer);
    timer.trigger(app.heartbeat_timer);

    graphics.set_color(app.renderer, 0, 0, 0, 255);
    graphics.clear(app.renderer);
    graphics.set_color(app.renderer, 128, 128, 128, 255);

    ecs.process(app.game_world, dt);


    local server_frame_id, local_frame_id = client.get_frame_id(app.client);
    local str = app.convert_to_codepoints("中fps:" .. fps .. " l:" .. local_frame_id .. " s:" .. server_frame_id);
    graphics.update_text(app.texts.fps_text, app.fonts.simhei_font, str, 255, 255, 255, 255);
    graphics.print_text(app.renderer, app.texts.fps_text, 100, 0);

    -- world2df.foreach_body(app.world, function (body)
    --         --core.log(app.ctx, "body:" .. rigidbody.tostring(body));
    --         local x, y = rigidbody.get_position(body);
    --         local width = rigidbody.get_width(body) * 50;
    --         local height = rigidbody.get_height(body) * 50;
    --         graphics.rectangle(app.renderer, "fill", x * 50, y * 50, width, height);
    -- end);

    draw_ui();

    graphics.present(app.renderer);
end

app.destroy = function()
    ecs.destroy(app.game_world);
    timer.destroy(app.heartbeat_timer);
    timer.destroy(app.my_timer);
    ui.button.destroy(app.buttons.move_up_btn);
    world2df.destroy(app.world);
    graphics.destroy(app.renderer);
    window.destroy(app.window);
    net.kcpclient.destroy(app.kcpclient);
    client.destroy(app.client);
    profiler.simple_fps.destory(app.fps);
    core.log(app.ctx, "client destroy");
end

return app;
