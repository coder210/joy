#include "SDL3/SDL.h"
#include "external/dr_mp3.h"
#include "external/dr_flac.h"
#include "external/dr_wav.h"
#include "joy.h"

#define HAVE_OGG 1

/* OGG 解码函数声明。
 * 注意：external/stb_vorbis.h 是"头文件内含实现"的版本（无 STB_VORBIS_IMPLEMENTATION 守卫，
 * 且内部定义了 L / PLAYBACK_* 等宏），若在此 include 会与 jimpl.c 的实现重复定义，
 * 并污染 joy.h 里名为 L 的参数。因此这里只手动声明本文件用到的这个函数，
 * 实现由 jimpl.c 提供。strict C99 的 clang（Android NDK）下不能依赖隐式声明。 */
extern int stb_vorbis_decode_filename(const char *filename, int *channels, int *sample_rate, short **output);
/* 同上：手动声明（不能 include 实现头）。用 decode_memory 而非 decode_filename ——
 * *_filename 版内部走 stdio fopen，安卓上资源在 APK 内读不到，会静默返回 0（没声音）。 */
extern int stb_vorbis_decode_memory(const unsigned char *mem, int len, int *channels, int *sample_rate, short **output);

/* ==================== 内部辅助 ==================== */

typedef struct {
        Uint8* data;
        Uint32 data_len;
        Uint32 pos;
        bool loop;
} AudioData;

/* ==================== 设备管理 ==================== */

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

/* ==================== 流回调（循环播放） ==================== */

static void SDLCALL FeedTheAudioStreamMore(void* userdata, SDL_AudioStream* astream, int additional_amount, int total_amount)
{
        AudioData* audio = (AudioData*)userdata;
        (void)total_amount;
        if (audio->loop && additional_amount > 0) {
                Uint32 remaining = audio->data_len - audio->pos;
                Uint32 to_copy = (additional_amount < remaining) ? (Uint32)additional_amount : remaining;
                if (to_copy > 0) {
                        SDL_PutAudioStreamData(astream, audio->data + audio->pos, to_copy);
                        audio->pos += to_copy;
                }
                if (audio->pos >= audio->data_len)
                        audio->pos = 0;
        }
}

/* ==================== 格式解码器 ==================== */

/* 用 dr_mp3 解码 MP3 → F32 PCM，返回 malloc 的 buffer，caller 负责 SDL_free */
static Uint8* decode_mp3(const char* path, SDL_AudioSpec* spec, Uint32* out_len)
{
        drmp3 mp3;
        if (!drmp3_init_file(&mp3, path, NULL))
                return NULL;

        drmp3_uint64 total_frames = drmp3_get_pcm_frame_count(&mp3);
        Uint32 channels = mp3.channels;
        Uint32 total_samples = (Uint32)(total_frames * channels);
        Uint32 byte_len = total_samples * sizeof(float);

        float* buf = (float*)SDL_malloc(byte_len);
        if (!buf) { drmp3_uninit(&mp3); return NULL; }
        drmp3_read_pcm_frames_f32(&mp3, total_frames, buf);
        drmp3_uninit(&mp3);

        spec->format   = SDL_AUDIO_F32;
        spec->channels = (Uint8)channels;
        spec->freq     = mp3.sampleRate;
        *out_len       = byte_len;
        return (Uint8*)buf;
}

/* 用 dr_flac 解码 FLAC → F32 PCM */
static Uint8* decode_flac(const char* path, SDL_AudioSpec* spec, Uint32* out_len)
{
        unsigned int channels, sampleRate;
        drflac_uint64 totalPCMFrameCount;
        float* samples = drflac_open_file_and_read_pcm_frames_f32(path, &channels, &sampleRate, &totalPCMFrameCount, NULL);
        if (!samples) return NULL;

        Uint32 total_samples = (Uint32)(totalPCMFrameCount * channels);
        Uint32 byte_len = total_samples * sizeof(float);

        spec->format   = SDL_AUDIO_F32;
        spec->channels = (Uint8)channels;
        spec->freq     = (int)sampleRate;
        *out_len       = byte_len;
        return (Uint8*)samples; /* drflac 用 drflac_free() 释放，但 SDL_free 也能用 */
}

