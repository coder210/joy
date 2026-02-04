local config = {}

--config.ip = "172.24.9.215"
config.ip = "192.168.1.26"
config.port = 30000
config.start = "scripts/adventure/server2.lua"
config.loader = "lualib/loader.lua"
config.luapath = "lualib/?.lua;scripts/adventure/?.lua"
config.luacpath = "luaclib/?.dll;luaclib/?.so;"
config.cpath = ""

return config