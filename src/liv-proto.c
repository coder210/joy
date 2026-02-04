#include <SDL3/SDL.h>
#include "liv-config.h"
#include "liv-msgpack.h"
#include "liv-proto.h"


void pack_test()
{
        {
                c2s_t c2s1 = { 0 };
                c2s1.cmd = C2S_CMD_READY;
                int len = sizeof(c2s1);
                char buf[LIVNET_MIN_BUFFER];
                c2s_serialize(&c2s1, buf, &len);

                c2s_t c2s2;
                c2s_deserialize(&c2s2, buf, len);
                int t = 0;
        }


        {
                s2c_t s2c1 = { 0 };
                s2c1.cmd = S2C_CMD_LOADING;
                s2c1.loading.conv = 100;
                s2c1.loading.ok = false;
                s2c1.loading.data_len = 0;
                //SDL_memcpy(s2c1.loading.data, "liweiaaaab", s2c1.loading.data_len);

                int len;
                //int len = sizeof(s2c1);
                char buf[LIVNET_MAX_BUFFER];
                s2c_serialize(&s2c1, buf, &len);

                s2c_t s2c2;
                s2c_deserialize(&s2c2, buf, len);
                int t = 0;
        }


        {
                c2s_t c2s1 = { 0 };
                c2s1.cmd = C2S_CMD_PLAYER_JOIN;
                c2s1.player_join.position_x = 10;
                c2s1.player_join.position_y = 12;
                int len = sizeof(c2s1);
                char* buf = (char*)SDL_malloc(sizeof(char) * len);
                c2s_serialize(&c2s1, buf, &len);

                c2s_t c2s2;
                c2s_deserialize(&c2s2, buf, len);
                int t = 0;
        }

        {
                c2s_t c2s1;
                c2s1.cmd = C2S_CMD_PLAYER_INPUT;
                c2s1.player_input.keycode = 10;
                int len = sizeof(c2s_p);
                char* buf = (char*)malloc(sizeof(char) * len);
                c2s_serialize(&c2s1, buf, &len);

                c2s_t c2s2;
                c2s_deserialize(&c2s2, buf, len);
                int t = 0;
        }

        {
                /*s2c_t s2c1;

                s2c1.cmd = S2C_CMD_COMMAND;
                s2c1.command.frame_id = 1;

                s2c_init(&s2c1);

                player_join_t player_join1;
                player_join1.conv = 100;
                player_join1.position_x = 1;
                player_join1.position_y = 3;
                kv_push(player_join_t, s2c1.command.player_joins, player_join1);

                player_join_t player_join2;
                player_join2.conv = 200;
                player_join2.position_x = 21;
                player_join2.position_y = 13;
                kv_push(player_join_t, s2c1.command.player_joins, player_join2);

                player_cmd_t player_cmd1;
                player_cmd1.conv = 100;
                player_cmd1.keycode = 68;
                kv_push(player_cmd_t, s2c1.command.player_cmds, player_cmd1);

                player_cmd_t player_cmd2;
                player_cmd2.conv = 200;
                player_cmd2.keycode = 67;
                kv_push(player_cmd_t, s2c1.command.player_cmds, player_cmd2);


                s2c1.world.entity_id = 10;
                entity_t entity1;
                entity1.id = 10;
                entity1.position_x = 10;
                entity1.position_y = 10;
                entity1.linear_velocity_x = 10;
                entity1.linear_velocity_y = 10;
                entity1.rotation = 10;
                entity1.angular_velocity = 10;
                entity1.width = 10;
                entity1.height = 101;
                kv_push(entity_t, s2c1.world.entities, entity1);

                entity_t entity2;
                entity2.id = 11;
                entity2.position_x = 100;
                entity2.position_y = 100;
                entity2.linear_velocity_x = 100;
                entity2.linear_velocity_y = 100;
                entity2.rotation = 100;
                entity2.angular_velocity = 100;
                entity2.width = 100;
                entity2.height = 100;
                kv_push(entity_t, s2c1.world.entities, entity2);

                player_t player1;
                player1.conv = 100;
                player1.entity_id = 10;
                kv_push(player_t, s2c1.world.players, player1);

                player_t player2;
                player2.conv = 101;
                player2.entity_id = 10;
                kv_push(player_t, s2c1.world.players, player2);

                SDL_strlcpy(s2c1.world.checksum, "11111111", 16);
                s2c1.world.checksum_len = SDL_strlen(s2c1.world.checksum);

                char buf[LIVNET_MAX_BUFFER];
                int len;
                s2c_serialize(&s2c1, buf, &len);

                s2c_t s2c2;
                s2c_init(&s2c2);
                s2c_deserialize(&s2c2, buf, len);
                s2c_release(&s2c1);
                s2c_release(&s2c2);*/
        }
}

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
                offset = pack_int16(buf, c2s->player_input.keycode, offset);
        }
        *len = offset;
        return true;
}

