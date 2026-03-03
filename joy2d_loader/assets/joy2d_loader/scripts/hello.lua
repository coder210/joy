local livnet = requirex "livnet"

local app = {}


app.start = function(ctx)
    livnet.log("hello world");
end

app.event = function()

end

app.update = function()
end

app.destroy = function()
end

return app;
