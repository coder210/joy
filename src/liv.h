#ifndef LIVNET_H
#define LIVNET_H
#include <stdbool.h>
#include "liv-mono.h"

typedef struct app app_t, *app_p;

app_p liv_create(const char* file_data);
void liv_destroy(app_p app);
void liv_update(app_p app);
void liv_event(app_p app, void *event);
bool liv_running(app_p app);
void liv_quit(app_p app);
void liv_set(app_p app, const char* key, const char* value);
const char* liv_get(app_p app, const char* key);
MonoDomain* liv_get_app_domain(app_p app);
void liv_info(app_p app, const char* str);

#endif // !LIV_H
