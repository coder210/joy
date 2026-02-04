#include "external/lua/lapi.h"
#include "external/lua/lauxlib.h"
#include "liv-audio.h"
#include "luaclib.h"

static int l_audio_open_device(lua_State* L)
{
        uint32_t device_id;
        device_id = audio_open_device();
        lua_pushinteger(L, device_id);
        return 1;
}

static int l_audio_close_device(lua_State* L)
{
        uint32_t device_id;
        device_id = luaL_checkinteger(L, 1);
        audio_close_device(device_id);
        return 0;
}

static int l_audio_create_wav(lua_State* L)
{
        bool result;
        const char* filepath;
        uint32_t device_id;
        filepath = luaL_checkstring(L, 1);
        device_id = luaL_checkinteger(L, 2);
        result = audio_create_wav(filepath, device_id);
        lua_pushboolean(L, result);
        return 1;
}

static int l_audio_create_ogg(lua_State* L)
{
        bool result;
        const char* filepath;
        uint32_t device_id;
        filepath = luaL_checkstring(L, 1);
        device_id = luaL_checkinteger(L, 2);
        result = audio_create_ogg(filepath, device_id);
        lua_pushboolean(L, result);
        return 1;
}

//static int lcreate_audio_stream_from_ogg(lua_State* L)
//{
//        const char* path;
//        SDL_IOStream* file;
//        SDL_AudioSpec spec;
//        Uint8* data;
//        Uint32 data_len;
//        SDL_AudioStream* stream;
//        int error;
//        stb_vorbis* vorbis;
//        stb_vorbis_info info;
//
//        path = luaL_checkstring(L, 1);
//
//        file = SDL_IOFromFile(path, "rb");
//        SDL_SeekIO(file, 0, SDL_IO_SEEK_END);
//        data_len = SDL_TellIO(file);
//        SDL_SeekIO(file, 0, SDL_IO_SEEK_SET);
//        data = (Uint8*)SDL_malloc(sizeof(Uint8) * data_len);
//        SDL_ReadIO(file, data, data_len);
//        SDL_CloseIO(file);
//
//        vorbis = stb_vorbis_open_memory(data, data_len, &error, NULL);
//        info = stb_vorbis_get_info(vorbis);
//
//        spec.freq = info.sample_rate;
//        spec.format = SDL_AUDIO_S16;
//        spec.channels = info.channels;
//
//        stream = SDL_CreateAudioStream(&spec, NULL);
//
//
//        //SDL_free(data);
//        data_len = 0;
//
//        short* pcm = (short*)SDL_malloc(sizeof(short) * 409600 * info.channels);
//        int samples;
//        while ((samples = stb_vorbis_get_samples_short_interleaved(
//                vorbis, info.channels, pcm, 4096)) > 0) {
//                size_t size = samples * info.channels * sizeof(short);
//                data_len += size;
//        }
//
//        lua_newtable(L);
//        lua_pushlightuserdata(L, stream);
//        lua_setfield(L, -2, "stream");
//        lua_pushlightuserdata(L, pcm);
//        lua_setfield(L, -2, "data");
//        lua_pushinteger(L, data_len);
//        lua_setfield(L, -2, "data_len");
//
//        return 1;
//}

//static int lopen_audio_device(lua_State* L)
//{
//        SDL_AudioDeviceID audio_device;
//        audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
//        lua_pushinteger(L, audio_device);
//        return 1;
//}
//
//static int lpause_audio_device(lua_State* L)
//{
//        SDL_AudioDeviceID audio_device;
//        bool retval;
//        audio_device = (SDL_AudioDeviceID)luaL_checkinteger(L, 1);
//        retval = SDL_PauseAudioDevice(audio_device);
//        lua_pushboolean(L, retval);
//        return 1;
//}

