local args = { ... }
local luapath = args[1];
local luacpath = args[2];

package.path = package.path .. ";" .. luapath;
package.cpath = package.cpath .. ";" .. luacpath;
packagex.path = packagex.path .. ";" .. luapath;
packagex.cpath = packagex.cpath .. ";" .. luacpath;


local lua_name = args[3];

local ok, err = xpcall(dofilex, function(errmsg) 
    sdl.error("load " .. lua_name .. ":" .. errmsg);
    sdl.error(debug.traceback());
end, lua_name);
if not ok then
    sdl.error("xpcall function failed: " .. tostring(err))
end