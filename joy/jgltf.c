/******************************************************************************
 * jgltf —— joy 的 glTF/glb 加载 + CPU 骨骼蒙皮动画 + 绘制模块（可选, 需定义 JOY_USE_GLTF）
 *
 * 渲染：基于 joy 的 jr3d 动态蒙皮网格接口（r3d_mesh_skinned_create/update/draw）。
 * 加载：先把带 BOM / 头部杂散字节的"畸形 glb"在内存中归一化重建为 clean glb
 *      （算法照抄 build/norm_test/norm_test.c），再用 cgltf 解析 + 加载 buffer。
 *
 * 说明：
 *   - r3d_draw_mesh_skinned 走非索引三角形列表绘制，因此带 INDICES 的 primitive
 *     在加载时按索引展开成非索引进制顶点（skin 属性随顶点复制），交给 GPU 时
 *     不建索引缓冲。
 *   - 未启用 JOY_USE_GLTF 时本文件为空编译单元，不影响任何项目。
 *****************************************************************************/

#ifdef JOY_USE_GLTF

#define CGLTF_IMPLEMENTATION
#include "external/cgltf/cgltf.h"

#include "joy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* 贴图解码用 stb_image（实现在 joy/jimpl.c 的 STB_IMAGE_IMPLEMENTATION） */
#include "external/stb_image.h"

/* ==================== 内部类型 ==================== */

/* 每个 primitive 的 CPU 蒙皮源数据（共享, 只读） */
typedef struct {
        int   vertex_count;   /* 已按索引展开的顶点数（非索引进制列表） */
        int   skinned;
        int   joint_count;    /* 该 primitive 关联 skin 的关节数 */
        int  *joint_node;     /* 每个 joint 对应的 node 下标 */
        mat44_t *inverse_bind;/* 每个 joint 的逆绑定矩阵 */
        float *bind_verts;    /* lit 布局: pos.xyz + normal.xyz + uv.xy, 每顶点 8 float */
        float *jnt;           /* 每顶点 4 个关节索引(float), 为 skin joints 数组下标 */
        float *wts;           /* 每顶点 4 个权重 */
        void  *tex;           /* baseColor 贴图的 SDL 纹理(r3d_create_texture), 无则 NULL */
} jgltf_prim_src_t;

struct jgltf_asset {
        int   refs;
        jr3d_p r3d;
        cgltf_data *data;
        void *clean_glb;        /* 规范化后的完整 glb 内存：cgltf 的 buffers/bin 与动画采样
                                   读数都指向它，必须存活到 cgltf_free 之后（随 asset 一并释放，
                                   否则 use-after-free）*/
        void  *white_tex;           /* 纯白填充纹理, 供光照管线采样 */
        int    node_count;
        int    prim_count;
        jgltf_prim_src_t *prims;
        int    anim_count;
        float *anim_duration;
        vec3_t bounds_center;   /* 所有 primitive 绑定姿态的包围球中心（mesh 局部空间） */
        float  bounds_radius;   /* 包围球半径（mesh 局部空间） */
};

struct jgltf_instance {
        jgltf_asset_p asset;
        int     anim_index;
        float   time;
        float   speed;
        bool    paused;
        int     node_count;
        cgltf_node **nodes;
        bool   *matrix_mode;
        mat44_t *static_mat;         /* has_matrix 节点的静态矩阵 */
        mat44_t *node_world;         /* 各节点世界矩阵（mesh 局部空间） */
        vec3_t *lcl_t;
        vec4_t *lcl_r;
        vec3_t *lcl_s;
        int     prim_count;
        render_mesh_p *gpu;          /* 每 primitive 一个动态蒙皮缓冲 */
        float **skinned;             /* 每 primitive CPU 蒙皮输出（重叠写 GPU） */
        int     moved_vertices;
        render_mesh_p bones_mesh;    /* 待销毁的上一帧骨骼线 mesh */
};

/* ==================== 数学（最小实现, 只用 <math.h>） ==================== */

/* 四元数到列主序旋转矩阵（与 mat44_t/mat44_mul 的列主序约定一致） */
static mat44_t quat_to_mat44(vec4_t q) {
        float l = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
        if (l < 1e-12f) l = 1.0f;
        float x = q.x/l, y = q.y/l, z = q.z/l, w = q.w/l;
        float x2=x*x, y2=y*y, z2=z*z;
        mat44_t m;
        m.m0 = 1-2*(y2+z2); m.m4 = 2*(x*y-w*z);  m.m8 = 2*(x*z+w*y);  m.m12 = 0;
        m.m1 = 2*(x*y+w*z);  m.m5 = 1-2*(x2+z2);  m.m9 = 2*(y*z-w*x);  m.m13 = 0;
        m.m2 = 2*(x*z-w*y);  m.m6 = 2*(y*z+w*x);  m.m10= 1-2*(x2+y2);  m.m14 = 0;
        m.m3 = 0;            m.m7 = 0;            m.m11= 0;            m.m15 = 1;
        return m;
}

static vec4_t quat_slerp(vec4_t a, vec4_t b, float t) {
        float dot = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
        if (dot < 0.0f) { dot = -dot; b.x=-b.x; b.y=-b.y; b.z=-b.z; b.w=-b.w; }
        if (dot > 0.9995f) {
                vec4_t r = { a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t,
                             a.z+(b.z-a.z)*t, a.w+(b.w-a.w)*t };
                float l = sqrtf(r.x*r.x+r.y*r.y+r.z*r.z+r.w*r.w);
                if (l) { r.x/=l; r.y/=l; r.z/=l; r.w/=l; }
                return r;
        }
        float theta = acosf(dot);
        float st = sinf(theta);
        float sa = sinf((1-t)*theta)/st;
        float sb = sinf(t*theta)/st;
        vec4_t r = { a.x*sa + b.x*sb, a.y*sa + b.y*sb, a.z*sa + b.z*sb, a.w*sa + b.w*sb };
        return r;
}

