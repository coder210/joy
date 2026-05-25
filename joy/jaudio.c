#define DR_MP3_IMPLEMENTATION
#define DR_FLAC_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#define STB_VORBIS_HEADER_ONLY
#include "SDL3/SDL.h"
#include "external/dr_mp3.h"
#include "external/dr_flac.h"
#include "external/dr_wav.h"
#include "jaudio.h"

typedef struct {
        Uint8* data;
        Uint32 data_len;
        Uint32 pos;
        bool loop;
} AudioData;

uint32_t audio_open_device()
{
        SDL_AudioDeviceID device_id;
        device_id = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
        return device_id;
}

void audio_close_device(uint32_t device_id)
{
        SDL_CloseAudioDevice(device_id);
}

static void SDLCALL FeedTheAudioStreamMore(void* userdata, SDL_AudioStream* astream, int additional_amount, int total_amount)
{
        AudioData* audio = (AudioData*)userdata;

        if (audio->loop && additional_amount > 0) {
                // 计算剩余可用的数据
                Uint32 remaining = audio->data_len - audio->pos;
                Uint32 to_copy = (additional_amount < remaining) ? additional_amount : remaining;

                if (to_copy > 0) {
                        SDL_PutAudioStreamData(astream, audio->data + audio->pos, to_copy);
                        audio->pos += to_copy;
                }

                // 如果到达末尾，回到开头实现循环
                if (audio->pos >= audio->data_len) {
                        audio->pos = 0;
                }
        }
}

bool audio_create_wav(const char* filepath, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint8* data;
        Uint32 data_len;
        SDL_AudioStream* stream;
        if (SDL_LoadWAV(filepath, &spec, &data, &data_len)) {
                stream = SDL_CreateAudioStream(&spec, NULL);

                // 创建音频数据上下文
                AudioData* audio_data = (AudioData*)SDL_malloc(sizeof(AudioData));
                audio_data->data = data;
                audio_data->data_len = data_len;
                audio_data->pos = 0;
                audio_data->loop = true;

                
                SDL_SetAudioStreamGetCallback(stream, FeedTheAudioStreamMore, audio_data);
                if (SDL_BindAudioStream(device_id, stream)) {
                        SDL_PutAudioStreamData(stream, data, data_len);
                }
                return true;
        }
        else {
                SDL_free(data);
                return false;
        }
}

/* ==================== 背景音乐 ==================== */
struct audio_bgm {
        SDL_AudioSpec    spec;
        Uint8*           data;
        Uint32           len;
        uint32_t         device_id;
        SDL_AudioStream* stream;
        AudioData*       audio_data;
        float            gain;
};

audio_bgm_p audio_bgm_create(const char* wav_path, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint8* data;
        Uint32 len;
        if (!SDL_LoadWAV(wav_path, &spec, &data, &len))
                return NULL;

        audio_bgm_p bgm = (audio_bgm_p)SDL_malloc(sizeof(audio_bgm_t));
        bgm->spec      = spec;
        bgm->data      = data;
        bgm->len       = len;
        bgm->device_id = device_id;
        bgm->gain      = 1.0f;

        bgm->audio_data = (AudioData*)SDL_malloc(sizeof(AudioData));
        bgm->audio_data->data     = data;
        bgm->audio_data->data_len = len;
        bgm->audio_data->pos      = 0;
        bgm->audio_data->loop     = true;

        bgm->stream = SDL_CreateAudioStream(&spec, NULL);
        if (!bgm->stream) { SDL_free(bgm->audio_data); SDL_free(data); SDL_free(bgm); return NULL; }

        SDL_SetAudioStreamGetCallback(bgm->stream, FeedTheAudioStreamMore, bgm->audio_data);
        SDL_SetAudioStreamGain(bgm->stream, bgm->gain);
        SDL_BindAudioStream(device_id, bgm->stream);
        SDL_PutAudioStreamData(bgm->stream, data, len);
        return bgm;
}

void audio_bgm_stop(audio_bgm_p bgm)
{
        if (!bgm || !bgm->stream) return;
        SDL_UnbindAudioStream(bgm->stream);
        SDL_ClearAudioStream(bgm->stream);
}

void audio_bgm_set_gain(audio_bgm_p bgm, float gain)
{
        if (!bgm) return;
        bgm->gain = gain;
        if (bgm->stream)
                SDL_SetAudioStreamGain(bgm->stream, gain);
}

void audio_bgm_destroy(audio_bgm_p bgm)
{
        if (!bgm) return;
        if (bgm->stream) {
                SDL_UnbindAudioStream(bgm->stream);
                SDL_DestroyAudioStream(bgm->stream);
        }
        if (bgm->audio_data) SDL_free(bgm->audio_data);
        if (bgm->data) SDL_free(bgm->data);
        SDL_free(bgm);
}

/* ==================== 一次性音效 ==================== */
struct audio_shot {
        SDL_AudioSpec spec;
        Uint8*        data;
        Uint32        len;
        uint32_t      device_id;
        float         gain;
};

audio_shot_p audio_shot_create(const char* wav_path, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint8* data;
        Uint32 len;
        if (!SDL_LoadWAV(wav_path, &spec, &data, &len))
                return NULL;

        audio_shot_p s = (audio_shot_p)SDL_malloc(sizeof(audio_shot_t));
        s->spec      = spec;
        s->data      = data;
        s->len       = len;
        s->device_id = device_id;
        s->gain      = 1.0f;
        return s;
}

void audio_shot_play(audio_shot_p shot)
{
        if (!shot || !shot->data) return;
        SDL_AudioStream* stream = SDL_CreateAudioStream(&shot->spec, NULL);
        if (!stream) return;
        SDL_SetAudioStreamGain(stream, shot->gain);
        SDL_BindAudioStream(shot->device_id, stream);
        SDL_PutAudioStreamData(stream, shot->data, shot->len);
        // 数据播放完后流会自动销毁（无人绑定时）
        SDL_UnbindAudioStream(stream);
        SDL_DestroyAudioStream(stream);
}

void audio_shot_set_gain(audio_shot_p shot, float gain)
{
        if (shot) shot->gain = gain;
}

void audio_shot_destroy(audio_shot_p shot)
{
        if (!shot) return;
        if (shot->data) SDL_free(shot->data);
        SDL_free(shot);
}
