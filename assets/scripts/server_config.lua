local config = {}

--config.ip = "172.24.9.215"
config.ip = "192.168.2.11"
config.port = 30001
config.start = "scripts/server2.lua"
config.loader = "lualib/loader.lua"
config.luapath = "lualib/?.lua;scripts/?.lua"
config.luacpath = "luaclib/?.dll;luaclib/?.so;"
config.cpath = ""

return config