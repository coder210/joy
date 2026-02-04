#include "csclib.h"
#include "liv-mono.h"
#include "liv-graphics.h"

//typedef struct mono_texture2d {
//        SDL_Texture* texture;
//        MonoString* id;
//}mono_texture2d_t;
//
//
//static mono_texture2d_t
//load_bmp(SDL_Renderer* renderer, MonoString* filename)
//{
//        mono_texture2d_t mono_texture;
//        texture2d_t texture;
//        char* cstr_filename;
//        cstr_filename = livS_mono_string_to_utf8(filename);
//        texture = texture_load_bmp(renderer, cstr_filename);
//        mono_texture.id = livS_mono_string_new(livS_mono_domain_get(), texture.id);
//        mono_texture.texture = texture.texture;
//        livS_mono_free(cstr_filename);
//        return mono_texture;
//}
//
//static mono_texture2d_t
//load_png(SDL_Renderer* renderer, MonoString* filename)
//{
//        mono_texture2d_t mono_texture;
//        texture2d_t texture;
//        char* cstr_filename;
//        cstr_filename = livS_mono_string_to_utf8(filename);
//        texture = texture_load_png(renderer, cstr_filename);
//        mono_texture.id = livS_mono_string_new(livS_mono_domain_get(), texture.id);
//        mono_texture.texture = texture.texture;
//        livS_mono_free(cstr_filename);
//        return mono_texture;
//}
//
//static void
//draw(sprite_batch_t* sprites, mono_texture2d_t mono_texture,
//        SDL_FRect srcrect, SDL_FRect detrect,
//        SDL_FColor color)
//{
//        char* t;
//        texture2d_t texture;
//        t = livS_mono_string_to_utf8(mono_texture.id);
//        SDL_strlcpy(texture.id, t, LIVNET_MAX_PATH);
//        livS_mono_free(t);
//        texture.texture = mono_texture.texture;
//        sprite_batch_draw(sprites, &texture, srcrect, detrect, color);
//}

int csopen_sprite_batch()
{
        //livS_mono_add_internal_call("Livlib.Native.SpriteBatch::Create", sprite_batch_create);
       /* livS_mono_add_internal_call("Livlib.Native.SpriteBatch::Destroy", sprite_batch_destroy);
        livS_mono_add_internal_call("Livlib.Native.SpriteBatch::Init", sprite_batch_init);
        livS_mono_add_internal_call("Livlib.Native.SpriteBatch::Clear", sprite_batch_clear);
        livS_mono_add_internal_call("Livlib.Native.SpriteBatch::LoadBmp", load_bmp);
        livS_mono_add_internal_call("Livlib.Native.SpriteBatch::LoadPng", load_png);
        livS_mono_add_internal_call("Livlib.Native.SpriteBatch::Draw", draw);
        livS_mono_add_internal_call("Livlib.Native.SpriteBatch::Render", sprite_batch_render);*/
        return 1;
}
