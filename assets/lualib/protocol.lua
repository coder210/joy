local livnet = requirex "livnet"

local constant = livnet.constant;

local protocol = {};

function protocol.ready(conv)
        local packet = {};
        packet.cmd = constant.CMD_READY;
        packet.cn = conv;
        return cjson.encode(packet);
end     

function protocol.load_scene(conv, data, ok)
        local packet = {};
        packet.cmd = constant.CMD_LOAD_SCENE;
        packet.cn = conv;
        packet.data = data;
        packet.ok = ok;
        return cjson.encode(packet);
end

function protocol.player_join(conv, px, py)
        local packet = {};
        packet.cmd = constant.CMD_PLAYER_JOIN;
        packet.cn = conv;
        packet.px = fp.from_float(px);
        packet.py = fp.from_float(py);
        return cjson.encode(packet);
end

function protocol.player_leavex(conv)
        local packet = {};
        packet.cmd = constant.CMD_PLAYER_LEAVE;
        packet.cn = conv;
        return packet;
end

function protocol.player_leave(conv)
        local packet = {};
        packet.cmd = constant.CMD_PLAYER_LEAVE;
        packet.cn = conv;
        return cjson.encode(packet);
end

function protocol.player_command(conv, opkey)
        local packet = {};
        packet.cmd = constant.CMD_PLAYER_COMMAND;
        packet.cn = conv;
        packet.code = opkey;
        return cjson.encode(packet);
end

function protocol.keep_health(conv)
        local packet = {};
        packet.cmd = constant.CMD_HEALTH;
        packet.cn = conv;
        packet.current_time = utils.current_time();
        return cjson.encode(packet);
end

return protocol;