static mat44_t mat_from_float16(const float f[16]) {
        mat44_t m;
        m.m0=f[0]; m.m1=f[1]; m.m2=f[2]; m.m3=f[3];
        m.m4=f[4]; m.m5=f[5]; m.m6=f[6]; m.m7=f[7];
        m.m8=f[8]; m.m9=f[9]; m.m10=f[10]; m.m11=f[11];
        m.m12=f[12]; m.m13=f[13]; m.m14=f[14]; m.m15=f[15];
        return m;
}

/* ==================== glb 归一化重建（照抄 norm_test.c） ==================== */

static unsigned char* read_file(const char* path, long* out_len) {
        FILE* f = fopen(path, "rb");
        if (!f) return NULL;
        fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
        unsigned char* b = (unsigned char*)malloc(n ? (size_t)n : 1);
        if (n) fread(b, 1, (size_t)n, f);
        fclose(f);
        *out_len = n;
        return b;
}

static size_t align4(size_t x) { return (x + 3u) & ~(size_t)3u; }

static long find_bytes(const unsigned char* d, long len, long from,
                       const unsigned char* needle, int nlen) {
        for (long i = from; i + nlen <= len; i++)
                if (memcmp(d + i, needle, (size_t)nlen) == 0) return i;
        return -1;
}

static long find_root_close(const unsigned char* d, long len, long start) {
        long depth = 0;
        for (long i = start; i < len; i++) {
                if (d[i] == '"') {
                        i++;
                        while (i < len) {
                                if (d[i] == '\\' && i + 1 < len) { i += 2; continue; }
                                if (d[i] == '"') break;
                                i++;
                        }
                        continue;
                }
                if (d[i] == '{' || d[i] == '[') depth++;
                else if (d[i] == '}' || d[i] == ']') {
                        if (--depth == 0) return i;
                }
        }
        return -1;
}

/* 把(maybe 含 BOM + 杂散字节的) glb 重建为 clean glb，返回 malloc 缓冲, 出参 <out_len>。 */
static unsigned char* make_clean_glb(const unsigned char* raw, long fsize, long* out_len) {
        long p = 0;
        if (fsize >= 3 && raw[0]==0xEF && raw[1]==0xBB && raw[2]==0xBF) p = 3;
        const unsigned char jn[] = {'J','S','O','N'};
        const unsigned char bn[] = {'B','I','N',0x00};

        long json_type = find_bytes(raw, fsize, p, jn, 4);
        long json_start = json_type + 4;
        long json_end   = find_root_close(raw, fsize, json_start);
        if (json_type < 0 || json_end < 0) return NULL;
        long json_len   = json_end - json_start + 1;

        long bin_type  = find_bytes(raw, fsize, json_end + 1, bn, 4);
        long bin_start = (bin_type >= 0) ? bin_type + 4 : fsize;
        long bin_len   = fsize - bin_start;

        /* The exported GLB has UTF-8 replacement U+FFFD (bytes EF BF BD)
         * sequences inserted throughout the BIN chunk body. Strip them so
         * bufferView offsets (relative to the clean binary data) resolve. */
        size_t bom_count = 0;
        {
                long k = bin_start;
                while (k + 2 < fsize) {
                        if (raw[k]==0xEF && raw[k+1]==0xBF && raw[k+2]==0xBD) { bom_count++; k += 3; }
                        else k++;
                }
        }
        long clean_bin_len = bin_len - (long)(bom_count * 3);
        if (clean_bin_len < 0) clean_bin_len = 0;

        size_t json_pad = align4((size_t)json_len) - (size_t)json_len;
        size_t bin_pad  = align4((size_t)clean_bin_len)  - (size_t)clean_bin_len;
        size_t clean_len = 12 + (8 + align4(json_len)) + (8 + align4(clean_bin_len));
        unsigned char* clean = (unsigned char*)calloc(clean_len, 1);

        unsigned char* o = clean;
        o[0]='g';o[1]='l';o[2]='T';o[3]='F';
        uint32_t v = 2; memcpy(o+4, &v, 4);
        uint32_t tl = (uint32_t)clean_len; memcpy(o+8, &tl, 4);
        o += 12;
        uint32_t jl = (uint32_t)align4(json_len); memcpy(o, &jl, 4);
        memcpy(o+4, "JSON", 4); o += 8;
        memcpy(o, raw + json_start, (size_t)json_len);
        memset(o + json_len, ' ', json_pad);
        o += align4(json_len);
        uint32_t bl = (uint32_t)align4(clean_bin_len); memcpy(o, &bl, 4);
        o[4]='B'; o[5]='I'; o[6]='N'; o[7]=0x00;
        o += 8;
        {
                long k = bin_start;
                long w = 0;
                while (k < fsize) {
                        if (k + 2 < fsize && raw[k]==0xEF && raw[k+1]==0xBB && raw[k+2]==0xBF) { k += 3; }
                        else { o[w++] = raw[k++]; }
                }
        }
        memset(o + clean_bin_len, 0, bin_pad);

        *out_len = (long)clean_len;
        return clean;
}

/* ==================== accessor / 采样小工具 ==================== */

static cgltf_accessor* find_attr(cgltf_primitive* p, const char* name) {
        cgltf_size i;
        for (i = 0; i < p->attributes_count; i++)
                if (strcmp(p->attributes[i].name, name) == 0)
                        return p->attributes[i].data;
        return NULL;
}

