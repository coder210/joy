#include <stdarg.h>
#include "liv-ecs.h"
#include "liv-config.h"
#include "liv-hash.h"
#include "./external/klib/khash.h"
#include "./external/klib/klist.h"
#include "./external/klib/kvec.h"
#include "SDL3/SDL.h"

// ==================== Bitset实现 ====================
typedef struct bitset {
        uint64_t* words;
        int nwords;      // 当前word数量
        int capacity;    // 当前能表示的最大实体ID+1
} bitset_t;

static bitset_t* bitset_create(int initial_capacity) {
        bitset_t* bs = SDL_malloc(sizeof(bitset_t));
        // 初始容量向上取整到64的倍数
        bs->nwords = (initial_capacity + 63) / 64;
        bs->capacity = bs->nwords * 64;
        bs->words = SDL_calloc(bs->nwords, sizeof(uint64_t));
        return bs;
}

static void bitset_destroy(bitset_t* bs) {
        if (bs) {
                SDL_free(bs->words);
                SDL_free(bs);
        }
}

static void bitset_ensure_capacity(bitset_t* bs, int min_capacity) {
        if (min_capacity > bs->capacity) {
                int new_nwords = (min_capacity + 63) / 64;
                uint64_t* new_words = SDL_realloc(bs->words, new_nwords * sizeof(uint64_t));
                // 新分配的部分清零
                for (int i = bs->nwords; i < new_nwords; i++) {
                        new_words[i] = 0;
                }
                bs->words = new_words;
                bs->nwords = new_nwords;
                bs->capacity = new_nwords * 64;
        }
}

static void bitset_set(bitset_t* bs, int id) {
        if (id < 0) return;
        bitset_ensure_capacity(bs, id + 1);
        int word = id / 64;
        int bit = id % 64;
        bs->words[word] |= (1ULL << bit);
}

static void bitset_clear(bitset_t* bs, int id) {
        if (id < 0 || id >= bs->capacity) return;
        int word = id / 64;
        int bit = id % 64;
        bs->words[word] &= ~(1ULL << bit);
}

static bool bitset_test(bitset_t* bs, int id) {
        if (id < 0 || id >= bs->capacity) return false;
        int word = id / 64;
        int bit = id % 64;
        return (bs->words[word] & (1ULL << bit)) != 0;
}

static void bitset_and(bitset_t* dest, bitset_t* src) {
        int min_words = dest->nwords < src->nwords ? dest->nwords : src->nwords;
        for (int i = 0; i < min_words; i++) {
                dest->words[i] &= src->words[i];
        }
        // dest中超出src的部分全部清零
        for (int i = min_words; i < dest->nwords; i++) {
                dest->words[i] = 0;
        }
}

static void bitset_copy(bitset_t* dest, bitset_t* src) {
        bitset_ensure_capacity(dest, src->capacity);
        int words_to_copy = dest->nwords < src->nwords ? dest->nwords : src->nwords;
        SDL_memcpy(dest->words, src->words, words_to_copy * sizeof(uint64_t));
        // 如果dest更大，多余部分保持原样
}

// ==================== Sparse Set with Bitset ====================
KHASH_INIT(ksparse, int, int, 1, kh_int_hash_func, kh_int_hash_equal)

typedef struct sparse_set {
        kvec_t(int) density;
        khash_t(ksparse)* sparse;
        bitset_t* bitset;  // 新增位集
} sparse_set_t, * sparse_set_p;

static sparse_set_p sparse_set_create() {
        sparse_set_p sparse_set = (sparse_set_p)SDL_malloc(sizeof(sparse_set_t));
        kv_init(sparse_set->density);
        sparse_set->sparse = kh_init(ksparse);
        sparse_set->bitset = bitset_create(256);  // 初始容量256个实体
        return sparse_set;
}

static void sparse_set_destroy(sparse_set_p sparse_set) {
        kv_destroy(sparse_set->density);
        kh_destroy(ksparse, sparse_set->sparse);
        bitset_destroy(sparse_set->bitset);
        SDL_free(sparse_set);
}

static bool sparse_set_contain(sparse_set_p sparse_set, int value) {
        khint_t k = kh_get(ksparse, sparse_set->sparse, value);
        return k != kh_end(sparse_set->sparse);
}

