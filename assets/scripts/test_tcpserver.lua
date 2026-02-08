local app = {}


app.start = function(ctx)
    app.ctx = ctx;
    local ip = core.get_env(app.ctx, "ip");
    local port = math.tointeger(core.get_env(app.ctx, "port"));
    core.log(app.ctx, "tcp_server test");
    app.tcpserver = net.tcpserver.create(ip, port);

end

app.event = function(ev)

end

app.update = function(dt)
    net.tcpserver.update(app.tcpserver);
    local result, msg = net.tcpserver.poll(app.tcpserver);
    if result then
        core.log(app.ctx, msg.type .. ":" .. msg.conv);
        if msg.type == 0 then
        elseif msg.type == 2 then
            core.log(app.ctx, msg.type .. ":" .. msg.conv .. ":" .. msg.data);
            net.tcpserver.send(app.tcpserver, msg.conv, "hello from server");
        end
    end
end

app.destroy = function()
    net.tcpserver.destroy(app.tcpserver);
end

return app;