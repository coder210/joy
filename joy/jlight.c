/* jlight.c — 直接翻译 Red Blob Games Haxe 实现
 *   https://www.redblobgames.com/articles/visibility/
 *   Copyright 2012 Red Blob Games (Apache v2)
 *
 * 数据结构使用 klib (kvec.h)
 *   注意: 所有指针关系用索引代替,避免 kv_push realloc 后指针失效
 */
#include "jlight.h"
#include "external/klib/kvec.h"
#include <string.h>
#include <math.h>
#include <SDL3/SDL.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct { float x, y; } v2;
static v2 v2f(float x, float y) { v2 v={x,y}; return v; }

/* 改用索引代替指针, 防止 kv_push realloc 后指针失效 */
typedef struct seg SEG;
typedef struct ep  EP;
struct ep  { v2 pos; int begin; int seg_id; float angle; };
struct seg { int p1_id, p2_id; float d; };

/* 索引访问宏 */
#define EP_POS(vc,id)  ((vc)->eps.a[(id)].pos)
#define EP_ANG(vc,id)  ((vc)->eps.a[(id)].angle)
#define EP_BGN(vc,id)  ((vc)->eps.a[(id)].begin)
#define EP_SEG(vc,id)  ((vc)->eps.a[(id)].seg_id)
#define SEG_P1(vc,s)   ((vc)->eps.a[(s).p1_id])
#define SEG_P2(vc,s)   ((vc)->eps.a[(s).p2_id])
#define SEG_P1P(vc,s)  (SEG_P1(vc,s).pos)
#define SEG_P2P(vc,s)  (SEG_P2(vc,s).pos)

typedef kvec_t(EP)  ep_vec;
typedef kvec_t(SEG) seg_vec;
typedef kvec_t(v2)  v2_vec;
typedef kvec_t(int) int_vec;

/* Haxe: Visibility */
typedef struct {
    v2      center;
    seg_vec segs;
    ep_vec  eps;
    int_vec open;
    float   begin_angle;
    v2_vec  out;
} VC;

/* ==================== 数学 ==================== */
static v2 line_x(v2 p1, v2 p2, v2 p3, v2 p4) {
    float s = ((p4.x-p3.x)*(p1.y-p3.y)-(p4.y-p3.y)*(p1.x-p3.x))
            / ((p4.y-p3.y)*(p2.x-p1.x)-(p4.x-p3.x)*(p2.y-p1.y));
    return v2f(p1.x+s*(p2.x-p1.x), p1.y+s*(p2.y-p1.y));
}

/* leftOf: 需要 vc 来解析线段端点 */
static bool left_of(VC* vc, int a1, int a2, v2 p) {
    v2 pa = EP_POS(vc,a1), pb = EP_POS(vc,a2);
    return (pb.x-pa.x)*(p.y-pa.y) - (pb.y-pa.y)*(p.x-pa.x) < 0;
}

static v2 lerp(v2 a, v2 b, float f) {
    return v2f(a.x*(1-f)+b.x*f, a.y*(1-f)+b.y*f);
}

static int ep_cmp(const void* a, const void* b) {
    const EP* ea=(const EP*)a,*eb=(const EP*)b;
    if (ea->angle > eb->angle) return 1;
    if (ea->angle < eb->angle) return -1;
    if (!ea->begin &&  eb->begin) return 1;
    if ( ea->begin && !eb->begin) return -1;
    return 0;
}

/* ==================== 构造/析构 ==================== */
static VC* vc_new(void) {
    VC* vc=(VC*)SDL_calloc(1,sizeof(VC));
    if(!vc)return NULL;
    kv_init(vc->segs); kv_init(vc->eps);
    kv_init(vc->open); kv_init(vc->out);
    return vc;
}
static void vc_free(VC* vc) {
    if(!vc)return;
    kv_destroy(vc->segs);kv_destroy(vc->eps);
    kv_destroy(vc->open);kv_destroy(vc->out);
    SDL_free(vc);
}
static void vc_clear(VC* vc) {
    vc->segs.n=0; vc->eps.n=0;
    vc->open.n=0; vc->out.n=0;
}

/* ==================== addSegment ==================== */
static void add_seg(VC* vc, float x1, float y1, float x2, float y2) {
    int si=(int)kv_size(vc->segs), ei=(int)kv_size(vc->eps);
    SEG s; EP e1,e2;
    memset(&s,0,sizeof(s));memset(&e1,0,sizeof(e1));memset(&e2,0,sizeof(e2));
    e1.pos=v2f(x1,y1); e1.seg_id=si;
    e2.pos=v2f(x2,y2); e2.seg_id=si;
    s.p1_id=ei; s.p2_id=ei+1;
    kv_push(SEG,vc->segs,s);
    kv_push(EP, vc->eps, e1);
    kv_push(EP, vc->eps, e2);
}

