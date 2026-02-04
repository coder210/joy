/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-proto.h
Description: Э��
Author: ydlc
Version: 1.0
Date: 2025.3.28
History:
*************************************************/
#ifndef LIV_PROTO_H
#define LIV_PROTO_H
#include <stdint.h>
#include "external/klib/kvec.h"
#include "liv-config.h"


typedef enum {
        C2S_CMD_NONE = 0x1,
        C2S_CMD_READY = 0x2,
        C2S_CMD_LOADING = 0x3,
        C2S_CMD_PLAYER_JOIN = 0x4,
        C2S_CMD_PLAYER_LEAVE = 0x5,
        C2S_CMD_PLAYER_INPUT = 0x6,
        C2S_CMD_HEARTBEAT = 0x7,
}c2s_cmd_k;

typedef struct c2s_player_join {
        int64_t position_x;
        int64_t position_y;
}c2s_player_join_t, *c2s_player_join_p;

typedef int c2s_player_leave_t, * c2s_player_leave_p;

typedef struct c2s_player_input{
        int16_t keycode;
}c2s_player_input_t, * c2s_player_input_p;

typedef struct c2s_player_live {
        int32_t conv;
        int64_t current_time;
}c2s_player_live_t, *c2s_player_live_p;

typedef struct c2s {
        c2s_cmd_k cmd;
        c2s_player_join_t player_join;
        c2s_player_leave_t player_leave;
        c2s_player_input_t player_input;
        c2s_player_live_t player_live;
}c2s_t, * c2s_p;


typedef enum {
        S2C_CMD_NONE = 0x1,
        S2C_CMD_LOADING = 0x3,
        S2C_CMD_COMMAND = 0x4,
}s2c_cmd_k;

typedef struct s2c_player_join {
        int32_t conv;
        int64_t position_x;
        int64_t position_y;
}s2c_player_join_t, *s2c_player_join_p;

typedef int s2c_player_leave_t, *s2c_player_leave_p;

typedef struct s2c_player_input {
        int32_t conv;
        int16_t keycode;
}s2c_player_input_t, *s2c_player_input_p;

typedef struct s2c_player_live {
        int32_t conv;
        int64_t current_time;
}s2c_player_live_t, * s2c_player_live_p;

typedef struct s2c_creating_emeny {
        int64_t width;
        int64_t height;
        int64_t linear_velocity_x;
        int64_t linear_velocity_y;
        int64_t position_x;
        int64_t position_y;
}s2c_creating_emeny_t, *s2c_creating_emeny_p;

typedef int s2c_destroying_emeny_t, *s2c_destroying_emeny_p;

typedef struct s2c_loading {
        int32_t frame_id;
        int32_t conv;
        int32_t data_len;
        char data[LIVNET_MAX_BUFFER];
        bool ok;
}s2c_loading_t, * s2c_loading_p;

typedef struct s2c_player {
        int32_t conv;
        uint32_t entity_id;
}s2c_player_t, * s2c_player_p;

typedef struct s2c_command {
        int32_t frame_id;
        kvec_t(s2c_player_join_t) player_joins;
        kvec_t(s2c_player_leave_t) player_leaves;
        kvec_t(s2c_player_input_t) player_inputs;
        kvec_t(s2c_creating_emeny_t) creating_emenies;
        kvec_t(s2c_destroying_emeny_t) destroying_emenies;
        char checksum[LIVNET_MAX_BUFFER];
        int32_t checksum_len;
}s2c_command_t, * s2c_command_p;


typedef struct s2c {
        s2c_cmd_k cmd;
        s2c_loading_t loading;
        s2c_command_t command;
}s2c_t, *s2c_p;


void pack_test();

int c2s_serialize(const c2s_p c2s, char* buf, int* len);
int c2s_deserialize(c2s_p c2s, const char* buf, int len);

int s2c_serialize(const s2c_p s2c, char* buf, int* len);
int s2c_deserialize(s2c_p s2c, const char* buf, int len);


#endif // !LIV_PROTO_H