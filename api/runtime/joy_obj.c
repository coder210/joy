#include "joy_obj.h"

#include <string.h>

#define JOY_OBJ_CAP 4096

typedef struct joy_obj_slot {
        void*          ptr;
        const char*    tag;
        joy_obj_dtor   dtor;
        const char*    api;
        uint32_t       refs;
        uint32_t       gen;
        int            used;
} joy_obj_slot;

static joy_obj_slot s_slots[JOY_OBJ_CAP];
static uint32_t s_live = 0;
static uint32_t s_destroyed = 0;
static uint32_t s_cursor = 0;

/* 句柄 = generation(高 32 位) | (index+1)(低 32 位)，0 恒为无效。 */
static uint64_t encode(uint32_t idx, uint32_t gen)
{
        return (((uint64_t)gen) << 32) | (uint64_t)(idx + 1);
}

static int decode(uint64_t h, uint32_t* idx, uint32_t* gen)
{
        uint32_t low;
        if (h == 0) return 0;
        low = (uint32_t)(h & 0xffffffffu);
        if (low == 0 || low > JOY_OBJ_CAP) return 0;
        *idx = low - 1;
        *gen = (uint32_t)(h >> 32);
        return 1;
}

uint64_t joy_obj_register(void* ptr, const char* tag, joy_obj_dtor dtor, const char* api)
{
        uint32_t i;
        uint32_t n;
        joy_obj_slot* s;
        if (!ptr || !tag) return 0;
        for (n = 0; n < JOY_OBJ_CAP; n++) {
                i = (s_cursor + n) % JOY_OBJ_CAP;
                if (!s_slots[i].used) {
                        s = &s_slots[i];
                        s->ptr = ptr;
                        s->tag = tag;
                        s->dtor = dtor;
                        s->api = api;
                        s->refs = 1;
                        s->gen++;
                        if (s->gen == 0) s->gen = 1;
                        s->used = 1;
                        s_live++;
                        s_cursor = i + 1;
                        return encode(i, s->gen);
                }
        }
        return 0;
}

static joy_obj_slot* lookup(uint64_t handle, const char* tag, joy_status_t* st)
{
        uint32_t idx;
        uint32_t gen;
        joy_obj_slot* s;
        if (!decode(handle, &idx, &gen)) {
                if (st) *st = JOY_ERR_HANDLE;
                return NULL;
        }
        s = &s_slots[idx];
        if (!s->used || s->gen != gen || !s->ptr) {
                if (st) *st = JOY_ERR_HANDLE;
                return NULL;
        }
        if (tag && strcmp(s->tag, tag) != 0) {
                if (st) *st = JOY_ERR_TYPE;
                return NULL;
        }
        return s;
}

void* joy_obj_deref(uint64_t handle, const char* tag, joy_status_t* st)
{
        joy_obj_slot* s = lookup(handle, tag, st);
        if (!s) return NULL;
        if (st) *st = JOY_OK;
        return s->ptr;
}

void joy_obj_retain(uint64_t handle)
{
        uint32_t idx;
        uint32_t gen;
        joy_obj_slot* s;
        if (!decode(handle, &idx, &gen)) return;
        s = &s_slots[idx];
        if (!s->used || s->gen != gen) return;
        s->refs++;
}

void joy_obj_release(uint64_t handle)
{
        uint32_t idx;
        uint32_t gen;
        joy_obj_slot* s;
        if (!decode(handle, &idx, &gen)) return;
        s = &s_slots[idx];
        if (!s->used || s->gen != gen || s->refs == 0) return;
        s->refs--;
        if (s->refs > 0) return;
        if (s->dtor) s->dtor(s->ptr);
        s_destroyed++;
        /* 保留 generation：清零会让槽位复用后旧句柄重新匹配上新对象。 */
        {
                uint32_t keep_gen = s->gen;
                memset(s, 0, sizeof(*s));
                s->gen = keep_gen;
        }
        if (s_live > 0) s_live--;
}

uint32_t joy_obj_live_count(void)     { return s_live; }
uint32_t joy_obj_destroy_count(void)  { return s_destroyed; }