static void accessor_comp(cgltf_accessor* a, cgltf_size e, cgltf_size comps, float out[8]) {
        memset(out, 0, comps * sizeof(float));
        if (!a) return;
        cgltf_accessor_read_float(a, e, out, comps);
}

/* 读出 sampler 输出中第 e 个关键帧的 comps 个分量（cubic spline 取中心值分量） */
static void output_val(cgltf_animation_sampler* s, cgltf_size e, cgltf_size comps, float out[8]) {
        memset(out, 0, comps * sizeof(float));
        if (!s->output) return;
        cgltf_size base = e;
        if (s->interpolation == cgltf_interpolation_type_cubic_spline) base = 3*e + 1;
        cgltf_accessor_read_float(s->output, base, out, comps);
}

/* 对动画 tracker 求值（TRS 插值），返回该时刻采样值（前 comps 个分量） */
static void eval_sampler(cgltf_animation_sampler* s, float t, cgltf_size comps, float out[8]) {
        cgltf_accessor* in = s->input;
        cgltf_size n = in ? in->count : 0;
        if (!n) { memset(out, 0, comps*sizeof(float)); return; }

        float in0, in1;
        cgltf_accessor_read_float(in, 0, &in0, 1);
        cgltf_accessor_read_float(in, n-1, &in1, 1);

        float ft = t;
        if (n > 1 && in1 > in0) {           /* 循环到 [in0, in1) */
                float span = in1 - in0;
                ft = fmodf(t - in0, span);
                if (ft < 0.0f) ft += span;
                ft += in0;
        }

        cgltf_size k = 0;
        while (k + 1 < n) {
                float tk1;
                cgltf_accessor_read_float(in, k+1, &tk1, 1);
                if (ft < tk1) break;
                k++;
        }
        if (k >= n - 1) k = n - 2;

        float tk, tk1, v0[8], v1[8];
        cgltf_accessor_read_float(in, k, &tk, 1);
        cgltf_accessor_read_float(in, k+1, &tk1, 1);
        output_val(s, k, comps, v0);
        output_val(s, k+1, comps, v1);

        if (s->interpolation == cgltf_interpolation_type_step) {
                memcpy(out, v0, comps*sizeof(float));
                return;
        }
        float u = (tk1 > tk) ? (ft - tk) / (tk1 - tk) : 0.0f;
        if (comps == 4) {                    /* rotation: slerp */
                vec4_t q0 = { v0[0], v0[1], v0[2], v0[3] };
                vec4_t q1 = { v1[0], v1[1], v1[2], v1[3] };
                vec4_t q  = quat_slerp(q0, q1, u);
                out[0]=q.x; out[1]=q.y; out[2]=q.z; out[3]=q.w;
        } else {                             /* translation / scale: lerp */
                cgltf_size c;
                for (c = 0; c < comps; c++) out[c] = v0[c] + (v1[c]-v0[c])*u;
        }
}

/* ==================== asset 加载 ==================== */

static int build_prim(jgltf_prim_src_t* out, cgltf_primitive* pr, cgltf_skin* skin) {
        cgltf_accessor *aPos = find_attr(pr, "POSITION");
        cgltf_accessor *aNrm = find_attr(pr, "NORMAL");
        cgltf_accessor *aUv  = find_attr(pr, "TEXCOORD_0");
        cgltf_accessor *aJnt = NULL, *aWts = NULL;
        cgltf_size si, v;

        if (!aPos) return 0;
        if (skin) { aJnt = find_attr(pr, "JOINTS_0"); aWts = find_attr(pr, "WEIGHTS_0"); }

        /* 仅当本 prim 的关节/权重访问器都有已加载的数据源时才做蒙皮；
           有 skin 但个别 prim 缺 JOINTS_0/WEIGHTS_0 数据（如无骨骼权重的装饰子网格）
           的 glb 是合法的，应安全回退为非蒙皮网格，而非崩溃 */
        int skin_ok = (skin && aJnt && aWts
                && aJnt->buffer_view && aJnt->buffer_view->buffer && aJnt->buffer_view->buffer->data
                && aWts->buffer_view && aWts->buffer_view->buffer && aWts->buffer_view->buffer->data);
        if (skin && !skin_ok)
                fprintf(stderr, "[jgltf] warn: prim 缺 skin 权重数据，按非蒙皮处理 (pr=%p)\n", (void*)pr);

        cgltf_size src_count = aPos->count;
        cgltf_size vcount;
        int indexed = (pr->indices && pr->indices->count > 0);
        vcount = indexed ? pr->indices->count : src_count;

        memset(out, 0, sizeof(*out));
        out->vertex_count = (int)vcount;
        out->skinned   = skin_ok ? 1 : 0;
        out->joint_count = skin_ok ? (int)skin->joints_count : 0;
        out->bind_verts = (float*)calloc(vcount * 8, sizeof(float));
        out->jnt = (skin_ok ? (float*)calloc(vcount * 4, sizeof(float)) : NULL);
        out->wts = (skin_ok ? (float*)calloc(vcount * 4, sizeof(float)) : NULL);

        if (skin_ok) {
                cgltf_size j;
                out->joint_node  = (int*)calloc(skin->joints_count ? skin->joints_count : 1, sizeof(int));
                out->inverse_bind= (mat44_t*)calloc(skin->joints_count ? skin->joints_count : 1, sizeof(mat44_t));
                if (skin->inverse_bind_matrices) {
                        for (j = 0; j < skin->joints_count; j++) {
                                float f[16];
                                cgltf_accessor_read_float(skin->inverse_bind_matrices, j, f, 16);
                                out->inverse_bind[j] = mat_from_float16(f);
                        }
                } else {
                        for (j = 0; j < skin->joints_count; j++)
                                out->inverse_bind[j] = mat44_identity();
                }
        }

        for (si = 0; si < vcount; si++) {
                cgltf_size src = indexed ? cgltf_accessor_read_index(pr->indices, si) : si;
                float pv[8], nv[8], uv[8], jv[8], wv[8];
                accessor_comp(aPos, src, 3, pv);
                accessor_comp(aNrm, src, 3, nv);
                accessor_comp(aUv,  src, 2, uv);
                float* o = out->bind_verts + si*8;
                o[0]=pv[0]; o[1]=pv[1]; o[2]=pv[2];
                o[3]=nv[0]; o[4]=nv[1]; o[5]=nv[2];
                o[6]=uv[0]; o[7]=uv[1];
                if (skin_ok) {
                        accessor_comp(aJnt, src, 4, jv);
                        accessor_comp(aWts, src, 4, wv);
                        float sum = wv[0]+wv[1]+wv[2]+wv[3];
                        if (sum > 1e-6f) { wv[0]/=sum; wv[1]/=sum; wv[2]/=sum; wv[3]/=sum; }
                        float* jp = out->jnt + si*4;
                        float* wp = out->wts + si*4;
                        jp[0]=jv[0]; jp[1]=jv[1]; jp[2]=jv[2]; jp[3]=jv[3];
                        wp[0]=wv[0]; wp[1]=wv[1]; wp[2]=wv[2]; wp[3]=wv[3];
                }
        }
        return 1;
}

