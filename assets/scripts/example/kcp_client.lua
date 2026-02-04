local livnet = requirex "livnet"
local app = {}


app.start = function(ctx)
    livnet.log("kcp_client test");
    app.kcpclient = kcpclient.create("192.168.1.19", 30000);

end

app.event = function()

end

app.update = function()
    kcpclient.update(app.kcpclient);
    local result, msg = kcpclient.poll_message(app.kcpclient);
    if result then
        livnet.log(msg.type .. ":" .. msg.conv);
        if msg.type == 1 then
            kcpclient.send(app.kcpclient, "hello from client");
        elseif msg.type == 2 then
            livnet.log(msg.type .. ":" .. msg.conv .. ":" .. msg.data);
        end
    end
end

app.destroy = function()
    kcpclient.destroy(app.kcpclient);
end

return app;