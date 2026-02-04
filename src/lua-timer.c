#include <stdbool.h>
#include <SDL3/SDL.h>
#include "external/lua/lauxlib.h"
#include "external/lua/lua.h"
#include "luaclib.h"
#include "liv-config.h"

typedef struct timer {
        uint64_t last_time;
        int ref_idx; // lua函数的index
        int interval;// ms
        bool active; // 定时器是否激活
}xtimer_t, *timer_p;

// 创建定时器
static int l_timer_create(lua_State* L)
{
        // 检查参数：间隔时间和Lua回调函数
        int interval = (int)luaL_checkinteger(L, 1);
        luaL_checktype(L, 2, LUA_TFUNCTION);

        // 创建定时器对象
        timer_p timer = (timer_p)SDL_malloc(sizeof(xtimer_t));
        SDL_assert(timer);

        // 初始化定时器
        timer->last_time = SDL_GetTicks();
        timer->interval = interval;
        timer->active = true;

        // 保存Lua回调函数到注册表，避免被GC
        lua_pushvalue(L, 2); // 复制函数到栈顶
        timer->ref_idx = luaL_ref(L, LUA_REGISTRYINDEX); // 存储到注册表并获取引用

        // 将定时器对象返回给Lua
        lua_pushlightuserdata(L, timer);
        return 1;
}

// 销毁定时器
static int l_timer_destroy(lua_State* L)
{
        timer_p timer = (timer_p)lua_touserdata(L, 1);
        if (timer) {
                // 释放Lua函数引用
                if (timer->ref_idx != 0) {
                        luaL_unref(L, LUA_REGISTRYINDEX, timer->ref_idx);
                }
                SDL_free(timer);
        }
        return 0;
}

// 触发定时器检查
static int l_timer_trigger(lua_State* L)
{
        timer_p timer = (timer_p)lua_touserdata(L, 1);
        if (!timer || !timer->active) {
                return 0;
        }

        uint64_t current_time = SDL_GetTicks();
        uint64_t elapsed = current_time - timer->last_time;

        // 检查是否达到触发间隔
        if (elapsed >= timer->interval) {
                // 计算实际经过的时间（处理间隔超出的情况）
                int actual_interval = (int)elapsed;

                // 获取保存的Lua函数
                lua_rawgeti(L, LUA_REGISTRYINDEX, timer->ref_idx);

                if (lua_isfunction(L, -1)) {
                        // 调用Lua函数，传递经过的时间作为参数
                        lua_pushinteger(L, actual_interval);
                        lua_pushinteger(L, timer->interval);

                        if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
                                // 错误处理
                                const char* err = lua_tostring(L, -1);
                                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                                        "Timer callback error: %s", err);
                                lua_pop(L, 1); // 弹出错误信息
                        }
                }
                else {
                        lua_pop(L, 1); // 弹出非函数值
                }

                // 更新最后触发时间（使用累积方式避免时间漂移）
                timer->last_time += timer->interval;

                // 如果落后太多，追赶上来
                while (current_time - timer->last_time >= timer->interval) {
                        timer->last_time += timer->interval;
                }
        }

        return 0;
}

// 设置定时器间隔
static int l_timer_set_interval(lua_State* L)
{
        timer_p timer = (timer_p)lua_touserdata(L, 1);
        int interval = (int)luaL_checkinteger(L, 2);

        if (timer) {
                timer->interval = interval;
        }

        return 0;
}

// 启动/停止定时器
static int l_timer_set_active(lua_State* L)
{
        timer_p timer = (timer_p)lua_touserdata(L, 1);
        bool active = lua_toboolean(L, 2);

        if (timer) {
                timer->active = active;
                if (active) {
                        timer->last_time = SDL_GetTicks(); // 重置计时
                }
        }

        return 0;
}

// 重置定时器
static int l_timer_reset(lua_State* L)
{
        timer_p timer = (timer_p)lua_touserdata(L, 1);

        if (timer) {
                timer->last_time = SDL_GetTicks();
        }

        return 0;
}

// 获取定时器状态
static int l_timer_get_elapsed(lua_State* L)
{
        timer_p timer = (timer_p)lua_touserdata(L, 1);

        if (timer) {
                uint64_t elapsed = SDL_GetTicks() - timer->last_time;
                lua_pushinteger(L, (lua_Integer)elapsed);
        }
        else {
                lua_pushinteger(L, 0);
        }

        return 1;
}

int luaopen_timer(lua_State* L)
{
        luaL_checkversion(L);
        luaL_Reg l[] = {
            {"create", l_timer_create},
            {"destroy", l_timer_destroy},
            {"trigger", l_timer_trigger},
            {"setInterval", l_timer_set_interval},
            {"setActive", l_timer_set_active},
            {"reset", l_timer_reset},
            {"getElapsed", l_timer_get_elapsed},
            {NULL, NULL}
        };
        luaL_newlib(L, l);
        return 1;
}