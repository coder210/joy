function joy.load()
    -- 创建字体（只需一次）
    state.font = joy.graphics.create_font("simhei.ttf", 24)
    joy.window.create("test", 800, 600, 0)
    joy.graphics.create()
    joy.sdl.log("ready")
end

function joy.draw()
    joy.graphics.clear(50, 50, 50, 255)
    joy.graphics.set_color(255, 100, 100, 255)
    joy.graphics.rectangle("fill", 100, 100, 200, 150)
    joy.graphics.print(state.font, "Hello World", 100, 200, 255, 255, 255, 255)
    joy.graphics.present()
end

function joy.mousepressed(x, y, button)
    print("mouse", button, x, y)
end

function joy.keypressed(key)
    print("key:", key)
    if key == "escape" then os.exit() end
end

function joy.resize(w, h)
    print("resize", w, h)
end
