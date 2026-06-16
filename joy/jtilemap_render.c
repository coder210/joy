/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: jtilemap_render.c
Description: Tilemap 渲染封装（可重入实例，无全局变量）
Author: ydlc
Version: 1.0
Date: 2026.5.22
History:
*************************************************/
#include "jtilemap_render.h"
#include "jcore.h"
#include "jutils.h"
#include "jr2d.h"
#include "external/stb_image.h"
#include "external/klib/khash.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

KHASH_MAP_INIT_STR(tex_cache, image_p)

struct jtilemap_render_t {
	jr2d_p                 r2d;
	char*                  asset_base;
	jtilemap_path_remap_t  remap;
	float                  view_w;
	float                  view_h;
	khash_t(tex_cache)*    tex_cache;
};

static const char* default_remap(const char* path)
{
	if (!path || !*path) return path;
	const char* p = path;
	while (*p == '.' || *p == '/' || *p == '\\') p++;
	static char buf[1024];
	snprintf(buf, sizeof(buf), "%s", p);
	return buf;
}

static image_p load_texture_internal(jtilemap_render_p r, const char* path)
{
	if (!path || !*path) return NULL;
	khiter_t k = kh_get(tex_cache, r->tex_cache, path);
	if (k != kh_end(r->tex_cache))
		return kh_value(r->tex_cache, k);

	char full_path[1024];
	if (r->asset_base)
		snprintf(full_path, sizeof(full_path), "%s%s", r->asset_base, path);
	else
		snprintf(full_path, sizeof(full_path), "%s", path);

	int w = 0, h = 0, ch = 0;
	unsigned char* data = stbi_load(full_path, &w, &h, &ch, 4);
	if (!data) {
		int ret;
		khiter_t ki = kh_put(tex_cache, r->tex_cache, path, &ret);
		kh_value(r->tex_cache, ki) = NULL;
		return NULL;
	}

	image_p img = image_create_from_data(r->r2d, data, w, h);
	stbi_image_free(data);
	if (!img) return NULL;

	char* key = utils_strdup(path, NULL);
	int ret;
	khiter_t ki = kh_put(tex_cache, r->tex_cache, key, &ret);
	kh_value(r->tex_cache, ki) = img;
	return img;
}

static image_p load_tileset_tex(jtilemap_render_p r, const jtilemap_tileset_t* ts)
{
	if (!ts || !ts->image) return NULL;
	const char* mapped = r->remap ? r->remap(ts->image) : ts->image;
	return load_texture_internal(r, mapped);
}

static void render_tile_layer_internal(jtilemap_render_p r, jtilemap_p tm,
                                        const jtilemap_tile_layer_p layer,
                                        float cam_x, float cam_y, uint32_t time_ms)
{
	if (!layer->base.visible) return;
	float tilew = (float)tm->tilewidth, tileh = (float)tm->tileheight;
	cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
	cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;

	int sx = (int)floorf(cam_x / tilew); if (sx < 0) sx = 0;
	int sy = (int)floorf(cam_y / tileh); if (sy < 0) sy = 0;
	int ex = (int)ceilf((cam_x + r->view_w) / tilew) + 1; if (ex > layer->width)  ex = layer->width;
	int ey = (int)ceilf((cam_y + r->view_h) / tileh) + 1; if (ey > layer->height) ey = layer->height;

	uint8_t alpha = (uint8_t)(layer->base.opacity * 255.0f);

	for (int ty = sy; ty < ey; ty++) {
		for (int tx = sx; tx < ex; tx++) {
			int raw_gid = jtilemap_get_tile(layer, tx, ty);
			if (raw_gid == 0) continue;
			int gid   = jtilemap_clean_gid(raw_gid);
			int flags = jtilemap_gid_flags(raw_gid);
			jtilemap_gid_resolve_t res = jtilemap_resolve_gid(tm, gid);
			if (!res.tileset) continue;

			float dst_x = tx * tilew - cam_x, dst_y = ty * tileh - cam_y;
			float dst_w = (float)res.tileset->tilewidth;
			float dst_h = (float)res.tileset->tileheight;

			const char* deco_img = jtilemap_get_tile_image(res.tileset, res.local_id);
			if (deco_img) {
				const char* mapped = r->remap ? r->remap(deco_img) : deco_img;
				image_p timg = load_texture_internal(r, mapped);
				if (!timg) continue;
				image_set_alpha(timg, alpha);
				int dw = (int)dst_w, dh = (int)dst_h;
				jtilemap_get_tile_def_size(res.tileset, res.local_id, &dw, &dh);
				image_draw1(timg, NULL, &(jrect_t){ dst_x, dst_y, (float)dw, (float)dh });
				continue;
			}

			float uv[4];
			if (!jtilemap_get_tile_uv(tm, gid, time_ms, uv)) continue;

			image_p timg = load_tileset_tex(r, res.tileset);
			if (!timg) {
				r2d_set_color(r->r2d, 255, 0, 255, 255);
				shape_draw_rectangle(r->r2d, "fill", (jrect_t){ dst_x, dst_y, dst_w, dst_h });
				continue;
			}

			image_set_alpha(timg, alpha);
			jrect_t src = { uv[0], uv[1], uv[2], uv[3] };

			int flip = JR2D_FLIP_NONE;
			if (flags & JTILEMAP_FLIP_H) flip |= JR2D_FLIP_H;
			if (flags & JTILEMAP_FLIP_V) flip |= JR2D_FLIP_V;
			image_draw_ex(timg, &src, (vec2_t){dst_x, dst_y},
				(flags & JTILEMAP_FLIP_D) ? 90.0f : 0.0f,
				(vec2_t){ flip & JR2D_FLIP_H ? -1 : 1, flip & JR2D_FLIP_V ? -1 : 1 },
				(vec2_t){0, 0});
		}
	}
}