//static int lresume_audio_device(lua_State* L)
//{
//        SDL_AudioDeviceID audio_device;
//        bool retval;
//        audio_device = (SDL_AudioDeviceID)luaL_checkinteger(L, 1);
//        retval = SDL_ResumeAudioDevice(audio_device);
//        lua_pushboolean(L, retval);
//        return 1;
//}
//
//static int lbind_audio_stream(lua_State* L)
//{
//        bool retval;
//        SDL_AudioDeviceID audio_device;
//        SDL_AudioStream* stream;
//        audio_device = (SDL_AudioDeviceID)luaL_checkinteger(L, 1);
//        stream = (SDL_AudioStream*)lua_touserdata(L, 2);
//        retval = SDL_BindAudioStream(audio_device, stream);
//        if (!retval)
//                SDL_Log("Failed to bind stream to device: %s", SDL_GetError());
//        lua_pushboolean(L, retval);
//        return 1;
//}
//
//static int lget_audio_device_gain(lua_State* L)
//{
//        SDL_AudioDeviceID audio_device;
//        float gain;
//        audio_device = (SDL_AudioDeviceID)luaL_checkinteger(L, 1);
//        gain = SDL_GetAudioDeviceGain(audio_device);
//        lua_pushnumber(L, gain);
//        return 1;
//}
//
//static int lset_audio_device_gain(lua_State* L)
//{
//        bool retval;
//        SDL_AudioDeviceID audio_device;
//        float gain;
//        audio_device = (SDL_AudioDeviceID)luaL_checkinteger(L, 1);
//        gain = (float)luaL_checknumber(L, 2);
//        retval = SDL_SetAudioDeviceGain(audio_device, gain);
//        lua_pushboolean(L, retval);
//        return 1;
//}
//
//static int lclose_audio_device(lua_State* L)
//{
//        SDL_AudioDeviceID audio_device;
//        audio_device = (SDL_AudioDeviceID)luaL_checkinteger(L, 1);
//        SDL_CloseAudioDevice(audio_device);
//        return 0;
//}
//
//static int ldestroy_audio_stream(lua_State* L)
//{
//        SDL_AudioStream* stream;
//        Uint8* data;
//        Uint32 data_len;
//
//        lua_getfield(L, 1, "stream");
//        stream = (SDL_AudioStream*)lua_touserdata(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "data");
//        data = (Uint8*)lua_touserdata(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "data_len");
//        data_len = (Uint32)luaL_checkinteger(L, -1);
//        lua_pop(L, 1);
//
//        SDL_DestroyAudioStream(stream);
//        SDL_free(data);
//        return 0;
//}
//
//static int lput_audio_stream_data(lua_State* L)
//{
//        SDL_AudioStream* stream;
//        Uint8* data;
//        Uint32 data_len;
//
//        lua_getfield(L, 1, "stream");
//        stream = (SDL_AudioStream*)lua_touserdata(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "data");
//        data = (Uint8*)lua_touserdata(L, -1);
//        lua_pop(L, 1);
//        lua_getfield(L, 1, "data_len");
//        data_len = (Uint32)luaL_checkinteger(L, -1);
//        lua_pop(L, 1);
//
//        if (SDL_GetAudioStreamQueued(stream) < data_len) {
//                SDL_PutAudioStreamData(stream, data, data_len);
//        }
//        return 0;
//}
//
//static int lplay_mp3(lua_State* L)
//{
//        drmp3 mp3;
//        drmp3_config config;
//        //drmp3_init_memory()
//
//        return 1;
//}


int luaopen_audio(lua_State *L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
            {"open_device", l_audio_open_device},
            {"close_device", l_audio_close_device},
            {"create_wav", l_audio_create_wav},
            {"create_ogg", l_audio_create_ogg},
           /* {"create_audio_stream_from_wav", lcreate_audio_stream_from_wav},
            {"create_audio_stream_from_ogg", lcreate_audio_stream_from_ogg},
            {"open_audio_device", lopen_audio_device},
            {"pause_audio_device", lpause_audio_device},
            {"resume_audio_device", lresume_audio_device},
            {"set_audio_device_gain", lset_audio_device_gain},
            {"get_audio_device_gain", lget_audio_device_gain},
            {"close_audio_device", lclose_audio_device},
            {"bind_audio_stream", lbind_audio_stream},
            {"destroy_audio_stream", ldestroy_audio_stream},
            {"put_audio_stream_data", lput_audio_stream_data},*/
            {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}
