#include <joy2d/utils.h>
#include "proto.h"


int c2s_serialize(const c2s_p c2s, char* buf, int* len)
{
        int offset;
        int8_t cmd;

        cmd = (int8_t)c2s->cmd;
        offset = 0;
        offset = pack_int8(buf, cmd, offset);

        if (c2s->cmd == C2S_CMD_PLAYER_JOIN) {
                offset = pack_int64(buf, c2s->player_join.position_x, offset);
                offset = pack_int64(buf, c2s->player_join.position_y, offset);
        }
        else if (c2s->cmd == C2S_CMD_PLAYER_LEAVE) {
                offset = pack_int32(buf, c2s->player_leave, offset);
        }
        else if (c2s->cmd == C2S_CMD_PLAYER_INPUT) {
                offset = pack_int32(buf, c2s->player_input.sequence, offset);
                offset = pack_int32(buf, c2s->player_input.keycode, offset);
        }
        *len = offset;
        return true;
}

int c2s_deserialize(c2s_p c2s, const char* buf, int len)
{
        int offset;
        int8_t cmd;

        offset = 0;
        offset = unpack_int8(buf, &cmd, offset);
        c2s->cmd = (c2s_cmd_k)cmd;

        if (c2s->cmd == C2S_CMD_PLAYER_JOIN) {
                offset = unpack_int64(buf, &c2s->player_join.position_x, offset);
                offset = unpack_int64(buf, &c2s->player_join.position_y, offset);
        }
        else if (c2s->cmd == C2S_CMD_PLAYER_LEAVE) {
                offset = unpack_int32(buf, &c2s->player_leave, offset);
        }
        else if (c2s->cmd == C2S_CMD_PLAYER_INPUT) {
                offset = unpack_int32(buf, &c2s->player_input.sequence, offset);
                offset = unpack_int32(buf, &c2s->player_input.keycode, offset);
        }
        else if (c2s->cmd == C2S_CMD_READY) {
        }
        else if (c2s->cmd == C2S_CMD_LOADING) {
        }
        else if (c2s->cmd == C2S_CMD_HEARTBEAT) {
        }
        else {
                return false;
        }
        return true;
}

int s2c_command_count(s2c_command_p command)
{
        int count = command->player_inputs.size();
        count += command->player_joins.size();
        count += command->player_leaves.size();
        count += command->creating_emenies.size();
        return count;
}

void s2c_command_clear(s2c_command_p command)
{
        command->player_inputs.clear();
        command->player_leaves.clear();
        command->creating_emenies.clear();
}

int s2c_serialize(const s2c_p s2c, char* buf, int* len)
{
        int offset;
        int8_t cmd;
        s2c_player_join_t player_join;
        s2c_player_leave_t player_leave;
        s2c_player_input_t player_input;
        s2c_creating_emeny_t creating_emeny;
        s2c_player_t player;

        offset = 0;
        cmd = (int8_t)s2c->cmd;
        offset = pack_int8(buf, cmd, offset);
        if (s2c->cmd == S2C_CMD_NONE) {

        }
        else if (s2c->cmd == S2C_CMD_LOADING) {
                offset = pack_int32(buf, s2c->loading.frame_id, offset);
                offset = pack_int32(buf, s2c->loading.conv, offset);
                offset = pack_int32(buf, s2c->loading.data_len, offset);
                if (s2c->loading.data_len > 0) {
                        offset = pack_string(buf, s2c->loading.data, s2c->loading.data_len, offset);
                }
        }
        else if (s2c->cmd == S2C_CMD_COMMAND) {
                offset = pack_int32(buf, s2c->command.frame_id, offset);
                offset = pack_int16(buf, s2c->command.player_joins.size(), offset);
                offset = pack_int16(buf, s2c->command.player_leaves.size(), offset);
                offset = pack_int16(buf, s2c->command.player_inputs.size(), offset);
                offset = pack_int16(buf, s2c->command.creating_emenies.size(), offset);
                for (int i = 0; i < s2c->command.player_joins.size(); i++) {
                        player_join = s2c->command.player_joins[i];
                        offset = pack_int32(buf, player_join.conv, offset);
                        offset = pack_int64(buf, player_join.position_x, offset);
                        offset = pack_int64(buf, player_join.position_y, offset);
                }
                for (int i = 0; i < s2c->command.player_leaves.size(); i++) {
                        player_leave = s2c->command.player_leaves[i];
                        offset = pack_int32(buf, player_leave, offset);
                }
                for (int i = 0; i < s2c->command.player_inputs.size(); i++) {
                        player_input = s2c->command.player_inputs[i];
                        offset = pack_int32(buf, player_input.conv, offset);
                        offset = pack_int32(buf, player_input.sequence, offset);
                        offset = pack_int16(buf, player_input.keycode, offset);
                }
                for (int i = 0; i < s2c->command.creating_emenies.size(); i++) {
                        creating_emeny = s2c->command.creating_emenies[i];
                        offset = pack_int64(buf, creating_emeny.width, offset);
                        offset = pack_int64(buf, creating_emeny.height, offset);
                        offset = pack_int64(buf, creating_emeny.linear_velocity_x, offset);
                        offset = pack_int64(buf, creating_emeny.linear_velocity_y, offset);
                        offset = pack_int64(buf, creating_emeny.position_x, offset);
                        offset = pack_int64(buf, creating_emeny.position_y, offset);
                }
                offset = pack_int32(buf, s2c->command.checksum_len, offset);
                offset = pack_string(buf, s2c->command.checksum, s2c->command.checksum_len, offset);
        }

        *len = offset;
        return 1;
}