/* 从 primitive 的 baseColor 贴图(image 的 bufferView)解码出 GPU 纹理；无贴图/失败返回 NULL */
void* jgltf_prim_load_texture(jr3d_p r3d, cgltf_primitive* pr) {
        if (!pr || !pr->material) return NULL;
        cgltf_texture_view* tv = &pr->material->pbr_metallic_roughness.base_color_texture;
        if (!tv->texture || !tv->texture->image) return NULL;
        cgltf_image* img = tv->texture->image;
        if (img->buffer_view) {
                cgltf_buffer* buf = img->buffer_view->buffer;
                const unsigned char* p = (const unsigned char*)buf->data + img->buffer_view->offset;
                int w = 0, h = 0, n = 0;
                unsigned char* rgba = stbi_load_from_memory(p, (int)img->buffer_view->size, &w, &h, &n, 4);
                if (!rgba || w <= 0 || h <= 0) { fprintf(stderr, "[jgltf] tex decode fail (%.0fx%.0f)\n", (double)w, (double)h); return NULL; }
                void* tex = r3d_create_texture(r3d, w, h, rgba, 1);
                stbi_image_free(rgba);
                printf("[jgltf] tex %dx%d -> %p\n", w, h, tex);
                return tex;
        }
        return NULL;
}

jgltf_asset_p jgltf_asset_load(jr3d_p r3d, const char* path) {
        long fsize; unsigned char* raw; unsigned char* clean; long clen = 0;
        cgltf_options opt; cgltf_data* data = NULL; cgltf_result res;
        jgltf_asset_p a; int mi, pi; cgltf_size i, pc;

        raw = read_file(path, &fsize);
        if (!raw) { printf("[jgltf] cannot read '%s'\n", path); return NULL; }
        clean = make_clean_glb(raw, fsize, &clen);
        free(raw);
        if (!clean) { printf("[jgltf] cannot normalize glb '%s'\n", path); return NULL; }
        {
                char dpath[256];
                snprintf(dpath, sizeof(dpath), "clean_%s.glb", path);
                FILE* df = fopen(dpath, "wb");
                if (df) { fwrite(clean, 1, (size_t)clen, df); fclose(df); }
        }

        memset(&opt, 0, sizeof(opt));
        res = cgltf_parse(&opt, clean, (cgltf_size)clen, &data);
        if (res != cgltf_result_success) {
                printf("[jgltf] cgltf_parse failed res=%d for '%s'\n", (int)res, path);
                free(clean);
                return NULL;
        }
        if (cgltf_load_buffers(&opt, data, NULL) != cgltf_result_success) {
                printf("[jgltf] cgltf_load_buffers failed for '%s'\n", path);
                cgltf_free(data);
                free(clean);
                return NULL;
        }

        /* mesh -> skin 映射（取第一个带着该 mesh 且有 skin 的节点） */
        cgltf_size mesh_count = data->meshes_count;
        cgltf_skin** mesh_skin = (cgltf_skin**)calloc(mesh_count ? mesh_count : 1, sizeof(cgltf_skin*));
        for (i = 0; i < data->nodes_count; i++) {
                cgltf_node* nd = &data->nodes[i];
                if (nd->mesh && nd->skin && !mesh_skin[(size_t)(nd->mesh - data->meshes)])
                        mesh_skin[(size_t)(nd->mesh - data->meshes)] = nd->skin;
        }

        cgltf_size prim_count = 0;
        for (mi = 0; mi < (int)mesh_count; mi++) prim_count += data->meshes[mi].primitives_count;

        a = (jgltf_asset_p)calloc(1, sizeof(*a));
        a->refs = 1;
        a->r3d = r3d;
        a->data = data;
        a->clean_glb = clean;   /* cgltf 数据(buffer/访问器/动画)指向它，asset 存活期内必须保留 */
        a->node_count = (int)data->nodes_count;
        a->prim_count = (int)prim_count;
        a->anim_count = (int)data->animations_count;
        a->prims = (jgltf_prim_src_t*)calloc(prim_count ? prim_count : 1, sizeof(jgltf_prim_src_t));
        a->anim_duration = (float*)calloc(a->anim_count ? a->anim_count : 1, sizeof(float));

        pc = 0;
        for (mi = 0; mi < (int)mesh_count; mi++) {
                cgltf_skin* skin = mesh_skin[mi];
                for (pi = 0; pi < (int)data->meshes[mi].primitives_count; pi++) {
                        cgltf_primitive* pr = &data->meshes[mi].primitives[pi];
                        build_prim(&a->prims[pc], pr, skin);
                        a->prims[pc].tex = jgltf_prim_load_texture(r3d, pr);
                        if (skin) {
                                cgltf_size j;
                                for (j = 0; j < skin->joints_count; j++)
                                        a->prims[pc].joint_node[j] = (int)(skin->joints[j] - data->nodes);
                        }
                        pc++;
                }
        }

        for (i = 0; i < (cgltf_size)a->anim_count; i++) {
                cgltf_animation* an = &data->animations[i];
                float dur = 0.0f; cgltf_size s;
                for (s = 0; s < an->samplers_count; s++) {
                        cgltf_size n = an->samplers[s].input ? an->samplers[s].input->count : 0;
                        if (n) { float t; cgltf_accessor_read_float(an->samplers[s].input, n-1, &t, 1);
                                 if (t > dur) dur = t; }
                }
                a->anim_duration[i] = dur;
        }

        free(mesh_skin);

        /* 纯白纹理（8x8, 光照管线采样） */
        if (r3d) {
                uint8_t rgba[8*8*4]; memset(rgba, 255, sizeof(rgba));
                a->white_tex = r3d_create_texture(r3d, 8, 8, rgba, 0);
        }

        /* 计算模型空间包围球（对所有 primitive 的绑定姿态）：供调用方按模型实际
         * 尺寸归一化缩放 / 自适配相机，避免大尺寸模型(如 Fox.glb 约 26×79×155)
         * 直接扑出视锥导致不可见。 */
        {
                bool first = true;
                vec3_t mn = {0,0,0}, mx = {0,0,0};
                int mi2;
                for (mi2 = 0; mi2 < a->prim_count; mi2++) {
                        jgltf_prim_src_t* ps = &a->prims[mi2];
                        int kk;
                        for (kk = 0; kk < ps->vertex_count; kk++) {
                                float x=ps->bind_verts[kk*8+0], y=ps->bind_verts[kk*8+1], z=ps->bind_verts[kk*8+2];
                                if (first) { mn.x=mx.x=x; mn.y=mx.y=y; mn.z=mx.z=z; first=false; }
                                else {
                                        if (x<mn.x)mn.x=x; if (y<mn.y)mn.y=y; if (z<mn.z)mn.z=z;
                                        if (x>mx.x)mx.x=x; if (y>mx.y)mx.y=y; if (z>mx.z)mx.z=z;
                                }
                        }
                }
                a->bounds_center.x=(mn.x+mx.x)*0.5f; a->bounds_center.y=(mn.y+mx.y)*0.5f; a->bounds_center.z=(mn.z+mx.z)*0.5f;
                float dx=mx.x-mn.x, dy=mx.y-mn.y, dz=mx.z-mn.z;
                a->bounds_radius = sqrtf(dx*dx+dy*dy+dz*dz) * 0.5f;
                if (a->bounds_radius < 1e-6f) a->bounds_radius = 1.0f;
        }

        printf("[jgltf] loaded '%s': nodes=%d prims=%d anims=%d\n",
               path, a->node_count, a->prim_count, a->anim_count);
        return a;
}

