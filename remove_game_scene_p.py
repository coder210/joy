import os

h = r"e:\c++\joy\project_proxy2d\scenes\game_scene_common.h"
cpp = r"e:\c++\joy\project_proxy2d\scenes\game_scene_common.cpp"

# game_scene_common.h：删掉 typedef 行，其余 game_scene_p -> GameScene*
with open(h, "r", encoding="utf-8") as f:
    lines = f.readlines()
lines = [ln for ln in lines if "typedef GameScene* game_scene_p" not in ln]
s = "".join(lines).replace("game_scene_p", "GameScene*")
with open(h, "w", encoding="utf-8") as f:
    f.write(s)
print("processed:", os.path.basename(h))

# game_scene_common.cpp：game_scene_p -> GameScene*
with open(cpp, "r", encoding="utf-8") as f:
    s = f.read()
s = s.replace("game_scene_p", "GameScene*")
with open(cpp, "w", encoding="utf-8") as f:
    f.write(s)
print("processed:", os.path.basename(cpp))
print("done")
