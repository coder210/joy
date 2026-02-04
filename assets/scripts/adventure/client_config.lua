local config = {}

--config.ip = "8.148.188.213"
config.ip = "192.168.1.26"
config.port = 30000
config.orientation = "LandscapeLeft"
config.start = "scripts/adventure/client2.lua"
config.loader = "lualib/loader.lua"
config.luapath = "lualib/?.lua;scripts/adventure/?.lua"
config.luacpath = "luaclib/?.dll;luaclib/?.so;"
config.cpath = ""

return config