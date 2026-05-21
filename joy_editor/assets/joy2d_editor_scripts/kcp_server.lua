local livnet = requirex "livnet"
local app = {}


app.start = function(ctx)
    app.ctx = ctx;
    livnet.log("kcp_server test");
    app.kcpserver = kcpserver.create("192.168.1.19", 30000);

end

app.event = function()

end

app.update = function()
    kcpserver.update(app.kcpserver);
    local result, msg = kcpserver.poll_message(app.kcpserver);
    if result then
        livnet.log(msg.type .. ":" .. msg.conv);
        if msg.type == 0 then
        elseif msg.type == 2 then
            livnet.log(msg.type .. ":" .. msg.conv .. ":" .. msg.data);
            kcpserver.send(app.kcpserver, msg.conv, "hello from server");
        end
    end
end

app.destroy = function()
    kcpserver.destroy(app.kcpserver);
end

return app;