#include "jsclib.h"
#include "joy2d/log.h"

static void jconsole_log(js_State* J)
{
        const char* str;
        str = js_tostring(J, 1);
        log_info("%s", str);
        js_pushundefined(J);
}

void jsopen_base(js_State* J)
{
        js_newobject(J);
        js_newcfunction(J, jconsole_log, "log", 1);
        js_setproperty(J, -2, "log");
        js_setglobal(J, "console");
}