/* ==================== asset 生命周期 ==================== */

void jgltf_asset_retain(jgltf_asset_p a) { if (a) a->refs++; }

void jgltf_asset_release(jgltf_asset_p a) {
        int i;
        if (!a) return;
        if (--a->refs > 0) return;
        if (a->white_tex) {
                /* r3d 无公开纹理销毁, 交由 r3d_destroy 一并回收 */
        }
        for (i = 0; i < a->prim_count; i++) {
                free(a->prims[i].bind_verts);
                free(a->prims[i].jnt);
                free(a->prims[i].wts);
                free(a->prims[i].joint_node);
                free(a->prims[i].inverse_bind);
        }
        free(a->prims);
        free(a->anim_duration);
        cgltf_free(a->data);
        free(a->clean_glb);      /* 须在 cgltf_free 之后再释放 clean 内存（动画数据指向它） */
        free(a);
}

int jgltf_asset_animation_count(jgltf_asset_p a) { return a ? a->anim_count : 0; }
const char* jgltf_asset_animation_name(jgltf_asset_p a, int index) {
        if (!a || index < 0 || index >= a->anim_count) return NULL;
        return a->data->animations[index].name;
}
float jgltf_asset_animation_duration(jgltf_asset_p a, int index) {
        if (!a || index < 0 || index >= a->anim_count) return 0.0f;
        return a->anim_duration[index];
}
int jgltf_asset_node_count(jgltf_asset_p a) { return a ? a->node_count : 0; }
int jgltf_asset_prim_count(jgltf_asset_p a) { return a ? a->prim_count : 0; }
int jgltf_asset_has_skin(jgltf_asset_p a) {
        int i;
        if (!a) return 0;
        for (i = 0; i < a->prim_count; i++) if (a->prims[i].skinned) return 1;
        return 0;
}
JOY_API void jgltf_asset_bounds(jgltf_asset_p a, vec3_t* out_center, float* out_radius) {
        if (a) {
                if (out_center) *out_center = a->bounds_center;
                if (out_radius) *out_radius = a->bounds_radius;
        } else {
                if (out_center) { out_center->x=out_center->y=out_center->z=0.0f; }
                if (out_radius) *out_radius = 1.0f;
        }
}

/* ==================== instance ==================== */

