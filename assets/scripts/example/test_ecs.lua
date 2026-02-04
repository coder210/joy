local app = {}
app.start = function (ctx)
        app.ctx = ctx;
        local kkkk = {
                x = 0.1,
                y = 0.2
        };

        local str = cjson.encode(kkkk);
        core.log(app.ctx, str);

        core.log(app.ctx, "test lua scriptx:");
        core.log(app.ctx, "float:" .. ecs.sizeoffloat());
        core.log(app.ctx, "integer:" .. ecs.sizeofinteger());
        core.log(app.ctx, "short:" .. ecs.sizeofshort());
        app.world = ecs.create();
        ecs.define(app.world, "position_t", 512);
        ecs.define(app.world, "velocity_t", 512);


        local entity_id = ecs.spawn(app.world);
        ecs.set(app.world, entity_id, "position_t", str);
        ecs.set(app.world, entity_id, "velocity_t", cjson.encode({
                x = 1.1,
                y = 1.2
        }));

        str = ecs.get(app.world, entity_id, "position_t", "velocity_t")
        core.log(app.ctx, str);

        -- move
        ecs.register(app.world, function(world)
                local entities = ecs.query(world, {"position_t", "velocity_t"});
                for i = 1, #entities do
                        local t = ecs.get(world, entities[i], "position_t");
                        local t2 = ecs.get(world, entities[i], "velocity_t");

                        core.log(app.ctx, type(t));
                        core.log(app.ctx, type(t2));

                        local position = cjson.decode(t);
                        local velocity = cjson.decode(t2);
                        position.x = position.x + 1;
                        position.y = position.y + 1;
                        velocity.x = velocity.x + 1;
                        velocity.y = velocity.y + 1;
                        core.log(app.ctx, "entity:" .. entities[i] .. " position:<"
                        .. position.x .. "," .. position.y .. ">" 
                        .. " velocity:<" .. velocity.x .. "," .. velocity.y .. ">");
                        ecs.set(world, entities[i], "position_t", cjson.encode(position));
                        ecs.set(world, entities[i], "velocity_t", cjson.encode(velocity));
                end
        end);

        core.quit(app.ctx);
end

app.event = function (ev)
        core.log(app.ctx, "lua event type");
end

app.update = function ()
        ecs.process(app.world);
end

app.destroy = function ()
        ecs.destroy(app.world);
        core.log(app.ctx, "test lua destroy");
end

return app;