/* ==================== loadEdgeOfMap ====================
 * 用屏幕边界作为遮挡, 而不是光源半径
 * minx/miny/maxx/maxy 是屏幕边界 */
static void vc_bound(VC* vc, float minx, float miny, float maxx, float maxy) {
    add_seg(vc, minx, miny, maxx, miny); /* top */
    add_seg(vc, maxx, miny, maxx, maxy); /* right */
    add_seg(vc, maxx, maxy, minx, maxy); /* bottom */
    add_seg(vc, minx, maxy, minx, miny); /* left */
}

/* ==================== setLightLocation ==================== */
static void vc_setup(VC* vc, float lx, float ly) {
    vc->center=v2f(lx,ly);
    for(size_t i=0;i<kv_size(vc->segs);i++){
        SEG* s=&vc->segs.a[i];
        v2 p1=EP_POS(vc,s->p1_id), p2=EP_POS(vc,s->p2_id);
        float dx=0.5f*(p1.x+p2.x)-lx, dy=0.5f*(p1.y+p2.y)-ly;
        s->d=dx*dx+dy*dy;
        EP_ANG(vc,s->p1_id)=atan2f(p1.y-ly,p1.x-lx);
        EP_ANG(vc,s->p2_id)=atan2f(p2.y-ly,p2.x-lx);
        float da=EP_ANG(vc,s->p2_id)-EP_ANG(vc,s->p1_id);
        if(da<=-(float)M_PI)da+=2.0f*(float)M_PI;
        if(da> (float)M_PI)da-=2.0f*(float)M_PI;
        EP_BGN(vc,s->p1_id)=(da>0);
        EP_BGN(vc,s->p2_id)=!EP_BGN(vc,s->p1_id);
    }
}

/* ==================== _segment_in_front_of ==================== */
static bool seg_front(VC* vc, int aid, int bid, v2 rel) {
    SEG* a=&vc->segs.a[aid]; SEG* b=&vc->segs.a[bid];
    v2 a1=EP_POS(vc,a->p1_id), a2=EP_POS(vc,a->p2_id);
    v2 b1=EP_POS(vc,b->p1_id), b2=EP_POS(vc,b->p2_id);
    float f=0.01f;
    bool A1=left_of(vc,a->p1_id,a->p2_id,lerp(b1,b2,f));
    bool A2=left_of(vc,a->p1_id,a->p2_id,lerp(b2,b1,f));
    bool A3=left_of(vc,a->p1_id,a->p2_id,rel);
    bool B1=left_of(vc,b->p1_id,b->p2_id,lerp(a1,a2,f));
    bool B2=left_of(vc,b->p1_id,b->p2_id,lerp(a2,a1,f));
    bool B3=left_of(vc,b->p1_id,b->p2_id,rel);
    if(B1==B2&&B2!=B3)return true;
    if(A1==A2&&A2==A3)return true;
    if(A1==A2&&A2!=A3)return false;
    if(B1==B2&&B2==B3)return false;
    /* Haxe: 几何模糊时用距离决定前后 */
    return a->d < b->d;
}

/* ==================== open list ==================== */
static int open_front(VC* vc){return kv_size(vc->open)>0?kv_A(vc->open,0):-1;}

static void open_add(VC* vc,int sid){
    size_t at=kv_size(vc->open);
    for(size_t i=0;i<kv_size(vc->open);i++)
        if(seg_front(vc,sid,kv_A(vc->open,i),vc->center))
            {at=i;break;}
    kv_push(int,vc->open,0);
    for(size_t i=kv_size(vc->open)-1;i>at;i--)
        kv_A(vc->open,i)=kv_A(vc->open,i-1);
    kv_A(vc->open,at)=sid;
}

static void open_rm(VC* vc,int sid){
    size_t f=kv_size(vc->open);
    for(size_t i=0;i<kv_size(vc->open);i++)
        if(kv_A(vc->open,i)==sid){f=i;break;}
    if(f<kv_size(vc->open)){
        for(size_t i=f;i<kv_size(vc->open)-1;i++)
            kv_A(vc->open,i)=kv_A(vc->open,i+1);
        vc->open.n--;
    }
}

