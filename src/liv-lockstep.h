/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-lockstep.h
Description: 
Author: ydlc
Version: 1.0
Date: 2025.10.30
History:
*************************************************/
#ifndef LIV_LOCKSTEP_H
#define LIV_LOCKSTEP_H
#include <stddef.h>
#include <stdint.h>

typedef struct lockstep lockstep_t, *lockstep_p;

lockstep_p lockstep_create();
void lockstep_destroy(lockstep_p lockstep);
void lockstep_add_command(lockstep_p lockstep, 
        int frameid, 
        const char* cmd, size_t size);
void lockstep_add_world(lockstep_p lockstep, int frameid, 
        const char* world, size_t size);

#endif // !LIV_LOCKSTEP_H
