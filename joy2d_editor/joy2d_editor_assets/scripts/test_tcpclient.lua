local app = {}


app.start = function(ctx)
    app.ctx = ctx; local ip = core.get_env(app.ctx, "ip");
    local port = math.tointeger(core.get_env(app.ctx, "port"));
    core.log(app.ctx, "tcpclient test");
    app.tcpclient = net.tcpclient.create(ip, port);
end

app.event = function(ev)

end

app.update = function(dt)
    net.tcpclient.update(app.tcpclient);
    local result, msg = net.tcpclient.poll(app.tcpclient);
    if result then
        core.log(app.ctx, msg.type .. ":" .. msg.conv);
        if msg.type == 1 then
            net.tcpclient.send(app.tcpclient, "hello from client");
        elseif msg.type == 2 then
            core.log(app.ctx, msg.type .. ":" .. msg.conv .. ":" .. msg.data);
        end
    end
end

app.destroy = function()
    net.tcpclient.destroy(app.tcpclient);
end

return app;