/* ==================== addTriangle ==================== */
static void add_tri(VC* vc, float a1, float a2, int old_sid) {
    v2 p1=vc->center;
    v2 p2=v2f(vc->center.x+(float)cos(a1),vc->center.y+(float)sin(a1));
    v2 p3,p4;
    if(old_sid>=0){
        SEG* s=&vc->segs.a[old_sid];
        p3=EP_POS(vc,s->p1_id); p4=EP_POS(vc,s->p2_id);
    }else{
        p3=v2f(vc->center.x+(float)cos(a1)*500,vc->center.y+(float)sin(a1)*500);
        p4=v2f(vc->center.x+(float)cos(a2)*500,vc->center.y+(float)sin(a2)*500);
    }
    v2 pb=line_x(p3,p4,p1,p2);
    p2=v2f(vc->center.x+(float)cos(a2),vc->center.y+(float)sin(a2));
    v2 pe=line_x(p3,p4,p1,p2);
    kv_push(v2,vc->out,pb); kv_push(v2,vc->out,pe);
}

/* ==================== sweep ==================== */
static int vc_sweep(VC* vc) {
    qsort(vc->eps.a,kv_size(vc->eps),sizeof(EP),ep_cmp);
    vc->out.n=0; vc->open.n=0; vc->begin_angle=0;
    for(int pass=0;pass<2;pass++){
        size_t n=kv_size(vc->eps);
        for(size_t i=0;i<n;i++){
            EP* ep=&vc->eps.a[i];
            int old_f=open_front(vc);
            if(ep->begin) open_add(vc,ep->seg_id);
            else          open_rm(vc,ep->seg_id);
            int new_f=open_front(vc);
            if(old_f!=new_f){
                if(pass==1) add_tri(vc,vc->begin_angle,ep->angle,old_f);
                vc->begin_angle=ep->angle;
            }
        }
    }
    return (int)kv_size(vc->out);
}

/* ==================== 渐变纹理 ==================== */
#define GSZ 256
static SDL_Texture* make_grad(SDL_Renderer* r){
    SDL_Texture* t=SDL_CreateTexture(r,SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STATIC,GSZ,GSZ);
    if(!t)return NULL;
    Uint32* px=(Uint32*)SDL_malloc(GSZ*GSZ*4);
    if(!px){SDL_DestroyTexture(t);return NULL;}
    float cx=(GSZ-1)*0.5f,cy=cx,mr=cx;
    for(int y=0;y<GSZ;y++)for(int x=0;x<GSZ;x++){
        float d=sqrtf((x-cx)*(x-cx)+(y-cy)*(y-cy));
        float k=d/mr;if(k>1)k=1;
        Uint8 a=(Uint8)((1-k*k)*255);
        px[y*GSZ+x]=((Uint32)a<<24)|0x00FFFFFF;
    }
    SDL_UpdateTexture(t,NULL,px,GSZ*4);
    SDL_free(px);
    return t;
}

/* ==================== 公开 API ==================== */
jlight_sys_p jlight_sys_create(int w,int h){
    jlight_sys_p s=(jlight_sys_p)SDL_calloc(1,sizeof(jlight_sys_t));
    if(!s)return NULL;
    s->width=w;s->height=h;
    s->ambient_r=20;s->ambient_g=20;s->ambient_b=20;
    return s;
}
static void ensure_tex(jlight_sys_p s,SDL_Renderer*r){
    if(!s->gradient){
        s->gradient=make_grad(r);
        if(s->gradient)SDL_SetTextureBlendMode(s->gradient,SDL_BLENDMODE_ADD);
    }
}
void jlight_sys_destroy(jlight_sys_p s){
    if(!s)return;
    if(s->gradient)SDL_DestroyTexture(s->gradient);
    if(s->lightmap)SDL_DestroyTexture(s->lightmap);
    SDL_free(s);
}
void jlight_sys_resize(jlight_sys_p s,int w,int h){
    if(!s)return;
    if(s->lightmap){SDL_DestroyTexture(s->lightmap);s->lightmap=NULL;}
    s->width=w;s->height=h;
}
int jlight_add(jlight_sys_p s,jlight_t l){
    if(!s||s->count>=JLIGHT_MAX)return -1;
    int id=-1;
    for(int i=0;i<JLIGHT_MAX;i++)if(!s->active[i]){id=i;break;}
    if(id<0)return -1;
    s->lights[id]=l;s->active[id]=true;s->count++;
    return id;
}
void jlight_remove(jlight_sys_p s,int id){
    if(!s||id<0||id>=JLIGHT_MAX||!s->active[id])return;
    s->active[id]=false;s->count--;
}
void jlight_set(jlight_sys_p s,int id,jlight_t l){
    if(!s||id<0||id>=JLIGHT_MAX||!s->active[id])return;
    s->lights[id]=l;
}
void jlight_clear(jlight_sys_p s){
    if(!s)return;memset(s->active,0,sizeof(s->active));s->count=0;
}
void jlight_clear_occluders(jlight_sys_p s){if(s)s->occluder_count=0;}
int jlight_add_occluder(jlight_sys_p s,float x1,float y1,float x2,float y2){
    if(!s||s->occluder_count>=JLIGHT_OCCLUDER_MAX)return -1;
    int id=s->occluder_count++;
    s->occluders[id].x1=x1;s->occluders[id].y1=y1;
    s->occluders[id].x2=x2;s->occluders[id].y2=y2;
    return id;
}
void jlight_set_ambient(jlight_sys_p s,Uint8 r,Uint8 g,Uint8 b){
    if(!s)return;s->ambient_r=r;s->ambient_g=g;s->ambient_b=b;
}

