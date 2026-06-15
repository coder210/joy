#ifndef JFONT_H
#define JFONT_H
#include "jconfig.h"

typedef struct jfont font_t, *font_p;

#ifdef __cplusplus
extern "C" {
#endif
	JOY_API font_p font_create(const char* filename, int fontsize);
	JOY_API void font_destroy(font_p font);
	JOY_API float font_get_scale(font_p font);
	JOY_API int font_get_size(font_p font);
	JOY_API int font_get_ascent(font_p font);
#ifdef __cplusplus
}
#endif

#endif // !JFONT_H
