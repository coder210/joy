local PIXELS_PER_METER = 100;

local SDL_EVENT_KEY_DOWN = 768;
local SDL_SCANCODE_ESCAPE = 41;
local SDL_SCANCODE_Q = 41;
local SDL_SCANCODE_RIGHT = 79;
local SDL_SCANCODE_UP = 82;
local SDL_SCANCODE_LEFT = 80;
local SDL_SCANCODE_DOWN = 81;

local window, renderer,context;
local shapes, contact;

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
        shapes = shape_batch.create();
end;

program.update = function ()
        local rect1, rect2;
        local color = {};
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
				color.r = 255; color.g = 0; color.b = 0; color.a = 255;
		else
				color.r = 0; color.g = 255; color.b = 0; color.a = 255;
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
                shape_batch.rectangle(shapes, {x=x1, y=y1, w=w1, h=h1}, 2, color);
		shape_batch.rectangle(shapes, {x=x2, y=y2, w=w2, h=h2}, 2, color);

		local rect1_angle = 10;
		local rect2_angle = 30;

                
		local n, contact = collision2df.rectanglesx(rect1, rect1_angle, rect2, fp.from_float(rect2_angle));
		if n ~= 0 then
                        color.r = 255; color.g = 0; color.b = 0; color.a = 255;
		else
                        color.r = 0; color.g = 255; color.b = 0; color.a = 255;
		end
		
		shape_batch.rectangle_rotate(shapes, {x=x1, y=y1, w=w1, h=h1}, rect1_angle, 2, color);
		shape_batch.rectangle_rotate(shapes, {x=x2, y=y2, w=w2, h=h2}, rect2_angle, 2, color);

		shape_batch.render(shapes, renderer);

		sdl.render_present(renderer);
end


program.destroy = function()
        shape_batch.destroy(shapes);
        sdl.destroy_renderer(renderer);
        sdl.destroy_window(window);
        sdl.log("collision test service exit");
end

return program;