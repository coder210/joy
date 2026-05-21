local livnet = requirex "livnet"


livnet.start(function ()
        livnet.log("bootstrap");
        livnet.alias("bootstrap");

        local root = livnet.get_env("root");
        livnet.log(root);

        -- local collision_circles = livnet.launch(root .. "collision_circles.lua");
        -- livnet.named(collision_circles, "collision_circles");
        -- local collision_rectangles = livnet.launch(root .. "collision_rectangles.lua");
        -- livnet.named(collision_rectangles, "collision_rectangles");
        -- local collision_polygons = livnet.launch(root .. "collision_polygons.lua");
        -- livnet.named(collision_polygons, "collision_polygons");
        -- local collision_ray_quad = livnet.launch(root .. "collision_ray_quad.lua");
        -- livnet.named(collision_ray_quad, "collision_ray_quad");
        -- local collision_ray_rectangle = livnet.launch(root .. "collision_ray_rectangle.lua");
        -- livnet.named(collision_ray_rectangle, "collision_ray_rectangle");
        -- local collision_ray_rectanglex = livnet.launch(root .. "collision_ray_rectanglex.lua");
        -- livnet.named(collision_ray_rectanglex, "collision_ray_rectanglex");
        -- local crypt = livnet.launch(root .. "crypt.lua");
        -- livnet.named(crypt, "crypt");
        -- local gpu = livnet.launch(root .. "gpu.lua");
        -- livnet.named(gpu, "gpu");

        --local ui = livnet.launch(root .. "ui.lua");
        --livnet.named(ui, "ui");

        -- local framework = livnet.launch(root .. "framework.lua");
        -- livnet.named(framework, "framework");

        -- local audio = livnet.launch(root .. "audio.lua");
        -- livnet.named(audio, "audio");
        
        --local tilemap = livnet.launch(root .. "tilemap.lua");
        --livnet.named(tilemap, "tilemap");

        livnet.exit_func(function ()
                livnet.log("bootstrap service exit");
        end);

        livnet.exit();
end);