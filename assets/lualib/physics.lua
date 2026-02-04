local physics = {};

local entity_id = 1;
local entities = {};
local players = {};

physics.TYPE_PLAYER = 0x1;
physics.TYPE_BULLET = 0x2;


physics.entities = entities;
physics.players = players;

physics.set_entity_id = function(id)
        entity_id = id;
end

physics.get_entity_id = function()
        return entity_id;
end


physics.get_checksum = function()
        local str = tostring(entity_id);
        for _, entity in ipairs(entities) do
                if entity.static == 0 then
                        str = string.format("%s%d,%d,%d,%d", str, entity.id,
                                entity.position.x, entity.position.y, entity.rotation);
                end
        end
        return utils.md5(str);
        --return str;
end

physics.new_entity = function(id)
        local entity = {};
        entity.id = id;
        entity.static = 1;
        entity.type = 0;
        entity.width = 0;
        entity.height = 0;
        entity.rotation = 0;
        entity.linear_velocity = { x = 0, y = 0 };
        entity.position = { x = 0, y = 0 };
        entity.userdata = {};
        entities[entity.id] = entity;
        return entity;
end

physics.create_entity = function()
        --livnet.log("entity id:" .. entity_id);
        local entity = physics.new_entity(entity_id);
        entity_id = entity_id + 1;
        return entity;
end


physics.destroy_entity = function(id)
        entities[id] = nil;
end

physics.add_player = function(conv, entity_id)
        if not players[conv] then
                players[conv] = entity_id;
        end
end

physics.remove_player = function(conv)
        if players[conv] then
                local entity_id = players[conv];
                physics.destroy_entity(entity_id);
                players[conv] = nil;
        end
end

physics.get_player = function(conv)
        return players[conv];
end

physics.get_entity = function(entity_id)
        return entities[entity_id];
end

physics.find_entity = function(conv)
        if players[conv] then
                return physics.entities[players[conv]];
        else
                return nil;
        end
end

-- physics.move = function()
--         for _, entity in pairs(entities) do
--                 if entity.static == 0 then
--                         entity.position = vec2f.add(entity.position, entity.linear_velocity);
--                 end
--         end
-- end


-- physics.clear = function()
--         for _, entity in pairs(entities) do
--                 if entity.static == 0 then
--                         if entity.type == physics.TYPE_PLAYER then
--                                 entity.linear_velocity = { x = 0, y = 0 };
--                         end
--                 end
--         end
-- end


local function collide(a, b)
        if a.static == 1 and b.static == 1 then
                return;
        end

        local a_rect = {
                x = a.position.x,
                y = a.position.y,
                w = a.width,
                h = a.height
        };

        local b_rect = {
                x = b.position.x,
                y = b.position.y,
                w = b.width,
                h = b.height
        };

        local num, contact = collision2df.rectanglesx(a_rect, a.rotation, b_rect, b.rotation);
        if num <= 0 then
                return;
        end
        
        local distance = vec2f.scale(contact.normal, contact.depth);
        if a.static == 0 and b.static == 1 then
                a.position = vec2f.add(a.position, distance);
        elseif a.static == 1 and b.static == 0 then
                b.position = vec2f.sub(b.position, distance);
        else
                --a.position = vec2f.add(a.position, distance);
                a.position = vec2f.add(a.position, vec2f.scale(distance, fp.half()));
                b.position = vec2f.sub(b.position, vec2f.scale(distance, fp.half()));
        end
        --livnet.log("collide");
end

local function collide2(a, b)
        if a.static == 1 and b.static == 1 then
                return;
        end

        local a_rect = {
                x = a.position.x,
                y = a.position.y,
                w = a.width,
                h = a.height
        };

        local b_rect = {
                x = b.position.x,
                y = b.position.y,
                w = b.width,
                h = b.height
        };

        local num, contact = collision2df.rectanglesx(a_rect, a.rotation, b_rect, b.rotation);
        if num <= 0 then
                return;
        end
        local distance = vec2f.scale(contact.normal, contact.depth);
        if a.static == 0 and b.static == 1 then
                a.position = vec2f.add(a.position, distance);
        elseif a.static == 1 and b.static == 0 then
                b.position = vec2f.sub(b.position, distance);
        else
                a.position = vec2f.add(a.position, vec2f.scale(distance, fp.half()));
                b.position = vec2f.sub(b.position, vec2f.scale(distance, fp.half()));
        end
        
        --livnet.log("collide");
end

physics.collide = function()
        local len = #entities;
        for i = 1, len, 1 do
                for j = i + 1, len, 1 do
                        collide(entities[i], entities[j]);
                end
        end
end

physics.collide2 = function(entity_id)
        local len = #entities;
        local entity = entities[entity_id];
        for i = 1, len, 1 do
                if entity_id ~= i then
                        collide2(entity, entities[i]);
                end
        end
        --livnet.log("count:" .. count .. " entity_name:" .. entities[entity_id].name);
end

local function query_collision(a, b)
        if a.static == 1 and b.static == 1 then
                return false;
        end

        local a_rect = {
                x = a.position.x,
                y = a.position.y,
                w = a.width,
                h = a.height
        };

        local b_rect = {
                x = b.position.x,
                y = b.position.y,
                w = b.width,
                h = b.height
        };

        local num, contact = collision2df.rectanglesx(a_rect, a.rotation, b_rect, b.rotation);
        if num <= 0 then
                return false;
        end
        
        return true;
end

physics.query_collision = function(entity)
        local query_entities = {};
        local len = #entities;
        for i = 1, len, 1 do
                if query_collision(entity, entities[i]) then
                        table.insert(query_entities, entities[i]);
                end
        end
        return query_entities;
end



physics.destroy = function()
        entity_id = 0;
        entities = {};
        players = {};
end

return physics;
