#include "liv-lockstep.h"
#include "jsclib.h"

static void jlockstep_create(js_State *J)
{
        //ecs_world_t* world;
        //world = lockstep_create();
        //js_pushint64(J, (long long)world);
}

static void jlockstep_destroy(js_State* J)
{
        //ecs_world_t* world;
        //world = (ecs_world_t*)js_toint64(J, 1);
        //lockstep_destroy(world);
}

static void jlockstep_update(js_State* J)
{
}

void jsopen_lockstep(js_State *J)
{
        js_newobject(J);
        js_newcfunction(J, jlockstep_create, "create", 1);
        js_setproperty(J, -2, "create");
        js_newcfunction(J, jlockstep_destroy, "destroy", 1);
        js_setproperty(J, -2, "destroy");
        js_newcfunction(J, jlockstep_update, "update", 1);
        js_setproperty(J, -2, "update");
        js_setglobal(J, "lockstep");
}