jgltf_instance_p jgltf_instance_create(jgltf_asset_p a) {
        jgltf_instance_p inst;
        cgltf_node* nodes;
        int i;
        if (!a || a->node_count < 0) return NULL;

        inst = (jgltf_instance_p)calloc(1, sizeof(*inst));
        inst->asset = a;
        jgltf_asset_retain(a);
        inst->anim_index = 0;
        inst->time = 0.0f;
        inst->speed = 1.0f;
        inst->paused = false;
        inst->node_count = a->node_count;
        inst->prim_count = a->prim_count;

        nodes = a->data->nodes;
        inst->nodes = (cgltf_node**)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(cgltf_node*));
        inst->matrix_mode = (bool*)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(bool));
        inst->static_mat = (mat44_t*)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(mat44_t));
        inst->node_world = (mat44_t*)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(mat44_t));
        inst->lcl_t = (vec3_t*)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(vec3_t));
        inst->lcl_r = (vec4_t*)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(vec4_t));
        inst->lcl_s = (vec3_t*)calloc((size_t)(a->node_count ? a->node_count : 1), sizeof(vec3_t));
        for (i = 0; i < a->node_count; i++) {
                inst->nodes[i] = &nodes[i];
                if (nodes[i].has_matrix) {
                        inst->matrix_mode[i] = true;
                        inst->static_mat[i] = mat_from_float16(nodes[i].matrix);
                }
        }

        inst->gpu = (render_mesh_p*)calloc((size_t)(a->prim_count ? a->prim_count : 1), sizeof(render_mesh_p));
        inst->skinned = (float**)calloc((size_t)(a->prim_count ? a->prim_count : 1), sizeof(float*));
        for (i = 0; i < a->prim_count; i++) {
                jgltf_prim_src_t* src = &a->prims[i];
                float* buf = (float*)malloc((size_t)src->vertex_count * 8 * sizeof(float));
                memcpy(buf, src->bind_verts, (size_t)src->vertex_count * 8 * sizeof(float));
                inst->skinned[i] = buf;
                inst->gpu[i] = r3d_mesh_skinned_create(a->r3d, buf, src->vertex_count, NULL, 0);
        }
        return inst;
}

void jgltf_instance_destroy(jgltf_instance_p inst) {
        int i;
        if (!inst) return;
        for (i = 0; i < inst->prim_count; i++) {
                if (inst->gpu[i]) r3d_mesh_destroy(inst->asset->r3d, inst->gpu[i]);
                free(inst->skinned[i]);
        }
        if (inst->bones_mesh) r3d_mesh_destroy(inst->asset->r3d, inst->bones_mesh);
        free(inst->gpu);
        free(inst->skinned);
        free(inst->nodes);
        free(inst->matrix_mode);
        free(inst->static_mat);
        free(inst->node_world);
        free(inst->lcl_t);
        free(inst->lcl_r);
        free(inst->lcl_s);
        jgltf_asset_release(inst->asset);
        free(inst);
}

void jgltf_instance_set_animation(jgltf_instance_p inst, int index) {
        if (!inst || index < 0) return;
        if (index >= inst->asset->anim_count) index = 0;
        inst->anim_index = index;
        inst->time = 0.0f;
}
int  jgltf_instance_animation(jgltf_instance_p inst) { return inst ? inst->anim_index : -1; }
void jgltf_instance_set_speed(jgltf_instance_p inst, float speed) {
        if (inst) inst->speed = speed;
}
float jgltf_instance_speed(jgltf_instance_p inst) { return inst ? inst->speed : 1.0f; }
void jgltf_instance_set_paused(jgltf_instance_p inst, bool paused) {
        if (inst) inst->paused = paused;
}