static void render_object_layer_internal(const jtilemap_object_layer_p layer,
                                          float cam_x, float cam_y, jr2d_p r2d)
{
	if (!layer->base.visible) return;
	cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
	cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;
	r2d_set_color(r2d, 255, 85, 0, 255);
	for (size_t i = 0; i < kv_size(layer->objects); i++) {
		jtilemap_object_t* obj = &kv_A(layer->objects, i);
		if (!obj->visible || obj->shape != JTILEMAP_SHAPE_RECT) continue;
		if (obj->width <= 0.0f || obj->height <= 0.0f || obj->width > 10000.0f || obj->height > 10000.0f) continue;
		shape_draw_rectangle(r2d, "line", (jrect_t){ obj->x - cam_x, obj->y - cam_y, obj->width, obj->height });
	}
}

static void render_image_layer_internal(jtilemap_render_p r,
                                         const jtilemap_image_layer_p layer,
                                         float cam_x, float cam_y)
{
	if (!layer->base.visible || !layer->image) return;
	cam_x = cam_x * layer->base.parallaxx - layer->base.offsetx;
	cam_y = cam_y * layer->base.parallaxy - layer->base.offsety;

	const char* mapped = r->remap ? r->remap(layer->image) : layer->image;
	image_p timg = load_texture_internal(r, mapped);
	if (!timg) return;

	float tex_w = 0, tex_h = 0;
	image_get_size(timg, &tex_w, &tex_h);

	float vp_w = r->view_w, vp_h = r->view_h;
	float start_x = (layer->repeatx ? fmodf(-cam_x, tex_w) : -cam_x);
	float start_y = (layer->repeaty ? fmodf(-cam_y, tex_h) : -cam_y);
	if (start_x > 0) start_x -= tex_w;
	if (start_y > 0) start_y -= tex_h;

	for (float py = start_y; py < vp_h; py += tex_h)
		for (float px = start_x; px < vp_w; px += tex_w)
			image_draw1(timg, NULL, &(jrect_t){ px, py, tex_w, tex_h });
}

/* ======================== 公共 API ======================== */

jtilemap_render_p jtilemap_render_create(jr2d_p renderer,
                                          const char* asset_base_path,
                                          jtilemap_path_remap_t remap)
{
	jtilemap_render_p r = (jtilemap_render_p)calloc(1, sizeof(jtilemap_render_t));
	if (!r) return NULL;
	r->r2d  = renderer;
	r->remap = remap ? remap : default_remap;
	r->asset_base = asset_base_path ? utils_strdup(asset_base_path, NULL) : NULL;
	r->view_w = 1024.0f;
	r->view_h = 760.0f;
	r->tex_cache = kh_init(tex_cache);
	return r;
}

void jtilemap_render_all(jtilemap_render_p r, jtilemap_p tm,
                          float camera_x, float camera_y,
                          uint32_t time_ms)
{
	if (!r || !tm || !r->r2d) return;

	{
		int vp_w = 0, vp_h = 0, mode;
		r2d_get_logical_presentation(r->r2d, &vp_w, &vp_h, &mode);
		if (vp_w <= 0 || vp_h <= 0)
			r2d_get_size(r->r2d, &vp_w, &vp_h);
		r->view_w = (float)vp_w;
		r->view_h = (float)vp_h;
	}

	for (size_t i = 0; i < kv_size(tm->layers); i++) {
		jtilemap_layer_p base = (jtilemap_layer_p)kv_A(tm->layers, i);
		switch (base->type) {
		case JTILEMAP_LAYER_TILE:
			render_tile_layer_internal(r, tm, (jtilemap_tile_layer_p)base, camera_x, camera_y, time_ms);
			break;
		case JTILEMAP_LAYER_OBJECT:
			render_object_layer_internal((jtilemap_object_layer_p)base, camera_x, camera_y, r->r2d);
			break;
		case JTILEMAP_LAYER_IMAGE:
			render_image_layer_internal(r, (jtilemap_image_layer_p)base, camera_x, camera_y);
			break;
		}
	}
}

bool jtilemap_render_layer(jtilemap_render_p r, jtilemap_p tm,
                            const char* layer_name,
                            float camera_x, float camera_y,
                            uint32_t time_ms)
{
	if (!r || !tm || !r->r2d || !layer_name) return false;

	for (size_t i = 0; i < kv_size(tm->layers); i++) {
		jtilemap_layer_p base = (jtilemap_layer_p)kv_A(tm->layers, i);
		if (!base->name || strcmp(base->name, layer_name) != 0) continue;
		switch (base->type) {
		case JTILEMAP_LAYER_TILE:
			render_tile_layer_internal(r, tm, (jtilemap_tile_layer_p)base, camera_x, camera_y, time_ms);
			return true;
		case JTILEMAP_LAYER_OBJECT:
			render_object_layer_internal((jtilemap_object_layer_p)base, camera_x, camera_y, r->r2d);
			return true;
		case JTILEMAP_LAYER_IMAGE:
			render_image_layer_internal(r, (jtilemap_image_layer_p)base, camera_x, camera_y);
			return true;
		}
	}
	return false;
}

void jtilemap_render_destroy(jtilemap_render_p r)
{
	if (!r) return;
	if (r->tex_cache) {
		const char* key;
		image_p timg;
		kh_foreach(r->tex_cache, key, timg, {
			if (timg) image_destroy(timg);
			free((void*)key);
		});
		kh_destroy(tex_cache, r->tex_cache);
	}
	if (r->asset_base) free(r->asset_base);
	free(r);
}
