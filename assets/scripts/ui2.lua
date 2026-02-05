local livnet = requirex "livnet"

local constant = livnet.constant;

local program = {};

local context = {};
context.window = nil;
context.renderer = nil;
context.opkey = constant.OPKEY_NONE;
context.fps = 0;

program.start = function(ctx)
        local ok, width, height;
        sdl.set_app_metadata("ui2", "1.0", "com.livnet.ui2");
        ok, context.window, context.renderer = sdl.create_window_and_renderer("ui2", 800, 600, 32);
        sdl.set_render_logical_presentation(context.renderer, 800, 600, 1);
        assert(ok);
        ok, width, height = sdl.get_render_logical_presentation(context.renderer);
       

        program.ctx = ctx;
        program.last_time = sdl.get_ticks();

        program.ok_btn = button.create(150, 100, 100, 50, "Start");
        program.combobox1 = combobox.create(0, 200, 100, 30);
        combobox.additem(program.combobox1, "1");
        combobox.additem(program.combobox1, "2");
        combobox.additem(program.combobox1, "3");
        combobox.additem(program.combobox1, "4");

        program.datagrid1 = datagrid.create({x=100, y=200, w=500, h=300}, 3, true);
        datagrid.setheaders(program.datagrid1, {"Name", "Age", "Occupation"});
        datagrid.addrow(program.datagrid1, {"Alice Brown", "35", "Developer"});
        datagrid.addrow(program.datagrid1, {"Jane Smith", "25", "Designer"});
        datagrid.addrow(program.datagrid1, {"Bob Johnson", "40", "Manager" });
        datagrid.addrow(program.datagrid1, {"Jane Smith2", "35", "Developer"});
        datagrid.addrow(program.datagrid1, {"Alice Brown", "35", "Developer"});
        datagrid.addrow(program.datagrid1, {"Jane Smith3", "35", "Developer"});
        datagrid.addrow(program.datagrid1, {"Alice Brown", "35", "Developer"});
        datagrid.addrow(program.datagrid1, {"Alice Brown", "35", "Developer"});
        datagrid.addrow(program.datagrid1, {"Alice Brown", "35", "Developer"});
        datagrid.addrow(program.datagrid1, {"Alice Brown1", "35", "Developer"});
        datagrid.addrow(program.datagrid1, {"Alice Brown2", "35", "Developer"});
        datagrid.addrow(program.datagrid1, {"Alice Brown3", "35", "Developer"});

        program.group = button_group.create();
        local radio1 = radio_button.create(50, 50, 30, "Option 1", 0);
        local radio2 = radio_button.create(50, 150, 30, "Option 2", 0);
        local radio3 = radio_button.create(50, 210, 30, "Option 3", 0);

        button_group.add_button(program.group, radio1);
        button_group.add_button(program.group, radio2);
        button_group.add_button(program.group, radio3);
        button_group.set_selected(program.group, 1);


        program.checkbox1 = checkbox.create(50, 50, 20, "Option 1");
        program.checkbox2 = checkbox.create(50, 80, 20, "Option 2");
        program.checkbox3 = checkbox.create(50, 110, 20, "Option 3 (Disabled)");

end

program.event = function(event)
        local etype = event.type;
        if etype == livnet.constant.SDL_EVENT_KEY_DOWN then
                if event.key.scancode == livnet.constant.SDL_SCANCODE_Q then
                        core.quit(program.ctx);
                elseif event.key.scancode == livnet.constant.SDL_SCANCODE_ESCAPE then
                        core.quit(program.ctx);
                end
        end
        button.handle_event(program.ok_btn, event.event, context.renderer);
        combobox.handle_event(program.combobox1, event.event, context.renderer);
        datagrid.handle_event(program.datagrid1, event.event, context.renderer);
        button_group.handle_event(program.group, event.event, context.renderer);

        checkbox.handle_event(program.checkbox1, event.event, context.renderer);
        checkbox.handle_event(program.checkbox2, event.event, context.renderer);
        checkbox.handle_event(program.checkbox3, event.event, context.renderer);
end

program.update = function()
        sdl.set_render_draw_color(context.renderer, 0, 92, 92, 255);
        sdl.render_clear(context.renderer);
        sdl.set_render_scale(context.renderer, 1, 1);
        button.render(program.ok_btn, context.renderer);
        combobox.render(program.combobox1, context.renderer);
        datagrid.render(program.datagrid1, context.renderer);
        button_group.render(program.group, context.renderer);
        checkbox.render(program.checkbox1, context.renderer);
        checkbox.render(program.checkbox2, context.renderer);
        checkbox.render(program.checkbox3, context.renderer);

        sdl.set_render_scale(context.renderer, 2, 2);
        sdl.set_render_draw_color(context.renderer, 0, 0, 0, 255);
        sdl.set_render_scale(context.renderer, 1, 1);
        sdl.render_present(context.renderer);
end


program.destroy = function()
        button.destroy(program.ok_btn);
        combobox.destroy(program.combobox1);
        datagrid.destroy(program.datagrid1);
        checkbox.destroy(program.checkbox1);
        checkbox.destroy(program.checkbox2);
        checkbox.destroy(program.checkbox3);
        local window = context.window;
        local renderer = context.renderer;
        sdl.destroy_renderer(renderer);
        sdl.destroy_window(window);
        livnet.log("ui2 exit");
end

return program;