/* ==================== 渲染 ==================== */
void jlight_render(jlight_sys_p s,SDL_Renderer*r){
    if(!s||!r||s->count==0)return;
    ensure_tex(s,r);
    if(!s->gradient)return;
    VC* vc=vc_new();
    if(!vc)return;
    for(int li=0;li<JLIGHT_MAX;li++){
        if(!s->active[li])continue;
        jlight_t* l=&s->lights[li];
        if(l->intensity<=0||l->radius<=0)continue;
        float lx=l->x,ly=l->y,rr=l->radius;
        Uint8 cr=(Uint8)(l->r*l->intensity);
        Uint8 cg=(Uint8)(l->g*l->intensity);
        Uint8 cb=(Uint8)(l->b*l->intensity);
        if(s->occluder_count==0){
            float d=rr*2;
            SDL_FRect dst={lx-rr,ly-rr,d,d};
            SDL_SetTextureColorMod(s->gradient,cr,cg,cb);
            SDL_SetTextureAlphaMod(s->gradient,255);
            SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_ADD);
            SDL_RenderTexture(r,s->gradient,NULL,&dst);
            continue;
        }
        vc_clear(vc);
        vc->center=v2f(lx,ly);
        vc_bound(vc, 0,0,(float)s->width,(float)s->height);
        for(int oi=0;oi<s->occluder_count;oi++)
            add_seg(vc,
                s->occluders[oi].x1,s->occluders[oi].y1,
                s->occluders[oi].x2,s->occluders[oi].y2);
        vc_setup(vc,lx,ly);
        int n_out=vc_sweep(vc);
        if(n_out<2)continue;
        /* 调试: 绿色多边形轮廓 */
        SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(r,0,255,0,255);
        for(int i=0;i<n_out-1;i++)
            SDL_RenderLine(r,vc->out.a[i].x,vc->out.a[i].y,
                            vc->out.a[i+1].x,vc->out.a[i+1].y);
        SDL_RenderLine(r,vc->out.a[n_out-1].x,vc->out.a[n_out-1].y,
                        vc->out.a[0].x,vc->out.a[0].y);
        /* 渐变纹理 + 三角扇 */
        int nv=1+n_out;
        SDL_Vertex* vv=(SDL_Vertex*)SDL_malloc(nv*sizeof(SDL_Vertex));
        if(!vv)continue;
        vv[0].position.x=lx;vv[0].position.y=ly;
        vv[0].color.r=vv[0].color.g=vv[0].color.b=vv[0].color.a=1.0f;
        vv[0].tex_coord.x=vv[0].tex_coord.y=0.5f;
        for(int i=0;i<n_out;i++){
            vv[i+1].position.x=vc->out.a[i].x;
            vv[i+1].position.y=vc->out.a[i].y;
            vv[i+1].color.r=vv[i+1].color.g=vv[i+1].color.b=vv[i+1].color.a=1.0f;
            vv[i+1].tex_coord.x=0.5f+(vc->out.a[i].x-lx)/(2.0f*rr);
            vv[i+1].tex_coord.y=0.5f+(vc->out.a[i].y-ly)/(2.0f*rr);
        }
        int nt=n_out;
        int* idx=(int*)SDL_malloc(nt*3*sizeof(int));
        int ni=0;
        if(idx){
            for(int i=0;i+1<n_out;i++){
                idx[ni++]=0;idx[ni++]=i+1;idx[ni++]=i+2;
            }
            idx[ni++]=0;idx[ni++]=n_out;idx[ni++]=1;
        }
        if(ni>0){
            SDL_SetTextureColorMod(s->gradient,cr,cg,cb);
            SDL_SetTextureAlphaMod(s->gradient,255);
            SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_ADD);
            SDL_RenderGeometry(r,s->gradient,vv,nv,idx,ni);
        }
        SDL_free(idx);SDL_free(vv);
    }
    vc_free(vc);
}
