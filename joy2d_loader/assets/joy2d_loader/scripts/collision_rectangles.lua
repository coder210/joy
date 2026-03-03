local PIXELS_PER_METER = 100;

local SDL_EVENT_KEY_DOWN = 768;
local SDL_SCANCODE_ESCAPE = 41;
local SDL_SCANCODE_Q = 41;
local SDL_SCANCODE_RIGHT = 79;
local SDL_SCANCODE_UP = 82;
local SDL_SCANCODE_LEFT = 80;
local SDL_SCANCODE_DOWN = 81;

local window, renderer,context;
local graphics_buffer, contact;

local program = {};



local function handle_keydown_event(scancode)
        if scancode == SDL_SCANCODE_Q then
                core.quit(context);
        elseif scancode == SDL_SCANCODE_ESCAPE then
                core.quit(context);
        elseif scancode == SDL_SCANCODE_UP then
        elseif scancode == SDL_SCANCODE_RIGHT then
        elseif scancode == SDL_SCANCODE_DOWN then
        elseif scancode == SDL_SCANCODE_LEFT then
        end
end


program.event = function (event)
        local etype = event.type;
        if etype == SDL_EVENT_KEY_DOWN then
		local scancode = event.key.scancode;
                handle_keydown_event(scancode);
        end
end


program.start = function(ctx)
        local ok;
        sdl.set_app_metadata("collision2d", "1.0", "com.livgame.collision2d");
        ok, window, renderer = sdl.create_window_and_renderer("collision2d", 720, 480, 32);
        assert(ok);
        context = ctx;
        graphics_buffer = graphics.create();
end;

program.update = function ()
        local rect1, rect2;
        rect1 = {
                x = fp.from_float(0.2),
                y = fp.from_float(0.2),
                w = fp.from_float(1.6),
                h = fp.from_float(1.6)
        };
        rect2 = {
                x = fp.from_float(1.55),
                y = fp.from_float(1.55),
                w = fp.from_float(1.2),
                h = fp.from_float(1.2)
        };
		sdl.set_render_draw_color(renderer, 0, 0, 0, 255);
		sdl.render_clear(renderer);

		if collision2df.rectangles(rect1, rect2) then
				sdl.set_render_draw_color(renderer, 255, 0, 0, 255);
		else
				sdl.set_render_draw_color(renderer, 0, 255, 0, 255);
		end

		local x1, y1, w1, h1;
		local x2, y2, w2, h2;
		x1 = fp.to_float(rect1.x) * PIXELS_PER_METER;
		y1 = fp.to_float(rect1.y) * PIXELS_PER_METER;
		w1 = fp.to_float(rect1.w) * PIXELS_PER_METER;
		h1 = fp.to_float(rect1.h) * PIXELS_PER_METER;
		x2 = fp.to_float(rect2.x) * PIXELS_PER_METER;
		y2 = fp.to_float(rect2.y) * PIXELS_PER_METER;
		w2 = fp.to_float(rect2.w) * PIXELS_PER_METER;
		h2 = fp.to_float(rect2.h) * PIXELS_PER_METER;
		sdl.render_rect(renderer, x1, y1, w1, h1);
		sdl.render_rect(renderer, x2, y2, w2, h2);

		local rect1_angle = 0;
		local rect2_angle = 30;

		local n, contact = collision2df.rectanglesx(rect1, rect1_angle, rect2, fp.from_float(rect2_angle));
		if n ~= 0 then
				sdl.set_render_draw_color(renderer, 255, 0, 0, 255);
		else
				sdl.set_render_draw_color(renderer, 0, 255, 0, 255);
		end
		
		graphics.rectr(renderer, graphics_buffer, x1, y1, w1, h1, rect1_angle, 2);
		graphics.rectr(renderer, graphics_buffer, x2, y2, w2, h2, rect2_angle, 2);

		graphics.render(renderer, graphics_buffer);
		graphics.clear(graphics_buffer);

		sdl.render_present(renderer);
end


program.destroy = function()
        graphics.destroy(graphics_buffer);
        sdl.destroy_renderer(renderer);
        sdl.destroy_window(window);
        sdl.log("collision test service exit");
end

return program;