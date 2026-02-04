#include "SDL3/SDL.h"
#include "external/klib/khash.h"
#include "liv-msgpack.h"
#include "liv-physics.h"
#include "liv-utils.h"

KHASH_INIT(krigidbodies, uint32_t, rigidbody_p, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(kconnections, int, uint32_t, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(kenemies, int, enemy_p, 1, kh_int_hash_func, kh_int_hash_equal)

typedef struct world {
        uint32_t entity_id;
        khash_t(krigidbodies)* rigidbodies;
        khash_t(kconnections)* connections;
        khash_t(kenemies)* enemies;
}world2df_t, * world2df_p;


world2df_p world2df_create()
{
        world2df_p world;
        world = SDL_malloc(sizeof(world2df_t));
        world->rigidbodies = kh_init(krigidbodies);
        world->connections = kh_init(kconnections);
        world->enemies = kh_init(kenemies);
        world->entity_id = 100;
        return world;
}

void world2df_destroy(world2df_p world)
{
        world->entity_id = 100;
        kh_destroy(krigidbodies, world->rigidbodies);
        kh_destroy(kconnections, world->connections);
        kh_destroy(kenemies, world->enemies);
        SDL_free(world);
}

void world2df_set_entityid(world2df_p world, uint32_t entity_id)
{
        world->entity_id = entity_id;
}

uint32_t world2df_get_entityid(world2df_p world)
{
        return world->entity_id;
}

int world2df_checksum(const world2df_p world, char* output)
{
        char buf[LIVNET_MAX_BUFFER];
        khint_t p, k;
        rigidbody_p body;
        int total_len, id;
        char tmp[64];
        total_len = sprintf(buf, "%u;", world->entity_id);
        for (p = kh_begin(world->connections); p != kh_end(world->connections); p++) {
                if (kh_exist(world->connections, p)) {
                        id = kh_val(world->connections, p);
                        k = kh_get(krigidbodies, world->rigidbodies, id);
                        if (k != kh_end(world->rigidbodies)) {
                                body = kh_val(world->rigidbodies, k);
                                int len = sprintf(tmp, "%u,%lld,%lld,%lld,%lld,%lld;", body->id,
                                        body->position.x, body->position.y, body->linear_velocity.x, body->linear_velocity.y, body->rotation);
                                if (len > 0) {
                                        SDL_memcpy(buf + total_len, tmp, len);
                                        total_len += len;
                                }
                        }
                }
        }
        //buf[total_len] = 0;
        //printf("%s\n", buf);
        return utils_md5x(buf, total_len, output);
        //SDL_memcpy(output, buf, total_len);
        //return total_len;
}

int world2df_serialize(const world2df_p world, char* output)
{
        int offset = 0;
        uint32_t entity_id;
        int conv;
        rigidbody_p rigidbody;
        enemy_p enemy;
        khint_t p;
        offset = pack_uint32(output, world->entity_id, offset);
        offset = pack_int16(output, kh_size(world->rigidbodies), offset);
        offset = pack_int16(output, kh_size(world->connections), offset);
        offset = pack_int16(output, kh_size(world->enemies), offset);
        for (p = kh_begin(world->rigidbodies); p != kh_end(world->rigidbodies); p++) {
                if (kh_exist(world->rigidbodies, p)) {
                        entity_id = kh_key(world->rigidbodies, p);
                        rigidbody = kh_val(world->rigidbodies, p);
                        offset = pack_uint32(output, rigidbody->id, offset);
                        offset = pack_int32(output, rigidbody->is_static, offset);
                        offset = pack_int64(output, rigidbody->position.x, offset);
                        offset = pack_int64(output, rigidbody->position.y, offset);
                        offset = pack_int64(output, rigidbody->linear_velocity.x, offset);
                        offset = pack_int64(output, rigidbody->linear_velocity.y, offset);
                        offset = pack_int64(output, rigidbody->rotation, offset);
                        offset = pack_int64(output, rigidbody->angular_velocity, offset);
                        offset = pack_int64(output, rigidbody->width, offset);
                        offset = pack_int64(output, rigidbody->height, offset);
                }
        }
        for (p = kh_begin(world->enemies); p != kh_end(world->enemies); p++) {
                if (kh_exist(world->enemies, p)) {
                        enemy = kh_val(world->enemies, p);
                        offset = pack_uint32(output, enemy->id, offset);
                        offset = pack_int32(output, enemy->hp, offset);
                }
        }
        for (p = kh_begin(world->connections); p != kh_end(world->connections); p++) {
                if (kh_exist(world->connections, p)) {
                        conv = kh_key(world->connections, p);
                        entity_id = kh_val(world->connections, p);
                        offset = pack_int32(output, conv, offset);
                        offset = pack_uint32(output, entity_id, offset);
                }
        }
        return offset;
}

int world2df_deserialize(world2df_p world, const char* data, int len)
{
        int offset, ret;
        short num_rigidbodies, num_connections, num_enemies;
        rigidbody_p rigidbody;
        enemy_p enemy;
        uint32_t entity_id;
        int conv;
        khint_t k;

        offset = 0;
        offset = unpack_int32(data, &world->entity_id, offset);
        offset = unpack_int16(data, &num_rigidbodies, offset);
        offset = unpack_int16(data, &num_connections, offset);
        offset = unpack_int16(data, &num_enemies, offset);

        for (int i = 0; i < num_rigidbodies; i++) {
                rigidbody = (rigidbody_p)SDL_malloc(sizeof(rigidbody_t));
                if (rigidbody) {
                        offset = unpack_int32(data, &rigidbody->id, offset);
                        offset = unpack_int32(data, &rigidbody->is_static, offset);
                        offset = unpack_int64(data, &rigidbody->position.x, offset);
                        offset = unpack_int64(data, &rigidbody->position.y, offset);
                        offset = unpack_int64(data, &rigidbody->linear_velocity.x, offset);
                        offset = unpack_int64(data, &rigidbody->linear_velocity.y, offset);
                        offset = unpack_int64(data, &rigidbody->rotation, offset);
                        offset = unpack_int64(data, &rigidbody->angular_velocity, offset);
                        offset = unpack_int64(data, &rigidbody->width, offset);
                        offset = unpack_int64(data, &rigidbody->height, offset);
                        k = kh_put(krigidbodies, world->rigidbodies, rigidbody->id, &ret);
                        kh_val(world->rigidbodies, k) = rigidbody;
                }
        }
        for (int i = 0; i < num_enemies; i++) {
                enemy = (enemy_p)SDL_malloc(sizeof(enemy_t));
                offset = unpack_uint32(data, &enemy->id, offset);
                offset = unpack_int32(data, &enemy->hp, offset);
                k = kh_put(kenemies, world->enemies, enemy->id, &ret);
                kh_val(world->enemies, k) = enemy;
        }
        for (int i = 0; i < num_connections; i++) {
                offset = unpack_int32(data, &conv, offset);
                offset = unpack_int32(data, &entity_id, offset);
                k = kh_put(kconnections, world->connections, conv, &ret);
                kh_val(world->connections, k) = entity_id;
        }
       
        return offset;
}

rigidbody_p
world2df_create_rigidbody(world2df_p world, fp_t width, fp_t height)
{
        khint_t k;
        rigidbody_p body;
        int ret;

        body = (rigidbody_p)SDL_malloc(sizeof(rigidbody_t));
        SDL_assert(body);
        SDL_memset(body, 0, sizeof(rigidbody_t));

        body->id = world->entity_id++;
        body->is_static = 1;
        body->width = width;
        body->height = height;
        k = kh_get(krigidbodies, world->rigidbodies, body->id);
        if (k == kh_end(world->rigidbodies)) {
                k = kh_put(krigidbodies, world->rigidbodies, body->id, &ret);
                kh_val(world->rigidbodies, k) = body;
        }
        return body;
}

void world2df_destroy_rigidbody(world2df_p world, uint32_t id)
{
        khint_t k;
        rigidbody_p body;
        k = kh_get(krigidbodies, world->rigidbodies, id);
        if (k != kh_end(world->rigidbodies)) {
                body = kh_val(world->rigidbodies, k);
                SDL_free(body);
                kh_del(krigidbodies, world->rigidbodies, k);
        }
}

rigidbody_p world2df_get_rigidbody(world2df_p world, int conv)
{
        uint32_t entity_id;
        int ret;
        khint_t k;
        rigidbody_p body;
        fp_t speed;

        speed = fp_from_float(0.3f);
        k = kh_get(kconnections, world->connections, conv);
        if (k == kh_end(world->connections)) {
                return NULL;
        }

        entity_id = kh_val(world->connections, k);
        k = kh_get(krigidbodies, world->rigidbodies, entity_id);
        if (k == kh_end(world->rigidbodies)) {
                return NULL;
        }
        body = kh_val(world->rigidbodies, k);
        return body;
}

int world2df_move_rigidbody(world2df_p world, int conv, short keycode)
{
        uint32_t entity_id;
        int ret;
        khint_t k;
        rigidbody_p body;
        fp_t speed;

        speed = fp_from_float(0.3f);
        k = kh_get(kconnections, world->connections, conv);
        if (k == kh_end(world->connections)) {
                return 0;
        }
        
        entity_id = kh_val(world->connections, k);
        k = kh_get(krigidbodies, world->rigidbodies, entity_id);
        if (k == kh_end(world->rigidbodies)) {
                return 0;
        }
        body = kh_val(world->rigidbodies, k);

        if (keycode == KEYCODE_UP) {
                body->linear_velocity.x = fp_zero();
                body->linear_velocity.y = -speed;
                body->position = vec2f_add(body->position, body->linear_velocity);
        }
        else if (keycode == KEYCODE_LEFT) {
                body->linear_velocity.x = -speed;
                body->linear_velocity.y = fp_zero();
                body->position = vec2f_add(body->position, body->linear_velocity);
        }
        else if (keycode == KEYCODE_RIGHT) {
                body->linear_velocity.x = speed;
                body->linear_velocity.y = fp_zero();
                body->position = vec2f_add(body->position, body->linear_velocity);
        }
        else if (keycode == KEYCODE_DOWN) {
                body->linear_velocity.x = fp_zero();
                body->linear_velocity.y = speed;
                body->position = vec2f_add(body->position, body->linear_velocity);
        } 
        return 1;
}

bool world2df_get_entity_id(world2df_p world, int conv, uint32_t* entity_id)
{
        int ret;
        khint_t k;
        k = kh_get(kconnections, world->connections, conv);
        if (k != kh_end(world->connections)) {
                *entity_id = kh_val(world->connections, k);
                return true;
        }
        else {
                *entity_id = 0;
                return false;
        }
}

int world2df_connections_count(world2df_p world)
{
        return kh_size(world->connections);
}

void world2df_add_connection(world2df_p world, int conv, uint32_t entity_id)
{
        int ret;
        khint_t k;
        k = kh_get(kconnections, world->connections, conv);
        if (k == kh_end(world->connections)) {
                k = kh_put(kconnections, world->connections, conv, &ret);
                kh_val(world->connections, k) = entity_id;
        }
}

void world2df_add_emeny(world2df_p world, uint32_t entity_id)
{
        int ret;
        khint_t k;
        enemy_p enemy;
        k = kh_get(kenemies, world->enemies, entity_id);
        if (k == kh_end(world->enemies)) {
                enemy = (enemy_p)SDL_malloc(sizeof(enemy_t));
                enemy->id = entity_id;
                enemy->hp = 10;
                k = kh_put(kenemies, world->enemies, enemy->id, &ret);
                kh_val(world->enemies, k) = enemy;
        }
}

int world2df_remove_connection(world2df_p world, int conv)
{
        uint32_t entity_id;
        int ret;
        khint_t k;
        rigidbody_p body;

        k = kh_get(kconnections, world->connections, conv);
        if (k == kh_end(world->connections)) {
                return 0;
        }
        entity_id = kh_val(world->connections, k);
        kh_del(kconnections, world->connections, k);
        k = kh_get(krigidbodies, world->rigidbodies, entity_id);
        if (k != kh_end(world->rigidbodies)) {
                body = kh_val(world->rigidbodies, k);
                kh_del(krigidbodies, world->rigidbodies, k);
                SDL_free(body);
        }
        return 1;
}

void world2df_update_connection(world2df_p world, int conv, uint32_t entity_id)
{
        int ret;
        khint_t k;
        k = kh_get(kconnections, world->connections, conv);
        if (k == kh_end(world->connections)) {
                k = kh_put(kconnections, world->connections, conv, &ret);
        }
        kh_val(world->connections, k) = entity_id;
}

int 
world2df_collide(world2df_p world, uint32_t a_id, uint32_t b_id, contact2df_t *contact)
{
        int ret, num;
        khint_t k;
        rigidbody_p a_body, b_body;
        rectanglef_t a, b;
        fp_t a_angle, b_angle;

        k = kh_get(krigidbodies, world->rigidbodies, a_id);
        if (k == kh_end(world->rigidbodies)) {
                return 0;
        }
        a_body = kh_val(world->rigidbodies, k);
        k = kh_get(krigidbodies, world->rigidbodies, b_id);
        if (k == kh_end(world->rigidbodies)) {
                return 0;
        }
        b_body = kh_val(world->rigidbodies, k);

        a.x = a_body->position.x;
        a.y = a_body->position.y;
        a.width = a_body->width;
        a.height = a_body->height;
        b.x = b_body->position.x;
        b.y = b_body->position.y;
        b.width = b_body->width;
        b.height = b_body->height;
        a_angle = a_body->rotation;
        b_angle = b_body->rotation;

        num = collision2df_get_rectangles(a, a_angle, b, b_angle, contact);
        return num;
}

void 
world2df_foreach_body(world2df_p world, 
        void(*cb)(rigidbody_p, void*), void* userdata)
{
        int ret;
        khint_t k;
        rigidbody_p body;
        k = kh_begin(world->connections);
        while (k != kh_end(world->rigidbodies)) {
                if (kh_exist(world->rigidbodies, k)) {
                        body = kh_val(world->rigidbodies, k);
                        cb(body, userdata);
                }
                k++;
        }
}

void
world2df_foreach_connection(world2df_p world,
        void(*cb)(int, uint32_t, void*), void* userdata)
{
        int conv;
        uint32_t entity_id;
        khint_t k;
        k = kh_begin(world->connections);
        while (k != kh_end(world->connections)) {
                if (kh_exist(world->connections, k)) {
                        conv = kh_key(world->connections, k);
                        entity_id = kh_val(world->connections, k);
                        cb(conv, entity_id, userdata);
                }
                k++;
        }
}


int
world2df_enemies_count(world2df_p world)
{
        return kh_size(world->enemies);
}

void world2df_update_emeny(world2df_p world, vec2f_t map_size)
{
        int ret;
        khint_t i, k;
        enemy_p enemy;
        rigidbody_p body;
        i = kh_begin(world->enemies);
        while (i != kh_end(world->enemies)) {
                if (kh_exist(world->enemies, i)) {
                        enemy = kh_val(world->enemies, i);
                        k = kh_get(krigidbodies, world->rigidbodies, enemy->id);
                        if (k != kh_end(world->rigidbodies)) {
                                body = kh_val(world->rigidbodies, k);
                                if (body->linear_velocity.x > 0) {
                                        if (body->position.x >= map_size.x) {
                                                kh_del(kenemies, world->enemies, i);
                                                kh_del(krigidbodies, world->rigidbodies, k);
                                                SDL_free(enemy);
                                                SDL_free(body);
                                        }
                                        else {
                                                body->position = vec2f_add(body->position, body->linear_velocity);
                                        }
                                }
                                else {
                                        if (body->position.x <= 0) {
                                                kh_del(kenemies, world->enemies, i);
                                                kh_del(krigidbodies, world->rigidbodies, k);
                                                SDL_free(enemy);
                                                SDL_free(body);
                                        }
                                        else {
                                                body->position = vec2f_add(body->position, body->linear_velocity);
                                        }
                                }
                        }
                }
                i++;
        }
}

