#ifndef JOY_H
#define JOY_H
#include <stdbool.h>
#include "./external/lua/lua.h"
#include "./external/lua/lualib.h"
#include "./external/lua/lauxlib.h"
#include "core/mono.h"
#include "core/sys.h"
#include "core/config.h"
#include "core/log.h"
#include "core/physics.h"

typedef struct app app_t, *app_p;

app_p joy_create(const char* config_file);
void joy_destroy(app_p app);
void joy_update(app_p app);
void joy_event(app_p app, void *event);
bool joy_running(app_p app);
void joy_quit(app_p app);
void joy_setenv(app_p app, const char* key, const char* value);
const char* joy_getenv(app_p app, const char* key);
void* joy_get_appdomain(app_p app);

#endif // !JOY_H
