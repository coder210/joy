local physics = requirex "physics"
local warrior_blue = requirex "warrior_blue"
local map = requirex "map"

local livnet = {};




local constant = {};
constant.SDL_EVENT_KEY_DOWN = 768;
constant.SDL_EVENT_KEY_UP = 0x301;
constant.SDL_SCANCODE_Q = 20;
constant.SDL_SCANCODE_ESCAPE = 41;
constant.SDL_SCANCODE_SPACE = 44;
constant.SDL_SCANCODE_RIGHT = 79;
constant.SDL_SCANCODE_UP = 82;
constant.SDL_SCANCODE_LEFT = 80;
constant.SDL_SCANCODE_DOWN = 81;

constant.CONNECTED = 1;
constant.DISCONNECTED = 2;
constant.READ = 3;

constant.CMD_NONE = 0x1;
constant.CMD_FRAME = 0x2;
constant.CMD_READY = 0x3;
constant.CMD_LOAD_SCENE = 0x4;
constant.CMD_HEALTH = 0x8;

constant.CMD_PLAYER_JOIN = 0x10;
constant.CMD_PLAYER_LEAVE = 0x11;
constant.CMD_PLAYER_COMMAND = 0x12;

constant.OPKEY_NONE = 0x0;
constant.OPKEY_UP = 0x1;
constant.OPKEY_DOWN = 0x2;
constant.OPKEY_LEFT = 0x3;
constant.OPKEY_RIGHT = 0x4;
constant.OPKEY_SHOOT = 0x5;

constant.PIXELS_PER_METER = 100;



livnet.constant = constant;




local resources = {};

resources.textures = {};

resources.get_texture = function(renderer, name)
        --livnet.log("renderer type:" .. type(renderer) .. " name type:" .. name);
        local texture, w, h;
        if resources.textures[name] then
                texture = resources.textures[name];
        else
                texture = sprite_batch.load_png(renderer, "resources/" .. name)
                --texture, w, h = sdl.create_texturex(renderer, "resources/" .. name);
                resources.textures[name] = texture;
        end
        return texture;
end

livnet.resources = resources;



Object = {};
Object.__index = Object


function Object.new()
        local self = setmetatable({}, Object);
        self.transform = {};
        self.transform.position = { x = 0, y = 0 };
        self.transform.size = { x = 0, y = 0 };
        self.transform.scale = { x = 1, y = 1 };
        self.color = { r = 255, g = 255, b = 255, a = 255 };
        return self;
end


function Object:update(dt)

end



function Object:draw_box(camera, renderer)
        local x, y, w, h;
        local color = self.color;
        sdl.set_render_draw_color(renderer, color.r, color.g, color.b, color.a);
        x = self.transform.position.x;
        y = self.transform.position.y;
        w = self.transform.size.x;
        h = self.transform.size.y;
        x = x * constant.PIXELS_PER_METER - camera.position.x;
        y = y * constant.PIXELS_PER_METER - camera.position.y;
        w = w * constant.PIXELS_PER_METER;
        h = h * constant.PIXELS_PER_METER;
        sdl.render_rect(renderer, x, y, w, h);
end

function Object:draw(camera, renderer)
        if self.entity then
                self:draw_box(camera, renderer);
        end
end

--Adimation
Animation = {};
Animation.__index = Animation


function Animation.new()
        local self = setmetatable({}, Animation);
        self.clips = {};
        self.is_loop = true;
        self.idx_frame = 1;
        self.total_time = 0;
        self.last_time = utils.current_time();
        return self;
end

function Animation:reset()
        self.idx_frame = 1;
        self.total_time = 0;
        self.last_time = utils.current_time();
end

function Animation:add_clip(image, duration, src_rect, dest_rect)
        table.insert(self.clips, {
                image = image,
                duration = duration,
                src_rect = src_rect,
                dest_rect = dest_rect
        });
end

