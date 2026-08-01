#include "loading_scene.h"
#include <joy/joy.h>

struct loading_scene {
        scene_p scene;
        Context* ctx;
};

static void on_load(scene_p s) {
        loading_scene_p self = (loading_scene_p)scene_get_userdata(s);
        (void)self;
        log_info("[loading scene] start...");
}

static void on_update(scene_p s, float dt) {
        (void)s;
        (void)dt;
}

static void on_render(scene_p s) {
        (void)s;
}

static void on_destroy(scene_p s) {
        loading_scene_p self = (loading_scene_p)scene_get_userdata(s);
        delete self;
}

loading_scene_p loading_scene_create(Context* ctx) {
        loading_scene_p self = new loading_scene_t();
        self->scene = scene_create("Loading");
        self->ctx = ctx;

        scene_set_userdata(self->scene, self);
        scene_set_load_callback(self->scene, on_load);
        scene_set_update_callback(self->scene, on_update);
        scene_set_render_callback(self->scene, on_render);
        scene_set_destroy_callback(self->scene, on_destroy);

        return self;
}

void loading_scene_destroy(loading_scene_p s) {
        if (s) {
                if (s->scene) {
                        scene_destroy(s->scene);
                }
                delete s;
        }
}

scene_p loading_scene_get_scene(loading_scene_p s) {
        return s ? s->scene : NULL;
}
