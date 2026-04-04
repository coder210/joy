local app = {};
app.start = function (ctx)
        local ok, width, height;
        app.ctx = ctx;
        core.log(app.ctx, "server start");
        sdl.set_app_metadata("server2d", "1.0", "com.livnet.server2d");
        ok, app.window, app.renderer = window.create_with_renderer("server2d", 800, 600, 32);
        window.set_icon(app.window, "./resources/textures/livnet.bmp");
        graphics.set_logical_presentation(app.renderer, 800, 600, 1);
        ok, width, height = graphics.get_logical_presentation(app.renderer);
        app.camera = { x = width, y = height, };

        app.audio_device_id = audio.open_device();
        local helicopter = audio.create_wav("resources/sounds/helicopter.wav", app.audio_device_id);
        --local zombie = audio.create_ogg("resources/sounds/zombie.ogg", app.audio_device_id);

        app.image1 = graphics.new_image(app.renderer, "./resources/textures/livnet.png");
        app.spritebatch = graphics.new_spritebatch(app.renderer, "./resources/textures/livnet.png");

        graphics.set_color(app.renderer, 1, 1, 1, 1);
        
        for i = 0, 0 do
                local y = i * 64;
                for j = 0, 0 do
                        local x = j * 64 + 64;
                        local src_rect = {x=0,y=0,w=64,h=64};
                        local dest_rect = {x=x,y=y,w=64,h=64};
                        local origin = {x=dest_rect.x + dest_rect.w / 2.0, y= dest_rect.y + dest_rect.h / 2.0 };
                        --graphics.addSpriteEx(app.spritebatch, src_rect, dest_rect, 45, origin);
                        graphics.add_sprite(app.spritebatch, src_rect, {x=10, y=10}, 45, 1, 1, origin)
                end
        end

        app.my_timer = timer.create(66, function(dt, interval)
                core.log(app.ctx, string.format("Timer triggered! dt: %d, interval: %d", dt, interval))
               
        end)
end

app.event = function (ev)
        if keyboard.is_down(ev, "Q") then
                core.quit(app.ctx);
        elseif keyboard.is_down(ev, "ESCAPE") then
                core.quit(app.ctx);
        end
end

app.update = function ()
        timer.trigger(app.my_timer);

        graphics.set_color(app.renderer, 0, 0, 0, 1);
        graphics.clear(app.renderer);

        graphics.set_color(app.renderer, 1, 0, 0, 1);
        graphics.point(app.renderer, 150, 150);
        graphics.points(app.renderer, {{x=340, y=340},{x=350, y=350},{x=450, y=450}});
        graphics.line(app.renderer, 0, 0, 100, 100);
        graphics.rectangle(app.renderer, "line", 100, 100, 20, 20);
        graphics.rectangle(app.renderer, "fill", 200, 200, 20, 20);
        graphics.polygon(app.renderer, "line", 400, 100, 200, 500, 600, 500);
        graphics.polygon(app.renderer, "line", 100, 100, 300, 100, 300, 200, 100, 200);
        graphics.polygon(app.renderer, "fill", 400, 100, 600, 100, 600, 200, 400, 200);
        -- 3. 旋转的矩形（菱形）
        graphics.polygon(app.renderer, "line", 400, 75, 450, 125, 400, 175, 350, 125);
        -- 4. 梯形
        graphics.polygon(app.renderer, "line", 500, 50, 550, 50, 525, 100, 475, 100);
        -- 5. 复杂多边形（房子形状）
        graphics.polygon(app.renderer, "line", 100, 200, 150, 200, 150, 250, 125, 275, 100, 250);
         -- 绘制线框圆
        graphics.circle(app.renderer, "line", 100, 100, 50) -- 圆心(100,100)，半径50
        -- 绘制填充圆
        graphics.circle(app.renderer, "fill", 200, 100, 50)
        
        --graphics.grid(app.renderer, {x=0,y=0}, {x=1000, y=1000}, 50);
        graphics.set_color(app.renderer, 1, 1, 1, 1);
        graphics.gridx(app.renderer, {x=10,y=10}, 10, 10, 50);
        
        graphics.draw_image(app.image1, 200, 200);
        graphics.draw_spritebatch(app.spritebatch);

        graphics.present(app.renderer);
end

app.destroy = function ()
        audio.close_device(app.audio_device_id);
        graphics.free_image(app.image1);
        graphics.free_spritebatch(app.spritebatch);
        graphics.destroy(app.renderer);
        timer.destroy(app.my_timer);
        window.destroy(app.window);
        core.log(app.ctx, "graphics destroy");
end

return app;