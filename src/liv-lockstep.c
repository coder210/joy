#include "liv-lockstep.h"
#include "external/klib/khash.h"
#include "liv-log.h"
#include "SDL3/SDL.h"

KHASH_INIT(kcommand, int, char*, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(kworld, int, char*, 1, kh_int_hash_func, kh_int_hash_equal)

typedef struct lockstep {
        khash_t(kcommand)* cmds;
        khash_t(kworld)* worlds;
}lockstep_t, *lockstep_p;

lockstep_p lockstep_create()
{
        lockstep_p lockstep;
        lockstep = (lockstep_p)SDL_malloc(sizeof(lockstep_t));
        lockstep->cmds = kh_init(kcommand);
        lockstep->worlds = kh_init(kworld);
        return lockstep;
}

void lockstep_destroy(lockstep_p lockstep)
{
        char *command, *world;
        khint_t k;
        for (k = kh_begin(lockstep->cmds); k != kh_end(lockstep->cmds); k++) {
                if (kh_exist(lockstep->cmds, k)) {
                        command = kh_val(lockstep->cmds, k);
                        SDL_free(command);
                }
        }
        for (k = kh_begin(lockstep->worlds); k != kh_end(lockstep->worlds); k++) {
                if (kh_exist(lockstep->worlds, k)) {
                        world = kh_val(lockstep->worlds, k);
                        SDL_free(world);
                }
        }
        kh_destroy(kcommand, lockstep->cmds);
        kh_destroy(kworld, lockstep->worlds);
}

void 
lockstep_add_command(lockstep_p lockstep, 
        int frameid, 
        const char* cmd, size_t size)
{
        khint_t k;
        char* new_cmd;
        int ret;

        k = kh_get(kcommand, lockstep->cmds, frameid);
        if (k == kh_end(lockstep->cmds)) {
                new_cmd = (char*)SDL_malloc(sizeof(char) * size);
                SDL_memcpy(new_cmd, cmd, size);
                k = kh_put(kcommand, lockstep->cmds, k, &ret);
                kh_val(lockstep->cmds, k) = new_cmd;
        }
        else {
                liv_log_error("The key '%d' is exists.", frameid);
        }
}

void 
lockstep_add_world(lockstep_p lockstep, 
        int frameid, 
        const char* world, size_t size)
{
        khint_t k;
        char* new_world;
        int ret;

        k = kh_get(kworld, lockstep->worlds, frameid);
        if (k == kh_end(lockstep->worlds)) {
                new_world = (char*)SDL_malloc(sizeof(char) * size);
                SDL_memcpy(new_world, world, size);
                k = kh_put(kworld, lockstep->worlds, k, &ret);
                kh_val(lockstep->worlds, k) = new_world;
        }
        else {
                liv_log_error("The key '%d' is exists.", frameid);
        }
}