int c2s_deserialize(c2s_p c2s, const char* buf, int len)
{
        int offset;
        uint8_t cmd;

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
                offset = unpack_int16(buf, &c2s->player_input.keycode, offset);
        }
        else {
                return false;
        }
        return true;
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
                offset = pack_int32(buf, s2c->loading.ok, offset);
        }
        else if (s2c->cmd == S2C_CMD_COMMAND) {
                offset = pack_int32(buf, s2c->command.frame_id, offset);
                offset = pack_int16(buf, kv_size(s2c->command.player_joins), offset);
                offset = pack_int16(buf, kv_size(s2c->command.player_leaves), offset);
                offset = pack_int16(buf, kv_size(s2c->command.player_inputs), offset);
                offset = pack_int16(buf, kv_size(s2c->command.creating_emenies), offset);
                for (int i = 0; i < kv_size(s2c->command.player_joins); i++) {
                        player_join = kv_A(s2c->command.player_joins, i);
                        offset = pack_int32(buf, player_join.conv, offset);
                        offset = pack_int64(buf, player_join.position_x, offset);
                        offset = pack_int64(buf, player_join.position_y, offset);
                }
                for (int i = 0; i < kv_size(s2c->command.player_leaves); i++) {
                        player_leave = kv_A(s2c->command.player_leaves, i);
                        offset = pack_int32(buf, player_leave, offset);
                }
                for (int i = 0; i < kv_size(s2c->command.player_inputs); i++) {
                        player_input = kv_A(s2c->command.player_inputs, i);
                        offset = pack_int32(buf, player_input.conv, offset);
                        offset = pack_int16(buf, player_input.keycode, offset);
                }
                for (int i = 0; i < kv_size(s2c->command.creating_emenies); i++) {
                        creating_emeny = kv_A(s2c->command.creating_emenies, i);
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
        uint8_t cmd;
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
                offset = unpack_int32(buf, &s2c->loading.ok, offset);
        }
        else if (s2c->cmd == S2C_CMD_COMMAND) {
                kv_init(s2c->command.player_inputs);
                kv_init(s2c->command.player_joins);
                kv_init(s2c->command.player_leaves);
                kv_init(s2c->command.creating_emenies);
                offset = unpack_int32(buf, &s2c->command.frame_id, offset);
                offset = unpack_int16(buf, &num_player_joins, offset);
                offset = unpack_int16(buf, &num_player_leaves, offset);
                offset = unpack_int16(buf, &num_player_inputs, offset);
                offset = unpack_int16(buf, &num_creating_emenies, offset);
                for (int i = 0; i < num_player_joins; i++) {
                        offset = unpack_int32(buf, &player_join.conv, offset);
                        offset = unpack_int64(buf, &player_join.position_x, offset);
                        offset = unpack_int64(buf, &player_join.position_y, offset);
                        kv_push(s2c_player_join_t, s2c->command.player_joins, player_join);
                }
                for (int i = 0; i < num_player_leaves; i++) {
                        offset = unpack_int32(buf, &player_leave, offset);
                        kv_push(s2c_player_leave_t, s2c->command.player_leaves, player_leave);
                }
                for (int i = 0; i < num_player_inputs; i++) {
                        offset = unpack_int32(buf, &player_input.conv, offset);
                        offset = unpack_int16(buf, &player_input.keycode, offset);
                        kv_push(s2c_player_input_t, s2c->command.player_inputs, player_input);
                }
                for (int i = 0; i < num_creating_emenies; i++) {
                        offset = unpack_int64(buf, &creating_emeny.width, offset);
                        offset = unpack_int64(buf, &creating_emeny.height, offset);
                        offset = unpack_int64(buf, &creating_emeny.linear_velocity_x, offset);
                        offset = unpack_int64(buf, &creating_emeny.linear_velocity_y, offset);
                        offset = unpack_int64(buf, &creating_emeny.position_x, offset);
                        offset = unpack_int64(buf, &creating_emeny.position_y, offset);
                        kv_push(s2c_creating_emeny_t, s2c->command.creating_emenies, creating_emeny);
                }
                offset = unpack_int32(buf, &s2c->command.checksum_len, offset);
                offset = unpack_string(buf, s2c->command.checksum, s2c->command.checksum_len, offset);
        }

        return 1;
}

