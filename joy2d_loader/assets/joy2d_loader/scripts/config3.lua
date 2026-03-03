local config = {}

config.ip = "192.168.1.22"
config.port = 30000
config.start = "Test1.dll"
config.luapath = "lualib/?.lua;scripts/adventure/?.lua"
config.luacpath = "luaclib/?.dll;luaclib/?.so;"
config.cpath = ""

return config