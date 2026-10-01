import os

files = [
    r"e:\c++\joy\project_proxy2d\scenes\game_scene.h",
    r"e:\c++\joy\project_proxy2d\scenes\game_scene.cpp",
    r"e:\c++\joy\project_proxy2d\scenes\single_game_scene.cpp",
    r"e:\c++\joy\project_proxy2d\scenes\host_game_scene.cpp",
    r"e:\c++\joy\project_proxy2d\scenes\client_game_scene.cpp",
    r"e:\c++\joy\project_proxy2d\scenes\server_game_scene.cpp",
]

# 仅替换函数名后接 '(' 的形态，避免误伤注释/其它标识符
reps = [
    ("handle_event(", "handleEvent("),
    ("network_update(", "networkUpdate("),
    ("consume_command_frame(", "consumeCommandFrame("),
]

for p in files:
    with open(p, "r", encoding="utf-8") as f:
        s = f.read()
    for old, new in reps:
        s = s.replace(old, new)
    with open(p, "w", encoding="utf-8") as f:
        f.write(s)
    print("processed:", os.path.basename(p))
print("done")