void jgltf_instance_update(jgltf_instance_p inst, float dt) {
        jgltf_asset_p a; int ni, v, k; cgltf_size c;
        cgltf_animation* an; float dur;

        if (!inst || inst->paused) return;
        a = inst->asset;
        dur = (inst->anim_index < a->anim_count) ? a->anim_duration[inst->anim_index] : 0.0f;
        inst->time += dt * inst->speed;
        if (dur > 0.0f) {
                if (inst->time >= dur) inst->time = fmodf(inst->time, dur);
                if (inst->time < 0.0f) inst->time += dur;
        }

        /* 1. 从节点静态 TRS 初始化局部变换 */
        for (ni = 0; ni < inst->node_count; ni++) {
                cgltf_node* nd = inst->nodes[ni];
                inst->lcl_t[ni] = (vec3_t){0,0,0};
                inst->lcl_r[ni] = (vec4_t){0,0,0,1};
                inst->lcl_s[ni] = (vec3_t){1,1,1};
                if (!inst->matrix_mode[ni]) {
                        if (nd->has_translation) inst->lcl_t[ni] = (vec3_t){nd->translation[0],nd->translation[1],nd->translation[2]};
                        if (nd->has_rotation)   inst->lcl_r[ni] = (vec4_t){nd->rotation[0],nd->rotation[1],nd->rotation[2],nd->rotation[3]};
                        if (nd->has_scale)      inst->lcl_s[ni] = (vec3_t){nd->scale[0],nd->scale[1],nd->scale[2]};
                }
        }

        /* 2. 应用当前动画各 channel 的 TRS 采样 */
        an = (inst->anim_index < a->anim_count) ? &a->data->animations[inst->anim_index] : NULL;
        if (an) {
                for (c = 0; c < an->channels_count; c++) {
                        cgltf_animation_channel* ch = &an->channels[c];
                        if (!ch->target_node) continue;
                        ni = (int)(ch->target_node - a->data->nodes);
                        if (ni < 0 || ni >= inst->node_count) continue;
                        if (inst->matrix_mode[ni]) continue;
                        float out[8];
                        eval_sampler(ch->sampler, inst->time, 4, out);
                        switch (ch->target_path) {
                        case cgltf_animation_path_type_translation:
                                inst->lcl_t[ni] = (vec3_t){out[0],out[1],out[2]}; break;
                        case cgltf_animation_path_type_rotation:
                                inst->lcl_r[ni] = (vec4_t){out[0],out[1],out[2],out[3]}; break;
                        case cgltf_animation_path_type_scale:
                                inst->lcl_s[ni] = (vec3_t){out[0],out[1],out[2]}; break;
                        default: break;
                        }
                }
        }

        /* 3. 合成各节点本地/世界矩阵 */
        for (ni = 0; ni < inst->node_count; ni++) {
                mat44_t local;
                if (inst->matrix_mode[ni]) {
                        local = inst->static_mat[ni];
                } else {
                        mat44_t T = mat44_translate(inst->lcl_t[ni].x, inst->lcl_t[ni].y, inst->lcl_t[ni].z);
                        mat44_t R = quat_to_mat44(inst->lcl_r[ni]);
                        mat44_t S = mat44_scale(inst->lcl_s[ni].x, inst->lcl_s[ni].y, inst->lcl_s[ni].z);
                        local = mat44_mul(T, mat44_mul(R, S));
                }
                cgltf_node* nd = inst->nodes[ni];
                if (nd->parent) {
                        int pni = (int)(nd->parent - a->data->nodes);
                        inst->node_world[ni] = mat44_mul(inst->node_world[pni], local);
                } else {
                        inst->node_world[ni] = local;
                }
        }

        /* 4. CPU 蒙皮 + 上传 */
        inst->moved_vertices = 0;
        for (ni = 0; ni < inst->prim_count; ni++) {
                jgltf_prim_src_t* src = &a->prims[ni];
                int vc = src->vertex_count;
                float* out = inst->skinned[ni];
                if (!src->skinned) continue;                /* 静态 primitive 无需更新 */

                /* 预计算每个 joint 的 skinMatrix = inverseBind * world（存临时数组, 不污染共享 source） */
                {
                        float* combo = (float*)malloc((size_t)(src->joint_count ? src->joint_count : 1) * 16 * sizeof(float));
                        for (k = 0; k < src->joint_count; k++) {
                                mat44_t comb = mat44_mul(src->inverse_bind[k],
                                                         inst->node_world[src->joint_node[k]]);
                                memcpy(combo + k*16, (const float*)&comb, 16 * sizeof(float));
                        }
                        for (v = 0; v < vc; v++) {
                                float M[16]; int c4;
                                float bx,by,bz,nx,ny,nz;
                                memset(M, 0, sizeof(M));
                                const float* jp = src->jnt + v*4;
                                const float* wp = src->wts + v*4;
                                for (c4 = 0; c4 < 4; c4++) {
                                        int ji = (int)jp[c4];
                                        if (ji < 0 || ji >= src->joint_count) continue;
                                        float w = wp[c4];
                                        if (w <= 0.0f) continue;
                                        const float* cm = combo + ji*16;
                                        int i2;
                                        for (i2 = 0; i2 < 16; i2++) M[i2] += w * cm[i2];
                                }
                                bx = src->bind_verts[v*8+0]; by = src->bind_verts[v*8+1]; bz = src->bind_verts[v*8+2];
                                nx = src->bind_verts[v*8+3]; ny = src->bind_verts[v*8+4]; nz = src->bind_verts[v*8+5];
                                out[v*8+0] = M[0]*bx + M[4]*by + M[8]*bz  + M[12];
                                out[v*8+1] = M[1]*bx + M[5]*by + M[9]*bz  + M[13];
                                out[v*8+2] = M[2]*bx + M[6]*by + M[10]*bz + M[14];
                                out[v*8+3] = M[0]*nx + M[4]*ny + M[8]*nz;
                                out[v*8+4] = M[1]*nx + M[5]*ny + M[9]*nz;
                                out[v*8+5] = M[2]*nx + M[6]*ny + M[10]*nz;
                                out[v*8+6] = src->bind_verts[v*8+6];
                                out[v*8+7] = src->bind_verts[v*8+7];
                                if (fabsf(out[v*8+0]-bx) > 1e-3f ||
                                    fabsf(out[v*8+1]-by) > 1e-3f ||
                                    fabsf(out[v*8+2]-bz) > 1e-3f) inst->moved_vertices++;
                        }
                        free(combo);
                }
                r3d_mesh_skinned_update(a->r3d, inst->gpu[ni], out, vc);
        }
}

