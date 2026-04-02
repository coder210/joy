local app = {};

app.start = function (ctx)
        app.ctx = ctx;
end

app.event = function (ev)
        local event = sdl.get_event(ev);
        if event.type == 768 then
                --print("lua scancode " .. event.key.scancode);
                core.quit(app.ctx);
        end
        core.log(app.ctx, "lua event type:" .. event.type);
end

app.update = function ()
        core.log(app.ctx, "test lua update");
end

app.destroy = function ()
        
end

return app;