static void sparse_set_remove(sparse_set_p sparse_set, int value) {
        khint_t k = kh_get(ksparse, sparse_set->sparse, value);
        if (k != kh_end(sparse_set->sparse)) {
                int index = kh_val(sparse_set->sparse, k);
                int len = kv_size(sparse_set->density);
                int last_value = kv_A(sparse_set->density, len - 1);

                kv_A(sparse_set->density, index) = last_value;
                kv_pop(sparse_set->density);

                kh_del(ksparse, sparse_set->sparse, k);
                bitset_clear(sparse_set->bitset, value);

                // 更新被移动元素的sparse映射
                if (value != last_value) {
                        khint_t k2 = kh_get(ksparse, sparse_set->sparse, last_value);
                        if (k2 != kh_end(sparse_set->sparse)) {
                                kh_val(sparse_set->sparse, k2) = index;
                        }
                }
        }
}

static void sparse_set_insert(sparse_set_p sparse_set, int value) {
        if (!sparse_set_contain(sparse_set, value)) {
                kv_push(int, sparse_set->density, value);
                int index = kv_size(sparse_set->density) - 1;
                int ret;
                khint_t k = kh_put(ksparse, sparse_set->sparse, value, &ret);
                kh_val(sparse_set->sparse, k) = index;
                bitset_set(sparse_set->bitset, value);
        }
}


typedef struct ecs_component_data {
        size_t size;
        void* data;
}ecs_component_data_t, *ecs_component_data_p;

typedef unsigned long ecs_cid_t;
#define __int_free(x)
KLIST_INIT(kecs_system, ecs_system_t, __int_free)

// 组件存储现在包含稀疏集和位集
typedef struct component_store {
        sparse_set_p sparse_set;
        bitset_t* bitset;
} component_store_t, *component_store_p;

