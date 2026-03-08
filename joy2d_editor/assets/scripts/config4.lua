local config = {}

config.ip = "192.168.2.13"
config.port = 20000
config.start = "scripts/example/ui.lua"
config.loader = "lualib/loader.lua"
config.luapath = "lualib/?.lua;scripts/example/?.lua"
config.luacpath = "luaclib/?.dll;luaclib/?.so;"
config.cpath = ""

return config