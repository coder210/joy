/******************************************************************************
 * src/joy_script.c —— script 域唯一实现文件（一个域一个 .c，配 joy_script.h）
 * 职责：msgpack / 插件模块
 * 合并自：jmsgpack.c / jmodule.c
 * 说明：本域公开 API 声明在配套头 src/joy_script.h，由 include/joy/japi.h 聚合包含；
 *       本文件是该域唯一的编译单元。
 *       内部符号（static / 私有宏 / typedef）保持各分片独立，未跨分片改名。
 ******************************************************************************/

#include "joy_script.h"

/* ==================== 合并自 src/jmsgpack.c ==================== */
#include <msgpack/pack.h>
#include <msgpack/unpack.h>
#include <msgpack/object.h>
#include <msgpack/zone.h>
#include "joy.h"

/* ===== Packer ===== */

void joy_script_msgpack__packer_init(joy_msgpack_packer_t* pk, void* data, joy_msgpack_write_fn cb) {
    msgpack_packer p;
    msgpack_packer_init(&p, data, (msgpack_packer_write)cb);
    pk->data = p.data;
    pk->callback = p.callback;
}

int joy_script_msgpack__pack_int(joy_msgpack_packer_t* pk, int d) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_int(&p, d);
}

int joy_script_msgpack__pack_int64(joy_msgpack_packer_t* pk, int64_t d) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_int64(&p, d);
}

int joy_script_msgpack__pack_uint(joy_msgpack_packer_t* pk, unsigned int d) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_unsigned_int(&p, d);
}

int joy_script_msgpack__pack_uint64(joy_msgpack_packer_t* pk, uint64_t d) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_uint64(&p, d);
}

int joy_script_msgpack__pack_nil(joy_msgpack_packer_t* pk) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_nil(&p);
}

int joy_script_msgpack__pack_array(joy_msgpack_packer_t* pk, size_t n) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_array(&p, n);
}

int joy_script_msgpack__pack_str(joy_msgpack_packer_t* pk, size_t l) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_str(&p, l);
}

int joy_script_msgpack__pack_str_body(joy_msgpack_packer_t* pk, const void* b, size_t l) {
    msgpack_packer p = { pk->data, (msgpack_packer_write)pk->callback };
    return msgpack_pack_str_body(&p, b, l);
}

/* ===== Unpacker ===== */

/* joy_msgpack_object_t 在内部就是 msgpack_object（对上层为不透明句柄）。
 * 所有字段访问都收敛到这里，任何平台/版本的布局差异只影响本文件，
 * 上层代码不再依赖结构体尺寸，杜绝 wasm32 下镜像错位之类的静默 bug。 */
#define JMP_OBJ(o) ((const msgpack_object*)(const void*)(o))

joy_msgpack_object_type_t joy_script_msgpack__object_type(const joy_msgpack_object_t* o) {
    return o ? (joy_msgpack_object_type_t)JMP_OBJ(o)->type : JMSGPACK_OBJECT_NIL;
}
uint32_t joy_script_msgpack__object_array_size(const joy_msgpack_object_t* o) {
    if (!o || JMP_OBJ(o)->type != MSGPACK_OBJECT_ARRAY) return 0;
    return JMP_OBJ(o)->via.array.size;
}
const joy_msgpack_object_t* joy_script_msgpack__object_array_at(const joy_msgpack_object_t* o, uint32_t i) {
    if (!o || JMP_OBJ(o)->type != MSGPACK_OBJECT_ARRAY) return NULL;
    if (i >= JMP_OBJ(o)->via.array.size) return NULL;
    return (const joy_msgpack_object_t*)(const void*)&JMP_OBJ(o)->via.array.ptr[i];
}
uint32_t joy_script_msgpack__object_str_size(const joy_msgpack_object_t* o) {
    if (!o || JMP_OBJ(o)->type != MSGPACK_OBJECT_STR) return 0;
    return JMP_OBJ(o)->via.str.size;
}
const char* joy_script_msgpack__object_str_ptr(const joy_msgpack_object_t* o) {
    if (!o || JMP_OBJ(o)->type != MSGPACK_OBJECT_STR) return NULL;
    return JMP_OBJ(o)->via.str.ptr;
}
uint64_t joy_script_msgpack__object_u64(const joy_msgpack_object_t* o) { return o ? JMP_OBJ(o)->via.u64 : 0; }
int64_t  joy_script_msgpack__object_i64(const joy_msgpack_object_t* o) { return o ? JMP_OBJ(o)->via.i64 : 0; }
bool     joy_script_msgpack__object_boolean(const joy_msgpack_object_t* o) { return o ? JMP_OBJ(o)->via.boolean : false; }
double   joy_script_msgpack__object_f64(const joy_msgpack_object_t* o) { return o ? JMP_OBJ(o)->via.f64 : 0.0; }

