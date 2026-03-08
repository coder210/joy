local sproto = requirex "sproto"

local app = {};
app.start = function (ctx)
        app.ctx = ctx;
        core.log(app.ctx, "server start");

        
        local protocol_str = [[
        .Player {
        id 0 : integer
        name 1 : string
        level 2 : integer
        }

        getPlayer 1 {
        request {
                player_id 0 : integer
        }
        response {
                player 0 : Player
        }
        }
        ]]

        local sp = sproto.parse(protocol_str);

        -- 创建 Player 对象
        local player = {
                player_id = 1001,
                name = "Alice",
                position = {100, 200, 0},
                health = 100,
                mana = 50
        }

        -- 序列化为二进制
        local binary_data = sp:encode("Player", player)
        core.log(app.ctx, "Encoded data size:", #binary_data)

        -- 编码请求数据
        local login_request = {
                username = "alice123",
                password = "secret"
        }
        local login_binary = sp:encode("login.request", login_request)

end

app.event = function (ev)
        if keyboard.is_down(ev, "Q") then
                core.quit(app.ctx);
        elseif keyboard.is_down(ev, "ESCAPE") then
                core.quit(app.ctx);
        end
end

app.update = function ()

end

app.destroy = function ()
        core.log(app.ctx, "test exit");
end

return app;