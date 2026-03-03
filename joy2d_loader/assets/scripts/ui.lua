local app = {};

app.convert_to_codepoints = function (str)
        local codepoints = {};
        for pos, char in utf8.codes(str) do
                table.insert(codepoints, char);
        end
        return codepoints;
end

app.start = function (ctx)
        local ok, width, height;
        app.ctx = ctx;
        core.log(app.ctx, "ui start");
        sdl.set_app_metadata("ui", "1.0", "com.livnet.ui");
        ok, app.window, app.renderer = window.create_with_renderer("ui", 800, 600, 32);
        window.set_icon(app.window, "resources/textures/livnet.bmp");
        graphics.set_logical_presentation(app.renderer, 800, 600, 1);
        ok, width, height = graphics.get_logical_presentation(app.renderer);
        app.camera = { x = width, y = height, };

        app.audio_device_id = audio.open_device();
        local helicopter = audio.create_wav("resources/sounds/helicopter.wav", app.audio_device_id);
        --local zombie = audio.create_ogg("resources/sounds/zombie.ogg", app.audio_device_id);

        app.image1 = graphics.new_image(app.renderer, "resources/textures/livnet.png");
        app.spritebatch = graphics.new_spritebatch(app.renderer, "resources/textures/livnet.png");
        app.font = graphics.create_font(app.renderer, "resources/fonts/simhei.ttf", 20);
        app.font1 = graphics.create_font(app.renderer, "resources/fonts/simhei.ttf", 20);
        app.hello_text = graphics.create_text(app.font, app.convert_to_codepoints("开始世界!"), 255, 255, 255, 255);
        app.move_up_btn = ui.button.create(app.renderer, 100, 100, 100, 30);
        ui.button.set_text(app.move_up_btn, app.font1, app.convert_to_codepoints("上"), 255, 0, 0, 255);

        app.fps_lab = ui.label.create(app.renderer, app.font, app.convert_to_codepoints("fps"));


        graphics.set_color(app.renderer, 255, 255, 255, 255);
        
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
                --core.log(app.ctx, string.format("Timer triggered! dt: %d, interval: %d", dt, interval))
               
        end);
end

app.event = function (event)
        if window.should_close(event) then
                core.quit(app.ctx);
        end
        if keyboard.is_down(event, "Q") or keyboard.is_down(event, "ESCAPE") then
                core.quit(app.ctx);
        end
        ui.button.listen(app.move_up_btn, event);

        if ui.button.is_clicked(app.move_up_btn) then
                core.log(app.ctx, "button clicked...");
                ui.button.set_text(app.move_up_btn, app.font1, app.convert_to_codepoints("下"), 255, 0, 0, 255);
        end
end

app.update = function ()
        timer.trigger(app.my_timer);

        graphics.set_color(app.renderer, 0, 0, 0, 255);
        graphics.clear(app.renderer);

        graphics.print_text(app.renderer, app.hello_text, 10, 10);

        ui.button.draw(app.move_up_btn);
        ui.label.draw(app.fps_lab);

        graphics.present(app.renderer);
end

app.destroy = function ()
        ui.button.destroy(app.move_up_btn);
        ui.label.destroy(app.fps_lab);
        graphics.destroy_text(app.hello_text);
        graphics.destroy_font(app.font);
        audio.close_device(app.audio_device_id);
        graphics.free_image(app.image1);
        graphics.free_spritebatch(app.spritebatch);
        graphics.destroy(app.renderer);
        timer.destroy(app.my_timer);
        window.destroy(app.window);
        core.log(app.ctx, "ui destroy");
end

return app;