/* 用 stb_vorbis 解码 OGG → F32 PCM */
static Uint8* decode_ogg(const char* path, SDL_AudioSpec* spec, Uint32* out_len)
{
        int channels, sampleRate;
        short* s16 = NULL;
        int frames;

        /* 先用 SDL_LoadFile 读进内存再解码：安卓资源打包在 APK 内，
         * stb_vorbis_decode_filename 内部的 fopen 读不到（会静默返回 0 → 没声音）。 */
        size_t file_len = 0;
        Uint8* filedata = (Uint8*)SDL_LoadFile(path, &file_len);
        if (!filedata) return NULL;

        frames = stb_vorbis_decode_memory(filedata, (int)file_len, &channels, &sampleRate, &s16);
        SDL_free(filedata);
        if (frames == 0) return NULL;

        /* frames = 每声道帧数，总样本数 = frames * channels */
        Uint32 total = (Uint32)(frames * channels);
        Uint32 byte_len = total * sizeof(float);

        float* buf = (float*)SDL_malloc(byte_len);
        if (!buf) { free(s16); return NULL; }
        for (Uint32 i = 0; i < total; i++)
                buf[i] = s16[i] / 32768.0f;
        free(s16);

        spec->format   = SDL_AUDIO_F32;
        spec->channels = (Uint8)channels;
        spec->freq     = sampleRate;
        *out_len       = byte_len;
        return (Uint8*)buf;
}

/* ==================== 简易播放（创建循环流，自动播放） ==================== */

/* 内部：从已解码的 PCM 创建循环播放流 */
static bool stream_from_pcm(uint32_t device_id, SDL_AudioSpec* spec, Uint8* data, Uint32 len)
{
        SDL_AudioStream* stream = SDL_CreateAudioStream(spec, NULL);
        if (!stream) { SDL_free(data); return false; }

        AudioData* ad = (AudioData*)SDL_malloc(sizeof(AudioData));
        if (!ad) { SDL_DestroyAudioStream(stream); SDL_free(data); return false; }
        ad->data     = data;
        ad->data_len = len;
        ad->pos      = 0;
        ad->loop     = true;

        SDL_SetAudioStreamGetCallback(stream, FeedTheAudioStreamMore, ad);
        SDL_BindAudioStream(device_id, stream);
        SDL_PutAudioStreamData(stream, data, len);
        return true;
}

bool audio_create_wav(const char* filepath, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint8* data;
        Uint32 data_len;
        if (!SDL_LoadWAV(filepath, &spec, &data, &data_len))
                return false;
        return stream_from_pcm(device_id, &spec, data, data_len);
}

bool audio_create_mp3(const char* filepath, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint32 len;
        Uint8* data = decode_mp3(filepath, &spec, &len);
        if (!data) return false;
        return stream_from_pcm(device_id, &spec, data, len);
}

bool audio_create_flac(const char* filepath, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint32 len;
        Uint8* data = decode_flac(filepath, &spec, &len);
        if (!data) return false;
        return stream_from_pcm(device_id, &spec, data, len);
}

bool audio_create_ogg(const char* filepath, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint32 len;
        Uint8* data = decode_ogg(filepath, &spec, &len);
        if (!data) return false;
        return stream_from_pcm(device_id, &spec, data, len);
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

/* 内部：从已解码 PCM 创建 BGM */
static audio_bgm_p bgm_from_pcm(const char* path, uint32_t device_id)
{
        (void)path;
        /* 由各格式函数负责解码，本函数仅创建 bgm 对象 */
        return NULL;
}

static audio_bgm_p bgm_create_from_pcm(SDL_AudioSpec* spec, Uint8* data, Uint32 len, uint32_t device_id)
{
        audio_bgm_p bgm = (audio_bgm_p)SDL_malloc(sizeof(audio_bgm_t));
        if (!bgm) { SDL_free(data); return NULL; }

        bgm->spec      = *spec;
        bgm->data      = data;
        bgm->len       = len;
        bgm->device_id = device_id;
        bgm->gain      = 1.0f;

        bgm->audio_data = (AudioData*)SDL_malloc(sizeof(AudioData));
        if (!bgm->audio_data) { SDL_free(data); SDL_free(bgm); return NULL; }
        bgm->audio_data->data     = data;
        bgm->audio_data->data_len = len;
        bgm->audio_data->pos      = 0;
        bgm->audio_data->loop     = true;

        bgm->stream = SDL_CreateAudioStream(spec, NULL);
        if (!bgm->stream) { SDL_free(bgm->audio_data); SDL_free(data); SDL_free(bgm); return NULL; }

        SDL_SetAudioStreamGetCallback(bgm->stream, FeedTheAudioStreamMore, bgm->audio_data);
        SDL_SetAudioStreamGain(bgm->stream, bgm->gain);
        SDL_BindAudioStream(device_id, bgm->stream);
        SDL_PutAudioStreamData(bgm->stream, data, len);
        return bgm;
}

audio_bgm_p audio_bgm_create(const char* wav_path, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint8* data;
        Uint32 len;
        if (!SDL_LoadWAV(wav_path, &spec, &data, &len))
                return NULL;
        return bgm_create_from_pcm(&spec, data, len, device_id);
}

audio_bgm_p audio_bgm_create_mp3(const char* path, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint32 len;
        Uint8* data = decode_mp3(path, &spec, &len);
        if (!data) return NULL;
        return bgm_create_from_pcm(&spec, data, len, device_id);
}

audio_bgm_p audio_bgm_create_flac(const char* path, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint32 len;
        Uint8* data = decode_flac(path, &spec, &len);
        if (!data) return NULL;
        return bgm_create_from_pcm(&spec, data, len, device_id);
}

audio_bgm_p audio_bgm_create_ogg(const char* path, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint32 len;
        Uint8* data = decode_ogg(path, &spec, &len);
        if (!data) return NULL;
        return bgm_create_from_pcm(&spec, data, len, device_id);
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
                SDL_ClearAudioStream(bgm->stream);
                SDL_DestroyAudioStream(bgm->stream);
        }
        if (bgm->audio_data) SDL_free(bgm->audio_data);
        if (bgm->data) SDL_free(bgm->data);
        SDL_free(bgm);
}