int s2c_deserialize(s2c_p s2c, const char* buf, int len)
{
        int offset;
        int8_t cmd;
        int16_t num_player_joins, num_player_leaves, num_player_inputs, num_creating_emenies;
        s2c_player_join_t player_join;
        s2c_player_leave_t player_leave;
        s2c_player_input_t player_input;
        s2c_creating_emeny_t creating_emeny;

        offset = 0;
        offset = unpack_int8(buf, &cmd, offset);
        s2c->cmd = (s2c_cmd_k)cmd;

        if (s2c->cmd == S2C_CMD_NONE) {

        }
        else if (s2c->cmd == S2C_CMD_LOADING) {
                offset = unpack_int32(buf, &s2c->loading.frame_id, offset);
                offset = unpack_int32(buf, &s2c->loading.conv, offset);
                offset = unpack_int32(buf, &s2c->loading.data_len, offset);
                if (s2c->loading.data_len > 0) {
                        offset = unpack_string(buf, s2c->loading.data, s2c->loading.data_len, offset);
                }
        }
        else if (s2c->cmd == S2C_CMD_COMMAND) {
                offset = unpack_int32(buf, &s2c->command.frame_id, offset);
                offset = unpack_int16(buf, &num_player_joins, offset);
                offset = unpack_int16(buf, &num_player_leaves, offset);
                offset = unpack_int16(buf, &num_player_inputs, offset);
                offset = unpack_int16(buf, &num_creating_emenies, offset);
                for (int i = 0; i < num_player_joins; i++) {
                        offset = unpack_int32(buf, &player_join.conv, offset);
                        offset = unpack_int64(buf, &player_join.position_x, offset);
                        offset = unpack_int64(buf, &player_join.position_y, offset);
                        s2c->command.player_joins.push_back(player_join);
                }
                for (int i = 0; i < num_player_leaves; i++) {
                        offset = unpack_int32(buf, &player_leave, offset);
                        s2c->command.player_leaves.push_back(player_leave);
                }
                for (int i = 0; i < num_player_inputs; i++) {
                        offset = unpack_int32(buf, &player_input.conv, offset);
                        offset = unpack_int32(buf, &player_input.sequence, offset);
                        offset = unpack_int32(buf, &player_input.keycode, offset);
                        s2c->command.player_inputs.push_back(player_input);
                }
                for (int i = 0; i < num_creating_emenies; i++) {
                        offset = unpack_int64(buf, &creating_emeny.width, offset);
                        offset = unpack_int64(buf, &creating_emeny.height, offset);
                        offset = unpack_int64(buf, &creating_emeny.linear_velocity_x, offset);
                        offset = unpack_int64(buf, &creating_emeny.linear_velocity_y, offset);
                        offset = unpack_int64(buf, &creating_emeny.position_x, offset);
                        offset = unpack_int64(buf, &creating_emeny.position_y, offset);
                        s2c->command.creating_emenies.push_back(creating_emeny);
                }
                offset = unpack_int32(buf, &s2c->command.checksum_len, offset);
                offset = unpack_string(buf, s2c->command.checksum, s2c->command.checksum_len, offset);
        }

        return 1;
}

