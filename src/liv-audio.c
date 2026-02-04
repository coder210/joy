#define DR_MP3_IMPLEMENTATION
#define DR_FLAC_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#define STB_VORBIS_HEADER_ONLY
#include "SDL3/SDL.h"
#include "external/dr_mp3.h"
#include "external/dr_flac.h"
#include "external/dr_wav.h"
#include "external/stb_vorbis.c"
#include "liv-audio.h"

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

bool audio_create_ogg(const char* filepath, uint32_t device_id)
{
        SDL_IOStream* file;
        SDL_AudioSpec spec;
        Uint8* data;
        size_t data_len;
        SDL_AudioStream* stream;
        int error;
        stb_vorbis* vorbis;
        stb_vorbis_info info;

        data = SDL_LoadFile(filepath, &data_len);
        vorbis = stb_vorbis_open_memory(data, data_len, &error, NULL);
        info = stb_vorbis_get_info(vorbis);
        spec.freq = info.sample_rate;
        spec.format = SDL_AUDIO_S16;
        spec.channels = info.channels;
        stream = SDL_CreateAudioStream(&spec, NULL);

        data_len = 0;

        short* pcm = (short*)SDL_malloc(sizeof(short) * 409600 * info.channels);
        int samples;
        while ((samples = stb_vorbis_get_samples_short_interleaved(
                vorbis, info.channels, pcm, 4096)) > 0) {
                size_t size = samples * info.channels * sizeof(short);
                data_len += size;
        }

        if (SDL_BindAudioStream(device_id, stream)) {
                return SDL_PutAudioStreamData(stream, pcm, data_len);
        }
        return false;
}