/* ==================== 一次性音效 ==================== */

struct audio_shot {
        SDL_AudioSpec    spec;
        Uint8*           data;
        Uint32           len;
        uint32_t         device_id;
        float            gain;
        SDL_AudioStream* stream;   /* 当前播放流，NULL 表示未播放 */
};

static audio_shot_p shot_create_from_pcm(SDL_AudioSpec* spec, Uint8* data, Uint32 len, uint32_t device_id)
{
        audio_shot_p s = (audio_shot_p)SDL_malloc(sizeof(audio_shot_t));
        if (!s) { SDL_free(data); return NULL; }
        s->spec      = *spec;
        s->data      = data;
        s->len       = len;
        s->device_id = device_id;
        s->gain      = 1.0f;
        s->stream    = NULL;
        return s;
}

audio_shot_p audio_shot_create(const char* wav_path, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint8* data;
        Uint32 len;
        if (!SDL_LoadWAV(wav_path, &spec, &data, &len))
                return NULL;
        return shot_create_from_pcm(&spec, data, len, device_id);
}

audio_shot_p audio_shot_create_mp3(const char* path, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint32 len;
        Uint8* data = decode_mp3(path, &spec, &len);
        if (!data) return NULL;
        return shot_create_from_pcm(&spec, data, len, device_id);
}

audio_shot_p audio_shot_create_flac(const char* path, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint32 len;
        Uint8* data = decode_flac(path, &spec, &len);
        if (!data) return NULL;
        return shot_create_from_pcm(&spec, data, len, device_id);
}

audio_shot_p audio_shot_create_ogg(const char* path, uint32_t device_id)
{
        SDL_AudioSpec spec;
        Uint32 len;
        Uint8* data = decode_ogg(path, &spec, &len);
        if (!data) return NULL;
        return shot_create_from_pcm(&spec, data, len, device_id);
}

void audio_shot_play(audio_shot_p shot)
{
        if (!shot || !shot->data) return;

        /* 复用一条常驻流：初次创建一次，之后每次播放只清空重灌，
         * 避免命中/击杀时高频新建/销毁 SDL_AudioStream 造成的音频管道抖动与掉帧卡顿。
         * 重触发时 ClearAudioStream 会打断上一次播放（一次只播一份，天然去重）。 */
        if (shot->stream) {
                SDL_ClearAudioStream(shot->stream);
        } else {
                shot->stream = SDL_CreateAudioStream(&shot->spec, NULL);
                if (!shot->stream) return;
                SDL_SetAudioStreamGain(shot->stream, shot->gain);
                SDL_BindAudioStream(shot->device_id, shot->stream);
        }

        SDL_PutAudioStreamData(shot->stream, shot->data, shot->len);
}

void audio_shot_set_gain(audio_shot_p shot, float gain)
{
        if (!shot) return;
        shot->gain = gain;
        // 复用常驻流：若流已建（首次播放后），必须同步改流增益，否则音量滑块改了不生效；
        // 与 audio_bgm_set_gain 行为一致。
        if (shot->stream)
                SDL_SetAudioStreamGain(shot->stream, gain);
}

void audio_shot_destroy(audio_shot_p shot)
{
        if (!shot) return;
        if (shot->stream) {
                SDL_UnbindAudioStream(shot->stream);
                SDL_DestroyAudioStream(shot->stream);
        }
        if (shot->data) SDL_free(shot->data);
        SDL_free(shot);
}