KHASH_INIT(kecs_component, ecs_cid_t, component_store_p, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(kecs_component_data, ecs_cid_t, ecs_component_data_p, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(kecs_meta, ecs_cid_t, size_t, 1, kh_int_hash_func, kh_int_hash_equal)

typedef khash_t(kecs_component_data) ecs_entity_t, * ecs_entity_p;
KHASH_INIT(kecs_entity, ecs_id_t, ecs_entity_p, 1, kh_int_hash_func, kh_int_hash_equal)

typedef struct ecs_world {
        ecs_id_t global_id;
        khash_t(kecs_meta)* metas;
        klist_t(kecs_system)* systems;
        khash_t(kecs_component)* components;
        khash_t(kecs_entity)* entities;
} ecs_world_t, * ecs_world_p;

ecs_world_p liv_ecs_create() 
{
        ecs_world_p world = (ecs_world_p)SDL_malloc(sizeof(ecs_world_t));
        world->metas = kh_init(kecs_meta);
        world->systems = kl_init(kecs_system);
        world->components = kh_init(kecs_component);
        world->entities = kh_init(kecs_entity);
        world->global_id = 1;
        return world;
}


void liv_ecs_destroy(ecs_world_p world)
{
        ecs_id_t id;
        ecs_entity_p entity;
        khint_t i, j, p;
        sparse_set_p sparse_set;
        ecs_component_data_p component_data;

        // 销毁实体
        for (khint_t i = kh_begin(world->entities); i != kh_end(world->entities); i++) {
                if (kh_exist(world->entities, i)) {
                        ecs_entity_p entity = kh_val(world->entities, i);
                        // 销毁实体组件数据
                        for (khint_t j = kh_begin(entity); j != kh_end(entity); j++) {
                                if (kh_exist(entity, j)) {
                                        component_data = kh_val(entity, j);
                                        SDL_free(component_data->data);
                                        SDL_free(component_data);
                                }
                        }
                        kh_destroy(kecs_component_data, entity);
                }
        }

        // 销毁组件存储（包括稀疏集和位集）
        for (khint_t i = kh_begin(world->components); i != kh_end(world->components); i++) {
                if (kh_exist(world->components, i)) {
                        component_store_t* store = kh_val(world->components, i);
                        sparse_set_destroy(store->sparse_set);
                        SDL_free(store);
                }
        }

        kl_destroy(kecs_system, world->systems);
        kh_destroy(kecs_meta, world->metas);
        kh_destroy(kecs_entity, world->entities);
        kh_destroy(kecs_component, world->components);
        SDL_free(world);
}

void liv_ecs_process(ecs_world_p world, float dt)
{
        ecs_system_t system;
        kliter_t(kecs_system)* p;
        p = kl_begin(world->systems);
        while (p != kl_end(world->systems)) {
                system = kl_val(p);
                system.cb(world, system.arg, dt);
                p = kl_next(p);
        }
}

ecs_id_t liv_ecs_spawn(ecs_world_p world)
{
        ecs_id_t id;
        khint_t k;
        int ret;
        id = world->global_id++;
        k = kh_put(kecs_entity, world->entities, id, &ret);
        kh_val(world->entities, k) = kh_init(kecs_component_data);
        return id;
}

void liv_ecs_kill(ecs_world_p world, ecs_id_t id)
{
        ecs_entity_p entity;
        component_store_t* store;
        ecs_cid_t component_id;
        khint_t k, p;
        ecs_component_data_p component_data;

        k = kh_get(kecs_entity, world->entities, id);
        if (k == kh_end(world->entities)) {
                return;
        }

        entity = kh_val(world->entities, k);
        kh_del(kecs_entity, world->entities, k);

        // 从所有组件中移除该实体
        for (k = kh_begin(entity); k != kh_end(entity); k++) {
                if (kh_exist(entity, k)) {
                        component_id = kh_key(entity, k);
                        component_data = kh_val(entity, k);
                        SDL_free(component_data->data);
                        SDL_free(component_data);

                        p = kh_get(kecs_component, world->components, component_id);
                        if (p != kh_end(world->components)) {
                                store = kh_val(world->components, p);
                                sparse_set_remove(store->sparse_set, id);
                                bitset_clear(store->bitset, id);
                        }
                }
        }
        kh_destroy(kecs_component_data, entity);
}

void liv_ecs_define(ecs_world_p world, const char* name, size_t size)
{
        ecs_cid_t component_id;
        khint_t k;
        int ret;
        component_id = murmur3_hash(name);
        k = kh_get(kecs_meta, world->metas, component_id);
        if (k == kh_end(world->metas)) {
                k = kh_put(kecs_meta, world->metas, component_id, &ret);
                kh_val(world->metas, k) = size;
        }
}

void liv_ecs_set(ecs_world_p world, ecs_id_t id, const char* name, const void* ptr, size_t size)
{
        ecs_entity_p entity;
        ecs_cid_t component_id;
        component_store_t* store;
        size_t meta;
        khint_t k;
        ecs_component_data_p component_data;
        int ret;

        component_id = murmur3_hash(name);
        k = kh_get(kecs_meta, world->metas, component_id);
        if (k == kh_end(world->metas))
                return;
        meta = kh_val(world->metas, k);

        k = kh_get(kecs_entity, world->entities, id);
        if (k == kh_end(world->entities))
                return;

        entity = kh_val(world->entities, k);
        k = kh_get(kecs_component_data, entity, component_id);

        if (k == kh_end(entity)) {
                component_data = (ecs_component_data_p)SDL_malloc(sizeof(ecs_component_data_t));
                component_data->data = SDL_malloc(meta);
                component_data->size = size;
                SDL_memcpy(component_data->data, ptr, component_data->size);
                k = kh_put(kecs_component_data, entity, component_id, &ret);
                kh_val(entity, k) = component_data;

                // 更新组件存储
                k = kh_get(kecs_component, world->components, component_id);
                if (k == kh_end(world->components)) {
                        k = kh_put(kecs_component, world->components, component_id, &ret);
                        store = (component_store_p)SDL_malloc(sizeof(component_store_t));
                        store->sparse_set = sparse_set_create();
                        store->bitset = bitset_create(world->global_id + 64); // 预留空间
                        kh_val(world->components, k) = store;
                }
                else {
                        store = kh_val(world->components, k);
                }

                sparse_set_insert(store->sparse_set, id);
                bitset_set(store->bitset, id);
        }
        else {
                component_data = kh_val(entity, k);
                component_data->size = size;
                SDL_memcpy(component_data->data, ptr, component_data->size);
        }
}

size_t liv_ecs_get(ecs_world_p world, ecs_id_t id, const char* name, void** data)
{
        ecs_cid_t component_id;
        ecs_entity_p entity;
        ecs_component_data_p component_data;
        khint_t k;
        component_id = murmur3_hash(name);
        k = kh_get(kecs_entity, world->entities, id);
        if (k == kh_end(world->entities)) {
                return 0;
        }
        entity = kh_val(world->entities, k);
        k = kh_get(kecs_component_data, entity, component_id);
        if (k == kh_end(entity)) {
                return 0;
        }
        component_data = kh_val(entity, k);
        *data = component_data->data;
        return component_data->size;
}

void liv_ecs_remove(ecs_world_p world, ecs_id_t id, const char* name)
{
        ecs_entity_p entity;
        component_store_t* store;
        ecs_component_data_p component_data;
        khint_t k;

        ecs_cid_t component_id = murmur3_hash(name);
        k = kh_get(kecs_entity, world->entities, id);
        if (k != kh_end(world->entities)) {
                entity = kh_val(world->entities, k);
                k = kh_get(kecs_component_data, entity, component_id);
                if (k != kh_end(entity)) {
                        component_data = kh_val(entity, k);
                        SDL_free(component_data->data);
                        SDL_free(component_data);
                        kh_del(kecs_component_data, entity, k);
                }
        }

        k = kh_get(kecs_component, world->components, component_id);
        if (k != kh_end(world->components)) {
                store = kh_val(world->components, k);
                sparse_set_remove(store->sparse_set, id);
                bitset_clear(store->bitset, id);
        }
}
//
//static liv_vector_t(kecs)
//_intersection(ecs_id_t* a_arr, int a_arr_len, ecs_id_t* b_arr, int b_arr_len)
//{
//        liv_vector_t(kecs) vec;
//        ecs_id_t a, b;
//
//        liv_vector_init(kecs, &vec);
//        for (size_t i = 0; i < a_arr_len; i++) {
//                a = a_arr[i];
//                for (size_t j = 0; j < b_arr_len; j++) {
//                        b = b_arr[j];
//                        if (a == b) {
//                                liv_vector_push(kecs, &vec, a);
//                                break;
//                        }
//                }
//        }
//
//        return vec;
//}

liv_vector_t(kecs) liv_ecs_query(ecs_world_p world, int arg_cnt, ...)
{
        if (arg_cnt <= 0) {
                liv_vector_t(kecs) empty;
                liv_vector_init(kecs, &empty);
                return empty;
        }

        const char** args = SDL_malloc(arg_cnt * sizeof(const char*));
        va_list va;
        va_start(va, arg_cnt);

        for (int i = 0; i < arg_cnt; i++) {
                args[i] = va_arg(va, const char*);
        }
        va_end(va);

        liv_vector_t(kecs) result = liv_ecs_queryx(world, arg_cnt, args);
        SDL_free(args);

        return result;
}

liv_vector_t(kecs) liv_ecs_queryx(ecs_world_p world, int arg_cnt, const char**args) 
{
        liv_vector_t(kecs) result;
        liv_vector_init(kecs, &result);

        if (arg_cnt <= 0) {
                return result;
        }

        // 创建临时位集用于交集运算
        bitset_t* temp_bitset = bitset_create(world->global_id + 64);
        bitset_t* result_bitset = bitset_create(world->global_id + 64);
        bool first = true;

        for (int i = 0; i < arg_cnt; ++i) {
                const char* comp_name = args[i];
                ecs_cid_t component_id = murmur3_hash(comp_name);
                khint_t k = kh_get(kecs_component, world->components, component_id);

                if (k == kh_end(world->components)) {
                        // 有一个组件不存在，结果为空
                        liv_vector_destroy(kecs, &result);
                        bitset_destroy(temp_bitset);
                        bitset_destroy(result_bitset);
                        liv_vector_init(kecs, &result);
                        return result;
                }

                component_store_t* store = kh_val(world->components, k);

                if (first) {
                        // 第一个组件，复制其位集
                        bitset_copy(result_bitset, store->bitset);
                        first = false;
                }
                else {
                        // 后续组件，与结果位集取交集
                        bitset_copy(temp_bitset, store->bitset);
                        bitset_and(result_bitset, temp_bitset);
                }
        }

        // 从结果位集中提取实体ID
        // 为了提高效率，我们可以遍历第一个组件的实体列表，而不是遍历整个位集
        if (!first) {
                const char* first_comp = args[0];
                ecs_cid_t first_cid = murmur3_hash(first_comp);
                khint_t k = kh_get(kecs_component, world->components, first_cid);

                if (k != kh_end(world->components)) {
                        component_store_t* store = kh_val(world->components, k);
                        sparse_set_p sparse_set = store->sparse_set;

                        // 遍历第一个组件的实体，检查是否在结果位集中
                        for (int i = 0; i < kv_size(sparse_set->density); i++) {
                                ecs_id_t id = kv_A(sparse_set->density, i);
                                if (bitset_test(result_bitset, id)) {
                                        liv_vector_push(kecs, &result, id);
                                }
                        }
                }
        }

        bitset_destroy(temp_bitset);
        bitset_destroy(result_bitset);
        return result;
}


void liv_ecs_register(ecs_world_p world, ecs_system_t system)
{
        *kl_pushp(kecs_system, world->systems) = system;
}


typedef struct position {
        float x, y;
}position_t;

typedef struct velocity {
        float x, y;
}velocity_t;


int liv_ecs_test()
{
        sparse_set_p sparse_set;
        sparse_set = sparse_set_create();

        for (int i = 0; i < 100000; i++) {
                sparse_set_insert(sparse_set, i);
        }

        sparse_set_remove(sparse_set, 1000);

        int index = 1000;
        bool found = false;
       /* if (!sparse_set_contian(sparse_set, index)) {
                SDL_Log("%d not exists", index);
                for (size_t i = 0; i < kv_size(sparse_set->density); i++) {
                        int value = kv_A(sparse_set->density, i);
                        if (value == index) {
                                found = true;
                                break;
                        }

                }

                if (!found) {
                        SDL_Log("%d not exists 2", index);
                }
        }
        sparse_set_destroy(sparse_set);*/


        ecs_world_p world;
        world = liv_ecs_create();
        liv_ecs_define(world, "position_t", sizeof(position_t));
        liv_ecs_define(world, "velocity_t", sizeof(velocity_t));

        ecs_id_t id1 = liv_ecs_spawn(world);
        ecs_id_t id2 = liv_ecs_spawn(world);
        /*liv_ecs_set(world, id1, "position_t", &(position_t){0.2, 0.3});
        liv_ecs_set(world, id1, "velocity_t", &(velocity_t){2.2, 4.3});
        liv_ecs_set(world, id2, "position_t", &(position_t){2.2, 3.3});
        liv_ecs_set(world, id2, "velocity_t", &(velocity_t){3.3, 4.4});*/

        liv_ecs_remove(world, id1, "position_t");
        //liv_ecs_set(world, id1, "velocity_t", &(velocity_t){2, 5});
        //liv_ecs_kill(world, id2);

        /*  liv_vector_t entities = liv_ecs_query(world, 2, "position_t", "velocity_t");
          for (int i = 0; i < liv_vector_size(&entities); i++) {
                  ecs_id_t id = liv_vector_a(&entities, i);
                  position_t* position = (position_t*)liv_ecs_get(world, id, "position_t");
                  velocity_t* velocity = (velocity_t*)liv_ecs_get(world, id, "velocity_t");
                  SDL_Log("%d <%f,%f> <%f,%f>", id, position->x, position->y, velocity->x, velocity->y);
          }*/
        liv_vector_t(kecs) entities = liv_ecs_query(world, 1, "position_t");
        for (int i = 0; i < liv_vector_size(kecs, &entities); i++) {
                ecs_id_t id = liv_vector_a(kecs, &entities, i);
                //position_t* position = (position_t*)liv_ecs_get(world, id, "position_t");
                //SDL_Log("%d <%f,%f>", id, position->x, position->y);
        }



        liv_ecs_destroy(world);
        return 0;
}