void jgltf_instance_draw(jr3d_p r3d, jgltf_instance_p inst, const mat44_t* model) {
        int i;
        static int dbg_printed = 0;
        if (!inst || !r3d) return;
        if (!dbg_printed && inst->prim_count > 0) {
                float* v = inst->skinned[0];
                float* bv = inst->asset->prims[0].bind_verts;
                float mnx=1e9f,mny=1e9f,mnz=1e9f,mxx=-1e9f,mxy=-1e9f,mxz=-1e9f;
                float bmnx=1e9f,bmny=1e9f,bmnz=1e9f,bmxx=-1e9f,bmxy=-1e9f,bmxz=-1e9f;
                int vc = inst->asset->prims[0].vertex_count;
                int kk;
                for (kk = 0; kk < vc; kk++) {
                        float x=v[kk*8+0], y=v[kk*8+1], z=v[kk*8+2];
                        if (x<mnx)mnx=x; if (y<mny)mny=y; if (z<mnz)mnz=z;
                        if (x>mxx)mxx=x; if (y>mxy)mxy=y; if (z>mxz)mxz=z;
                        float bx=bv[kk*8+0], by=bv[kk*8+1], bz=bv[kk*8+2];
                        if (bx<bmnx)bmnx=bx; if (by<bmny)bmny=by; if (bz<bmnz)bmnz=bz;
                        if (bx>bmxx)bmxx=bx; if (by>bmxy)bmxy=by; if (bz>bmxz)bmxz=bz;
                }
                printf("[dbg-draw] prim0 vc=%d skinned_bbox=(%.3f,%.3f,%.3f)-(%.3f,%.3f,%.3f)\n",
                       vc, mnx,mny,mnz, mxx,mxy,mxz);
                printf("[dbg-draw] prim0 bind_bbox=(%.3f,%.3f,%.3f)-(%.3f,%.3f,%.3f)\n",
                       bmnx,bmny,bmnz, bmxx,bmxy,bmxz);
                if (inst->asset->prims[0].joint_count > 0) {
                        mat44_t* ib = inst->asset->prims[0].inverse_bind;
                        printf("[dbg-draw] invbind0 m0=%.4f m5=%.4f m10=%.4f m12=%.4f m13=%.4f m14=%.4f\n",
                               ib[0].m0, ib[0].m5, ib[0].m10, ib[0].m12, ib[0].m13, ib[0].m14);
                        int jn0 = inst->asset->prims[0].joint_node[0];
                        mat44_t* w = &inst->node_world[jn0];
                        printf("[dbg-draw] world0 m0=%.4f m5=%.4f m10=%.4f m12=%.4f m13=%.4f m14=%.4f\n",
                               w->m0, w->m5, w->m10, w->m12, w->m13, w->m14);
                }
                if (model) printf("[dbg-draw] model m0=%.3f m5=%.3f m10=%.3f m12=%.3f m13=%.3f m14=%.3f\n",
                                  model->m0, model->m5, model->m10, model->m12, model->m13, model->m14);
                dbg_printed = 1;
        }
        for (i = 0; i < inst->prim_count; i++) {
                if (!inst->gpu[i]) continue;
                void* tex = inst->asset->prims[i].tex;
                if (!tex) tex = inst->asset->white_tex;
                r3d_draw_mesh_skinned(r3d, inst->gpu[i], model, tex);
        }
}

/* 沿 skin 关节链画线段：每段 = 子关节世界位置 → 父关节世界位置（已按 model 变换到世界） */
void jgltf_instance_draw_bones(jr3d_p r3d, jgltf_instance_p inst, const mat44_t* model) {
        jgltf_asset_p a; int j, k, seg; const mat44_t* m;
        float* verts; int vcount;
        mat44_t identity;
        render_mesh_p mesh;

        if (!inst || !r3d || inst->prim_count <= 0) return;
        a = inst->asset;
        m = model ? model : &identity;

        /* 取第一个蒙皮 primitive 的关节表 */
        {
                const jgltf_prim_src_t* src = NULL;
                for (k = 0; k < inst->prim_count; k++) {
                        if (a->prims[k].skinned) { src = &a->prims[k]; break; }
                }
                if (!src || src->joint_count <= 0) return;

                seg = src->joint_count;
                verts = (float*)malloc((size_t)seg * 2 * 5 * sizeof(float));
                vcount = 0;
                for (j = 0; j < src->joint_count; j++) {
                        int node_i = src->joint_node[j];
                        int parent_i = -1;
                        cgltf_node* cy = inst->nodes[node_i];
                        float px, py, pz, cx, cy2, cz;
                        /* 子关节世界位置 */
                        const mat44_t* w = &inst->node_world[node_i];
                        px = (*m).m0*(*w).m12 + (*m).m4*(*w).m13 + (*m).m8*(*w).m14 + (*m).m12;
                        py = (*m).m1*(*w).m12 + (*m).m5*(*w).m13 + (*m).m9*(*w).m14 + (*m).m13;
                        pz = (*m).m2*(*w).m12 + (*m).m6*(*w).m13 + (*m).m10*(*w).m14 + (*m).m14;
                        /* 父关节世界位置（无父则起点取关节自身, 画 0 长线） */
                        if (cy->parent) parent_i = (int)(cy->parent - a->data->nodes);
                        if (parent_i >= 0) {
                                const mat44_t* pw = &inst->node_world[parent_i];
                                cx = (*m).m0*(*pw).m12 + (*m).m4*(*pw).m13 + (*m).m8*(*pw).m14 + (*m).m12;
                                cy2= (*m).m1*(*pw).m12 + (*m).m5*(*pw).m13 + (*m).m9*(*pw).m14 + (*m).m13;
                                cz = (*m).m2*(*pw).m12 + (*m).m6*(*pw).m13 + (*m).m10*(*pw).m14 + (*m).m14;
                        } else { cx = px; cy2 = py; cz = pz; }
                        float* o = verts + (size_t)vcount*5;
                        o[0]=cx; o[1]=cy2; o[2]=cz; o[3]=0; o[4]=0;
                        o[5]=px; o[6]=py; o[7]=pz; o[8]=0; o[9]=0;
                        vcount += 2;
                }
        }
        if (vcount <= 0) { free(verts); return; }

        /* 用 base 线框网格（5 float/顶点, pos+uv）画线段；销毁上一帧的旧网格 */
        mesh = r3d_mesh_custom(r3d, verts, vcount);
        free(verts);
        if (!mesh) return;
        identity = mat44_identity();
        r3d_draw_wire_basis(r3d, mesh, &identity, (jcolor_t){255, 80, 80, 255});
        if (inst->bones_mesh) r3d_mesh_destroy(r3d, inst->bones_mesh);
        inst->bones_mesh = mesh;
}

int jgltf_instance_moved_vertices(jgltf_instance_p inst) { return inst ? inst->moved_vertices : 0; }

#endif /* JOY_USE_GLTF */