function Animation:check_finshed()
        if self.is_loop then
                return false;
        end
        return (self.idx_frame == #self.clips);
end

function Animation:set_position(x, y)
        for i = 1, #self.clips, 1 do
                self.clips[i].dest_rect.x = x;
                self.clips[i].dest_rect.y = y;
        end
end


function Animation:update(dt)
        local clip = self.clips[self.idx_frame];
        self.total_time = self.total_time + dt;
        if self.total_time >= clip.duration then
                self.idx_frame = self.idx_frame + 1;
                if self.idx_frame >= #self.clips then
                        if self.is_loop then
                                self.idx_frame = 1;
                        else
                                self.idx_frame = #self.clips;
                        end
                        --print(tostring(self.is_loop) .. ", total_time:" .. self.total_time .. " duration:" .. clip.duration);
                end
                self.total_time = 0;
        end
        -- local curr_time = utils.current_time();
        -- local clip = self.clips[self.idx_frame];
        -- if self.last_time + clip.duration <= curr_time then
        --         self.idx_frame = self.idx_frame + 1;
        --         if self.idx_frame >= #self.clips then
        --                 if self.is_loop then
        --                         self.idx_frame = 1;
        --                 else
        --                         self.idx_frame = #self.clips;
        --                 end
        --         end
        --         self.last_time = curr_time;
        -- end
end

function Animation:draw(camera, renderer, sprites)
        local clip = self.clips[self.idx_frame];
        local r_x = clip.dest_rect.x + clip.dest_rect.w;
        local r_y = clip.dest_rect.y + clip.dest_rect.h;
        if r_x < camera.position.x and r_y < camera.position.y then
                return;
        end

        r_x = camera.position.x + camera.size.x;
        r_y = camera.position.y + camera.size.y;
        if r_x < clip.dest_rect.x and r_y < clip.dest_rect.y then
                return;
        end

        local dest_rect = {
                x = clip.dest_rect.x - camera.position.x,
                y = clip.dest_rect.y - camera.position.y,
                w = clip.dest_rect.w,
                h = clip.dest_rect.h
        }

        local center = { x = 0, y = 0 };
        local texture = resources.get_texture(renderer, clip.image);
        --sdl.render_texture_rotated(renderer, texture, clip.src_rect, dest_rect, 0, center, 0);
        sprite_batch.draw(sprites, texture, clip.src_rect, dest_rect, {r = 1.0, g = 1.0, b = 1.0, a = 1.0});
end


Bullet = {}
Bullet.__index = Bullet

Bullet.MAX_HEALTH = 100
Bullet.DEFAULT_SPEED = 150

function Bullet.new(entityid)
        local self = setmetatable({}, Bullet);
        self.id = livnet.world.gen_id();
        self.transform = {};
        self.transform.position = { x = 0, y = 0 };
        self.transform.size = { x = 0, y = 0 };
        self.transform.scale = { x = 1, y = 1 };
        self.color = { r = 255, g = 255, b = 255, a = 255 };

        if entityid then
                self.entity = physics.new_entity(entityid);
        else
                self.entity = physics.create_entity();
        end

        local speed = fp.from_float(0.1);
        self.entity.static = 0;
        self.entity.type = physics.TYPE_BULLET;
        self.entity.width = fp.from_float(0.4);
        self.entity.height = fp.from_float(0.6);
        self.entity.linear_velocity.x = fp.zero();
        self.entity.linear_velocity.y = fp.negate(speed);
        self.entity.userdata.gid = self.id;

        livnet.world.objects[self.id] = self;

        return self;
end


function Bullet:set_position(x, y)
        self.entity.position.x = x;
        self.entity.position.y = y;
end

function Bullet:set_size(x, y)
        self.entity.width = x;
        self.entity.height = y;
end

function Bullet:change_animation(name, dt)
        if self.animation_name == name then
                return;
        end
        self.total_time = self.total_time + dt;
        if self.total_time <= 1000 then
                return;
        end
        self.total_time = 0;
        if self.animation_name ~= name then
                self.animation_name = name;
                self.animation = self.animations[self.animation_name];
                self.animation:reset();
        end
end

function Bullet:update_animation(dt)
        local linear_velocity = vec2f.abs(self.entity.linear_velocity);
        if linear_velocity.x > 0 or linear_velocity.y > 0 then
                self:change_animation("run", dt);
        else
                self:change_animation("idle", dt);
        end

        local x = self.transform.position.x * livnet.constant.PIXELS_PER_METER - map.tilewidth;
        local y = self.transform.position.y * livnet.constant.PIXELS_PER_METER - map.tileheight;
        self.animation:set_position(x, y);

        self.animation:update(dt);
end

function Bullet:update_transform(dt)
        self.transform.size.x = fp.to_float(self.entity.width);
        self.transform.size.y = fp.to_float(self.entity.height);

        local target = {};
        target.x = fp.to_float(self.entity.position.x);
        target.y = fp.to_float(self.entity.position.y);
        self.transform.position = vec2.lerp(target, self.transform.position, dt);
        -- self.transform.position.x = ft.lerp(target.x, self.transform.position.x, dt);
        -- self.transform.position.y = ft.lerp(target.y, self.transform.position.y, dt);
end

function Bullet:update(dt)
        local dt2 = dt / 1000;
        self:update_transform(dt2);
        self:update_animation(dt);

        self.entity.linear_velocity.x = 0;
        self.entity.linear_velocity.y = 0;
end

function Bullet:draw_box(camera, renderer)
        local x, y, w, h;
        local color = self.color;
        sdl.set_render_draw_color(renderer, color.r, color.g, color.b, color.a);
        x = self.transform.position.x;
        y = self.transform.position.y;
        w = self.transform.size.x;
        h = self.transform.size.y;
        x = x * livnet.constant.PIXELS_PER_METER - camera.position.x;
        y = y * livnet.constant.PIXELS_PER_METER - camera.position.y;
        w = w * livnet.constant.PIXELS_PER_METER;
        h = h * livnet.constant.PIXELS_PER_METER;
        sdl.render_rect(renderer, x, y, w, h);
end

function Bullet:draw(camera, renderer)
        self.animation:draw(camera, renderer);
        self:draw_box(camera, renderer);
end



Player = {}
Player.__index = Player

Player.MAX_HEALTH = 100
Player.DEFAULT_SPEED = 150

function Player.new(entityid)
        local self = setmetatable({}, Player);
        self.id = livnet.world.gen_id();
        self.transform = {};
        self.transform.position = { x = 0, y = 0 };
        self.transform.size = { x = 0, y = 0 };
        self.transform.scale = { x = 1, y = 1 };
        self.color = { r = 255, g = 255, b = 255, a = 255 };

        if entityid then
                self.entity = physics.new_entity(entityid);
        else
                self.entity = physics.create_entity();
        end

        self.entity.static = 0;
        self.entity.type = physics.TYPE_PLAYER;
        self.entity.width = fp.from_float(0.4);
        self.entity.height = fp.from_float(0.6);
        self.entity.userdata.gid = self.id;

        self.entity.name = "player" .. self.entity.id;

        --go.show_box = true;
        self.animations = {};

        local row, col;
        local image = warrior_blue.image;
        local columns = warrior_blue.columns;
        local grid = warrior_blue.grid;

        for _, tile in ipairs(warrior_blue.tiles) do
                local x = self.transform.position.x;
                local y = self.transform.position.y;

                local animation = Animation.new();

                if string.find(tile.type, "attack") then
                        animation.is_loop = false;
                end

                for _, clip in ipairs(tile.animation) do
                        if clip.tileid < columns then
                                row = 0;
                                col = clip.tileid;
                        else
                                row = math.floor(clip.tileid / columns);
                                col = clip.tileid % columns;
                        end
                        local src_rect = {
                                x = col * grid.width,
                                y = row * grid.height,
                                w = grid.width,
                                h = grid.height
                        };
                        local dest_rect = {
                                x = x,
                                y = y,
                                w = grid.width,
                                h = grid.height
                        };
                        animation:add_clip(image, clip.duration, src_rect, dest_rect);
                end
                self.animations[tile.type] = animation;
        end

        self.show_box = true;
        self.animation_name = "idle"
        self.total_time = 0;

        self.hp = 10;

        livnet.world.objects[self.id] = self;

        return self;
end

function Player:set_position(x, y)
        self.entity.position.x = x;
        self.entity.position.y = y;
end

function Player:set_size(x, y)
        self.entity.width = x;
        self.entity.height = y;
end

function Player:move()
        self.entity.position = vec2f.add(self.entity.position, self.entity.linear_velocity);
end

function Player:command(opkey)
        local speed = fp.from_float(0.3);
        if opkey == constant.OPKEY_NONE then
                self.entity.linear_velocity.x = 0;
                self.entity.linear_velocity.y = 0;
                self:change_animation("idle");
        elseif opkey == constant.OPKEY_UP then
                self.entity.linear_velocity.x = fp.zero();
                self.entity.linear_velocity.y = fp.negate(speed);
                self:move();
                self:change_animation("run");
        elseif opkey == constant.OPKEY_LEFT then
                self.entity.linear_velocity.x = fp.negate(speed);
                self.entity.linear_velocity.y = fp.zero();
                self:move();
                self:change_animation("run");
        elseif opkey == constant.OPKEY_RIGHT then
                self.entity.linear_velocity.x = speed;
                self.entity.linear_velocity.y = fp.zero();
                self:move();
                self:change_animation("run");
        elseif opkey == constant.OPKEY_DOWN then
                self.entity.linear_velocity.x = fp.zero();
                self.entity.linear_velocity.y = speed;
                self:move();
                self:change_animation("run");
        elseif opkey == constant.OPKEY_SHOOT then
                self:change_animation("attack_right1");
                local entities = physics.query_collision({
                        static = 1,
                        rotation = 0,
                        position = {
                                x = fp.add(self.entity.position.x, fp.from_float(0.2)),
                                y = self.entity.position.y
                        },
                        width = self.entity.width,
                        height = self.entity.height
                });
                if #entities > 0 then
                        for _, entity in ipairs(entities) do
                                if entity.userdata.gid then
                                        local obj = livnet.world.objects[entity.userdata.gid];
                                        if obj.collide then  
                                                obj:collide(self.entity);
                                        end
                                end
                        end
                        --livnet.log("attack_right1");
                end
                --prefab.bullet(entity);
        end

        --physics.collide();
        --local count = physics.query_collision(self.entity.id);
        --livnet.log("count:" .. count);
        physics.collide2(self.entity.id);
        --physics.collide();
end

function Player:collide(entity)
        if self.entity.id ~= entity.id then
                if self.hp >= 1 then
                        self.hp = self.hp - 1;
                end
                livnet.log("collide:" .. self.hp);
        end
end

function Player:change_animation(name)
        if self.animation_name == name then
                return;
        end
        -- self.total_time = self.total_time + dt;
        -- if self.total_time <= 1000 then
        --         return;
        -- end
        -- self.total_time = 0;
        local animation = self.animations[self.animation_name];
        if self.animation_name == 'attack_right1' then
                if not animation:check_finshed() then
                      return;
                end
        end

        self.animation_name = name;
        animation = self.animations[self.animation_name];
        animation:reset();
end

function Player:update_animation(dt)
        -- local linear_velocity = vec2f.abs(self.entity.linear_velocity);
        -- if linear_velocity.x > 0 or linear_velocity.y > 0 then
        --         self:change_animation("run", dt);
        -- else
        --         self:change_animation("idle", dt);
        -- end

        local x = self.transform.position.x * livnet.constant.PIXELS_PER_METER - map.tilewidth;
        local y = self.transform.position.y * livnet.constant.PIXELS_PER_METER - map.tileheight;
        self.animations[self.animation_name]:set_position(x, y);
        self.animations[self.animation_name]:update(dt);
end

function Player:update_transform(dt)
        self.transform.size.x = fp.to_float(self.entity.width);
        self.transform.size.y = fp.to_float(self.entity.height);

        local target = {};
        target.x = fp.to_float(self.entity.position.x);
        target.y = fp.to_float(self.entity.position.y);
        self.transform.position = vec2.lerp(target, self.transform.position, dt);
        -- self.transform.position.x = ft.lerp(target.x, self.transform.position.x, dt);
        -- self.transform.position.y = ft.lerp(target.y, self.transform.position.y, dt);
end

function Player:update(dt)
        local dt2 = dt / 1000;
        self:update_transform(dt2);
        self:update_animation(dt);
end

function Player:draw_box(camera, renderer)
        local x, y, w, h;
        local color = self.color;
        sdl.set_render_draw_color(renderer, color.r, color.g, color.b, color.a);
        x = self.transform.position.x;
        y = self.transform.position.y;
        w = self.transform.size.x;
        h = self.transform.size.y;
        x = x * livnet.constant.PIXELS_PER_METER - camera.position.x;
        y = y * livnet.constant.PIXELS_PER_METER - camera.position.y;
        w = w * livnet.constant.PIXELS_PER_METER;
        h = h * livnet.constant.PIXELS_PER_METER;
        sdl.render_rect(renderer, x, y, w, h);
end

function Player:draw(camera, renderer, sprites)
        --livnet.log(self.animation_name);
        local animation = self.animations[self.animation_name];
        animation:draw(camera, renderer, sprites);


        local str = string.format("<%f,%f> hp:%02d", self.transform.position.x, self.transform.position.y, self.hp);
        local px = self.transform.position.x * constant.PIXELS_PER_METER;
        local py = (self.transform.position.y + 0.1) * constant.PIXELS_PER_METER;
        sdl.render_debug_text(renderer, px, py, str);
        --self:draw_box(camera, renderer);
end





local queue = {}
queue.new = function()
    local q = {};
    q.elements = {};
    q.head = 1;
    q.tail = 0;
    return q;
end

queue.enqueue = function(q, value)
    q.tail = q.tail + 1
    q.elements[q.tail] = value
end

queue.dequeue = function(q)
    if q.head > q.tail then
        return nil;
    end
    local value = q.elements[q.head];
    q.elements[q.head] = nil;
    q.head = q.head + 1;
    return value;
end

queue.is_empty = function(q)
    return q.head > q.tail;
end

queue.size = function(q)
    return q.tail - q.head + 1;
end

queue.peek = function(q)
    if q.head > q.tail then
        return nil;
    end
    local value = q.elements[q.head];
    return value;
end


local common = {};

livnet.camera = {};
livnet.camera.position = { x = 0, y = 0 };
livnet.camera.size = { x = 0, y = 0 };


local world = {};
world.id = 1;
world.objects = {};

function world.gen_id()
        local id = world.id;
        world.id = world.id + 1;
        return id;
end

function world.new_animation(node, x, y)
        local animation = Animation:new();
        local grid = node.grid;
        local row, col;
        for _, clip in ipairs(node.animation) do
                if clip.tileid < node.columns then
                        row = 0;
                        col = clip.tileid;
                else
                        row = math.floor(clip.tileid / node.columns);
                        col = clip.tileid % node.columns;
                end
                local src_rect = {
                        x = col * grid.width,
                        y = row * grid.height,
                        w = grid.width,
                        h = grid.height
                };
                local dest_rect = { x = x, y = y, w = grid.width, h = grid.height };
                animation:add_clip(node.image, clip.duration, src_rect, dest_rect);
        end
        animation:set_position(x, y);
        return animation;
end

function world.new_image(tile, x, y)
        local inst = Image.new(tile.image);
        inst:set_src_rect(tile.s_rect.x, tile.s_rect.y, tile.s_rect.w, tile.s_rect.h);
        inst:set_dest_rect(x, y, tile.d_rect.w, tile.d_rect.h);
        return inst;
end


function world.get_object(id)
        return livnet.world.objects[id];
end

function world.remove_object(id)
        livnet.world.objects[id] = nil;
end

function world.update(dt)
        for _, obj in pairs(world.objects) do
                obj:update(dt);
        end
end

function world.render(renderer, sprites)
        for _, obj in pairs(world.objects) do
                obj:draw(livnet.camera, renderer, sprites);
        end
end

livnet.world = world;


local call = {};
call.new = function(interval)
        local inst = {};
        inst.interval = interval;
        inst.total_time = 0;
        return inst;
end

call.invoke = function(inst, dt, func, ...)
        inst.total_time = inst.total_time + dt;
        if inst.total_time >= inst.interval then
                func(dt, inst.interval);
                inst.total_time = inst.total_time - inst.interval;
        end
end



local tilemap = {};
tilemap.tiles = {};
tilemap.handle_tile = function (renderer, tileset, key, id)
        local is_tile = false;
        for _, tile in ipairs(tileset.tiles) do
                if tile.id == id then
                        is_tile = true;
                        if tile.animation then
                                resources.get_texture(renderer, tileset.image);
                                tilemap.tiles[key] = {
                                        image = tileset.image,
                                        animation = tile.animation,
                                        columns = tileset.columns,
                                        grid = tileset.grid,
                                };
                        elseif tile.image then
                                resources.get_texture(renderer, tile.image);
                                tilemap.tiles[key] = {
                                        image = tile.image,
                                        grid = {
                                                width = 0,
                                                height = tile.height,
                                        },
                                        s_rect = {
                                                x = 0,
                                                y = 0,
                                                w = tile.width,
                                                h = tile.height,
                                        },
                                        d_rect = {
                                                x = 0,
                                                y = 0,
                                                w = tile.width,
                                                h = tile.height,
                                        }
                                }
                        end
                        break;
                end
        end

        if not is_tile then
                local columns = tileset.columns;
                local grid = tileset.grid;
                local row, col;
                if id < columns then
                        row = 0;
                        col = id;
                else
                        row = math.floor(id / columns);
                        col = id % columns;
                end

                resources.get_texture(renderer, tileset.image);
                tilemap.tiles[key] = {
                        image = tileset.image,
                        s_rect = {
                                x = col * grid.width,
                                y = row * grid.height,
                                w = grid.width,
                                h = grid.height
                        },
                        d_rect = {
                                x = 0,
                                y = 0,
                                w = grid.width,
                                h = grid.height
                        },
                        grid = grid,
                };
        end
end

tilemap.handle_tileset = function (renderer, tileset)
        for i = 0, tileset.tilecount - 1, 1 do
                local key = string.format("%s_%d", tileset.name, i + tileset.firstgid);
                tilemap.handle_tile(renderer, tileset, key, i);
        end
end

tilemap.get_tile = function (tilesets, gid)
        local result = nil;
        for _, tile in ipairs(tilesets) do
                if gid >= tile.firstgid then
                        result = tile;
                end
        end
        if result then
                local key = string.format("%s_%d", result.name, gid);
                return tilemap.tiles[key];
        else
                return nil;
        end
end

tilemap.handle_tilelayer = function(map, layer)
        for index, gid in ipairs(layer.data) do
                local i = index - 1;
                local row, col;
                if i < layer.width then
                        row = 0;
                        col = i;
                else
                        row = math.floor(i / layer.width);
                        col = i % layer.width;
                end

                local x = col * map.tilewidth;
                local y = row * map.tileheight;

                if gid ~= 0 then
                        local tile = tilemap.get_tile(map.tilesets, gid);
                        if tile then
                                y = y + (map.tileheight - tile.grid.height);
                                if tile.animation then
                                        local animation = world.new_animation(tile, x, y);
                                        local id = world.gen_id();
                                        world.objects[id] = animation;
                                else
                                        local image = world.new_image(tile, x, y);
                                        local id = world.gen_id();
                                        world.objects[id] = image;
                                end
                        end
                end
        end
end

tilemap.handle_imagelayer = function(map, layer)

end

tilemap.handle_objectgroup = function(map, layer)
        if layer.name == "collision" then
                for _, obj in ipairs(layer.objects) do
                        if obj.shape == "rectangle" then
                                --livnet.log(string.format("%d, %d, %d, %d", obj.x, obj.y, obj.width, obj.height));
                                local id = world.gen_id();
                                local go = Object.new();
                                go.id = id;
                                go.entity = physics.create_entity();
                                go.entity.position.x = fp.from_float(obj.x / constant.PIXELS_PER_METER);
                                go.entity.position.y = fp.from_float(obj.y / constant.PIXELS_PER_METER);
                                go.entity.width = fp.from_float(obj.width / constant.PIXELS_PER_METER);
                                go.entity.height = fp.from_float(obj.height / constant.PIXELS_PER_METER);
                                go.entity.userdata.gid = go.id;
                                world.objects[id] = go;

                                go.transform.position.x = fp.to_float(go.entity.position.x);
                                go.transform.position.y = fp.to_float(go.entity.position.y);
                                go.transform.size.x = fp.to_float(go.entity.width);
                                go.transform.size.y = fp.to_float(go.entity.height);

                                -- go.show_box = true;
                        end
                end
        end
end

tilemap.load = function (renderer, map)
        for _, tileset in ipairs(map.tilesets) do
                tilemap.handle_tileset(renderer, tileset);
        end

        for _, layer in ipairs(map.layers) do
                if layer.visible then
                        if layer.type == "tilelayer" then
                                tilemap.handle_tilelayer(map, layer);
                        elseif layer.type == "imagelayer" then
                                tilemap.handle_imagelayer(map, layer);
                        elseif layer.type == "objectgroup" then
                                tilemap.handle_objectgroup(map, layer);
                        end
                end
        end
end


--ui
local ui = {};


ui.buttons = {};


ui.add_button = function (name, x, y, w, h)
        ui.buttons[name] = {
                name = name,
                x = x,
                y = y,
                w = w,
                h = h
        };
end

ui.render = function (renderer)
        for name, btn in pairs(ui.buttons) do   
                sdl.render_fill_rect(renderer, btn.x, btn.y, btn.w, btn.h);
        end
end




local ecs = {};

local entities = {};
local removing_entities = {};
local systems = {};
local systems2 = {};
local startup_systems = {};
local shutdown_systems = {};
local resources = {};
local component_types = {};
local component_entity_mapping = {};
local global_id = 1;


function ecs:register_type(name, type)
        if not component_types[name] then
                component_types[name] = type;
                return true;
        end
        return false;
end

local function array_intersect(a_arr, b_arr)
        local intersection = {};
        if not b_arr then
              return intersection;
        end
        for _, b_id in ipairs(b_arr) do
                for _, a_id in ipairs(a_arr) do
                        if b_id == a_id then
                                table.insert(intersection, a_id);
                        end
                end
        end
        return intersection;
end

function ecs:query(...)
        local component_container;
        local types = {...};
        local result = {};
        local first = true;
        for _, type in ipairs(types) do
                component_container = component_entity_mapping[type];
                if first then
                        first = false;
                        if component_container then
                                for _, id in ipairs(component_container) do
                                        table.insert(result, id);
                                end
                        end
                else
                        result = array_intersect(result, component_container);
                end

                if #result == 0 then
                        break;
                end
        end
        return result;
end

function ecs:spawn(...)
        local types = {...};
        local entity = {};
        local id = global_id;
        global_id = global_id + 1;
        entities[id] = entity;
        for index, type in ipairs(types) do
                ecs:add_component(id, type);
        end
        return id;
end

function ecs:destroy(id)
        table.insert(removing_entities, id);
end

function ecs:add_component(id, type)
        local entity, component_type, component_container;
        entity = entities[id];
        if not entity or entity[type] then
                return false;
        end
        component_type = component_types[type];
        if not component_type then
                return false;
        end
        entity[type] = component_type();
        if not component_entity_mapping[type] then
                component_entity_mapping[type] = {};
        end
        component_container = component_entity_mapping[type];
        table.insert(component_container, id);
        return true;
end

function ecs:get_component(id, type)
        local entity = entities[id];
        if entity then
                return entity[type];
        end
        return nil;
end

local function do_remove_component(id, type)
        local component_container = component_entity_mapping[type];
        if component_container then
                for index, value in ipairs(component_container) do
                        if id == value then
                                table.remove(component_container, index);
                                break;
                        end
                end
        end
end

function ecs:remove_component(id, type)
        local entity = entities[id];
        if entity then
                do_remove_component(id, type);
                entity[type] = nil;
        end
end

function ecs:set_resource(key, value)
        resources[key] = value;
end

function ecs:get_resource(key)
        return resources[key];
end

function ecs:add_system(name, system)
        if not systems[name] then
                systems[name] = system;
                return true;
        end
        return false;
end

function ecs:add_system2(name, system)
        if not systems2[name] then
                systems2[name] = system;
                return true;
        end
        return false;
end

function ecs:add_startup_system(name, system)
        if not startup_systems[name] then
                startup_systems[name] = system;
                return true;
        end
        return false;
end

function ecs:add_shutdown_system(name, system)
        if not shutdown_systems[name] then
                shutdown_systems[name] = system;
                return true;
        end
        return false;
end

function ecs:startup()
        for name, startup_system in pairs(startup_systems) do
                startup_system(name);
        end
end

function ecs:shutdown()
        for name, shutdown_system in pairs(shutdown_systems) do
                shutdown_system(name);
        end
end

local function remove_entities()
        if #removing_entities <= 0 then
                return;
        end
        for _, id in ipairs(removing_entities) do
                local entity = entities[id];
                if entity then
                        for type, _ in pairs(entity) do
                                do_remove_component(id, type);
                        end
                        entities[id] = nil;
                end
        end
        removing_entities = {};
end

function ecs:update()
        for name, system in pairs(systems) do
                system(name);
        end
        remove_entities();
end

function ecs:update2()
        for name, system in pairs(systems2) do
                system(name);
        end
        remove_entities();
end

livnet.ecs = ecs;
livnet.constant = constant;
livnet.queue = queue;
livnet.common = common;
livnet.call = call;
livnet.load_tilemap = tilemap.load;



local c = requirex "core"

livnet.c = c;

livnet.PTYPE_TEXT = 0x0;
livnet.PTYPE_RESPONSE = 0x1;
livnet.PTYPE_MULTICAST = 0x2;
livnet.PTYPE_LUA = 0x8;
livnet.PTYPE_SDL_EVENT = 0x10;

local protos = {}

function livnet.log(msg)
	local info = debug.getinfo(2);
	c.log(info.short_src .. ":" .. info.currentline .. " " .. msg);
end

function livnet.register_protocol(class)
	local name = class.name
	local id = class.id
	assert(protos[name] == nil)
	assert(type(name) == "string" and type(id) == "number" and id >= 0 and id <= 255)
	protos[name] = class
	protos[id] = class
end

do
	local REG = livnet.register_protocol
	REG {
		name = "lua",
		id = livnet.PTYPE_LUA
	}

	local REG = livnet.register_protocol
	REG {
		name = "response",
		id = livnet.PTYPE_RESPONSE
	}

	local REG = livnet.register_protocol
	REG {
		name = "sdl_event",
		id = livnet.PTYPE_SDL_EVENT
	}
end

function livnet.alias(name)
	--c.command("ALIAS", name, "");
end

function livnet.named(handle, name)
	--c.command("NAMED", handle, name);
end

function livnet.get_env(key)
	--local value = c.command("GETENV", key, "");
	--return value;
end

function livnet.set_env(key, value)
	--c.command("SETENV", key, value);
end

function livnet.launch(filename)
	local handle = c.command("LAUNCH", "cluax", filename);
	return handle;
end

function livnet.query(name)
	local handle;
	if name then
		handle = c.command("QUERY", name, "");
	else
		handle = c.command("QUERY", "", "");
	end
	return math.tointeger(handle);
end

function livnet.timeout(time, param)
	local result;
	if param then
		result = c.command("TIMEOUT", tostring(time), param);
	else
		result = c.command("TIMEOUT", tostring(time), "");
	end
	return result;
end

livnet.exit_func = c.exit_func;

function livnet.exit()
	c.command("EXIT", "", "");
end

function livnet.kill(handle)
	c.command("KILL", tostring(handle), "");
end

function livnet.send(handle, str)
	local p = protos["lua"];
	c.send(handle, p.id, str);
end

function livnet.sendname(name, str)
	local p = protos["lua"];
	c.sendname(name, p.id, str);
end

function livnet.dispatch(typename, func)
	local p = protos[typename]
	if func then
		local ret = p.dispatch
		p.dispatch = func
		return ret
	else
		return p and p.dispatch
	end
end

function livnet.dispatch_message(ptype, data, sz, source, ...)
	-- local p = protos[ptype];
	-- if p and p.dispatch then
	-- 	p.dispatch(data, sz, source, ...);
	-- end
        livnet.log(ptype);
end

function livnet.start(start_func, ...)
	c.callback(livnet.dispatch_message);
	local ok, err = xpcall(start_func, function(errmsg)
		livnet.log(errmsg);
		livnet.log(debug.traceback());
	end, ...);
	if not ok then
		livnet.log("xpcall function failed: " .. tostring(err))
	end
end

-- livnet.codepoints = {
--         ['开'] = 0x5F00,
--         ['始'] = 0x59CB,
--         ['我'] = 0x6211,
--         ['界'] = 0x754C,
-- };

-- livnet.get_codepoints = function (texts)
--         local codepoints = {};
--         for index, value in ipairs(texts) do
--                 table.insert(codepoints, livnet.codepoints[value]);
--         end
--         return codepoints;
-- end

livnet.get_codepoints = function (str)
        local codepoints = {};
        for pos, char in utf8.codes(str) do
                table.insert(codepoints, char);
        end
        return codepoints;
end

return livnet;