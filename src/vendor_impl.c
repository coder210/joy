#define STB_TRUETYPE_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#define DR_FLAC_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_truetype.h"
#include "external/dr_mp3.h"
#include "external/dr_flac.h"
#include "external/dr_wav.h"
#include "external/stb_image.h"
#include "external/stb_image_write.h"

/* OGG 支持（stb_vorbis.h 已在 external/ 中） */
#define STB_VORBIS_IMPLEMENTATION
#include "external/stb_vorbis.h"
