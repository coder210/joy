#include "../external/lua/lauxlib.h"
#include "../external/lua/lua.h"
#include "../joy.h"
#include "luaclib.h"


static int l_console_log(lua_State* L) {
	int n = lua_gettop(L);
	for (int i = 1; i <= n; i++) {
		lua_getglobal(L, "tostring");
		lua_pushvalue(L, i);   /* arg */
		lua_call(L, 1, 1);     /* tostring(arg) */
	}
	lua_concat(L, n);          /* 拼接所有参数 */
	log_info("%s", lua_tostring(L, -1));
	return 0;
}

int luaopen_console(lua_State* L) {
	luaL_checkversion(L);
	luaL_Reg l[] = {
		{"log", l_console_log},
		{NULL, NULL}
	};
	luaL_newlib(L, l);
	return 1;
}