void joy_script_msgpack__unpacked_init(joy_msgpack_unpacked_t* result) {
    result->zone = NULL;
    result->data = NULL;
}

void joy_script_msgpack__unpacked_destroy(joy_msgpack_unpacked_t* result) {
    if (result->zone) {
        msgpack_zone_free((msgpack_zone*)result->zone);
        result->zone = NULL;
    }
    result->data = NULL;
}

int joy_script_msgpack__unpack_next(joy_msgpack_unpacked_t* result, const char* data, size_t len, size_t* off) {
    msgpack_unpacked inner;
    msgpack_unpacked_init(&inner);
    int ret = msgpack_unpack_next(&inner, data, len, off);
    if (ret >= 0) {
        /* 把根对象复制进 zone，使其地址在整个解包生命周期内稳定（inner.data 在栈上）。
         * 之后所有访问器都直接以 msgpack_object* 读 zone 内存，不再有镜像结构体。 */
        msgpack_object* root = (msgpack_object*)msgpack_zone_malloc(inner.zone, sizeof(msgpack_object));
        if (!root) {
            msgpack_unpacked_destroy(&inner);
            return MSGPACK_UNPACK_NOMEM_ERROR;
        }
        *root = inner.data;
        result->zone = (joy_msgpack_zone_t*)inner.zone;
        inner.zone = NULL;  /* 转移所有权 */
        result->data = (const joy_msgpack_object_t*)(const void*)root;
        msgpack_unpacked_destroy(&inner);
    }
    return ret;
}

const joy_msgpack_object_t* joy_script_msgpack__unpacked_data(const joy_msgpack_unpacked_t* u) {
    return u ? u->data : NULL;
}

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef JMP_OBJ

/* ==================== 结束 src/jmsgpack.c ==================== */

/* ==================== 合并自 src/jmodule.c ==================== */
/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: module.h
Description: 插件系统
Author: ydlc
Version: 1.0
Date: 2021.3.3
History:

插件系统为可选模块：由 JOY_ENABLE_PLUGINS 控制（见 cmake/JoySources.cmake），
CMake 转发为 JOY_USE_PLUGINS。未启用时本文件为空编译单元，不影响任何项目。
公开声明在配套头 src/joy_script.h 的 JOY_USE_PLUGINS 条件段内，两者必须同步。
*************************************************/

#ifdef JOY_USE_PLUGINS

#include <SDL3/SDL.h>
#include "external/klib/khash.h"
#include "joy.h"


KHASH_INIT(kmod, const char*, joy_module_p, 1, kh_str_hash_func, kh_str_hash_equal)


struct joy_modules {
	char path[1024];
	khash_t(kmod)* mods;
};

static void* _try_open(const char* path, const char* name)
{
	const char* l;
	size_t path_size = SDL_strlen(path);
	size_t name_size = SDL_strlen(name);
	size_t sz = path_size + name_size;
	void* dl = NULL;
	char* tmp = (char*)SDL_malloc(sizeof(char) * sz);
	SDL_assert(tmp != NULL);
	do {
		SDL_memset(tmp, 0, sz);
		while (*path == ';')
			path++;
		if (*path == '\0')
			break;
		l = strchr(path, ';');
		if (l == NULL) l = path + SDL_strlen(path);
		int len = l - path;
		int i;
		for (i = 0; path[i] != '?' && i < len; i++) {
			tmp[i] = path[i];
		}
		SDL_memcpy(tmp + i, name, name_size);
		if (path[i] == '?') {
			SDL_strlcpy(tmp + i + name_size, path + i + 1, len - i - 1);
		}
		else {
			joy_core_log_error("Invalid C service path");
			break;
		}
		dl = SDL_LoadObject(tmp);
		path = l;
	} while (dl == NULL);

	SDL_free(tmp);

	if (dl == NULL) {
		char msg[512] = { 0 };
		joy_core_log_error("try open %s failed : %s", name, msg);
	}

	return dl;
}

static int _open_sym(joy_module_p mod)
{
	size_t name_size, total_size;
	char* tmp;
	name_size = SDL_strlen(mod->name);
	total_size = name_size + 9;
	tmp = (char*)SDL_malloc(sizeof(char) * (total_size));
	if (!tmp) {
		return 0;
	}
	SDL_memcpy(tmp, mod->name, name_size);
	SDL_strlcpy(tmp + name_size, "_create", total_size);
	mod->create = (joy_module_create_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_strlcpy(tmp + name_size, "_init", total_size);
	mod->init = (joy_module_init_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_strlcpy(tmp + name_size, "_event", total_size);
	mod->event_handler = (joy_module_event_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_strlcpy(tmp + name_size, "_update", total_size);
	mod->update = (joy_module_update_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_strlcpy(tmp + name_size, "_release", total_size);
	mod->release = (joy_module_release_t)SDL_LoadFunction(mod->handle, tmp);
	SDL_free(tmp);
	return mod->init != NULL;
}

joy_modules_p joy_script_module_create(const char* path)
{
	joy_modules_p mods;
	mods = SDL_malloc(sizeof(joy_modules_t));
	SDL_strlcpy(mods->path, path, 1024);
	mods->mods = kh_init(kmod);
	return mods;
}

void joy_script_module_destroy(joy_modules_p modules)
{
	const char* name;
	joy_module_p mod;
	khint_t p;
	for (p = kh_begin(modules->mods); p != kh_end(modules->mods); p++) {
		if (kh_exist(modules->mods, p)) {
			name = kh_key(modules->mods, p);
			mod = kh_val(modules->mods, p);
			if (mod->handle) {
				SDL_UnloadObject(mod->handle);
			}
			SDL_free(mod);
		}
	}
	kh_destroy(kmod, modules->mods);
	SDL_free(modules);
}

bool joy_script_module_query(joy_modules_p modules, const char* name, joy_module_p *mod)
{
	khint_t k;
	k = kh_get(kmod, modules->mods, name);
	if (k != kh_end(modules->mods)) {
		*mod = kh_val(modules->mods, k);
		return true;
	}
	else {
		return false;
	}
}

void joy_script_module_register(joy_modules_p modules, const char* name,
	joy_module_create_t create,
	joy_module_init_t init,
	joy_module_event_t event_handler,
	joy_module_update_t update,
	joy_module_release_t release)
{
	joy_module_p mod;
	int ret;
	khint_t k;

	k = kh_get(kmod, modules->mods, name);
	if (k == kh_end(modules->mods)) {
		mod = (joy_module_p)SDL_malloc(sizeof(joy_module_t));
		SDL_strlcpy(mod->name, name, 512);
		mod->handle = NULL;
		mod->create = create;
		mod->init = init;
		mod->event_handler = event_handler;
		mod->update = update;
		mod->release = release;
		k = kh_put(kmod, modules->mods, mod->name, &ret);
		kh_val(modules->mods, k) = mod;
	}
}

void joy_script_module_require(joy_modules_p modules, const char* name)
{
	int ret;
	void *handle;
	joy_module_p mod;
	khint_t k;

	k = kh_get(kmod, modules->mods, name);
	if (k != kh_end(modules->mods)) {
		return;
	}
	handle = _try_open(modules->path, name);
	if (handle) {
		mod = (joy_module_p)SDL_malloc(sizeof(joy_module_t));
		SDL_strlcpy(mod->name, name, 512);
		mod->handle = handle;   /* _open_sym 与 destroy 都要用到 DLL 句柄 */
		if (_open_sym(mod)) {
			k = kh_put(kmod, modules->mods, mod->name, &ret);
			kh_val(modules->mods, k) = mod;
		}
		else {
			SDL_UnloadObject(handle);
			joy_core_log_error("%s register failed.", mod->name);
			SDL_free(mod);
		}
	}
	else {
		joy_core_log_error("%s register failed.", name);
	}
}

bool joy_script_modules_retire(joy_modules_p modules, const char* name, joy_module_p *mod)
{
	khint_t k;
	k = kh_get(kmod, modules->mods, name);
	if (k != kh_end(modules->mods)) {
		*mod = kh_val(modules->mods, k);
		kh_del(kmod, modules->mods, k);
		return true;
	}
	else {
		joy_core_log_error("%s not found.", name);
		return false;
	}
}

void* joy_script_module_instance_create(joy_module_p mod)
{
	void* inst;
	if (mod->create) {
		inst = mod->create();
	}
	else {
		inst = NULL;
	}
	return inst;
}

void joy_script_module_instance_init(joy_module_p mod, void* inst, void* ctx, const char* param)
{
	if (mod->init) {
		mod->init(inst, ctx, param);
	}
}

void joy_script_module_instance_event(joy_module_p mod, void* inst, void *event)
{
	if (mod->event_handler) {
		mod->event_handler(inst, event);
	}
}

void joy_script_module_instance_update(joy_module_p mod, void* inst, float delta_time)
{
	if (mod->update) {
		mod->update(inst, delta_time);
	}
}

void joy_script_module_instance_release(joy_module_p mod, void* inst)
{
	if (mod->release) {
		mod->release(inst);
	}
}

#endif /* JOY_USE_PLUGINS */

/* ==================== 结束 src/jmodule.c ==================== */
