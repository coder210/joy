import os

files = [
    r"e:\c++\joy\project_proxy2d\scenes\game_scene.h",
    r"e:\c++\joy\project_proxy2d\scenes\game_scene.cpp",
    r"e:\c++\joy\project_proxy2d\scenes\single_game_scene.cpp",
    r"e:\c++\joy\project_proxy2d\scenes\host_game_scene.cpp",
    r"e:\c++\joy\project_proxy2d\scenes\client_game_scene.cpp",
    r"e:\c++\joy\project_proxy2d\scenes\server_game_scene.cpp",
]

# 按顺序替换；先去掉无用的 self 声明/哑行，再把 self-> 改成 this->，
# 再处理基类里的虚函数分派调用（必须在通用 (self) 处理之前），最后清理签名形参。
reps = [
    ("self->", "this->"),
    # 基类分派调用（带 self 参数，需在通用 (self)->(this) 之前处理掉）
    ("init(self)", "init()"),
    ("handle_event(self, ev)", "handle_event(ev)"),
    ("network_update(self)", "network_update()"),
    ("tick(self)", "tick()"),
    ("render(self)", "render()"),
    ("destroy(self)", "destroy()"),
    ("shouldSkipFrame(self, frame_id)", "shouldSkipFrame(frame_id)"),
    ("verifyFrameSync(self, frame_id, inputs, in, server_checksum)",
     "verifyFrameSync(frame_id, inputs, in, server_checksum)"),
    ("currentLogicFrame(self)", "currentLogicFrame()"),
    ("shouldCheckVictory(self)", "shouldCheckVictory()"),
    # 子类里传给 GameSceneCommon 静态助手的 self 参数
    ("(self)", "(this)"),
    ("(self,", "(this,"),
    # 虚函数签名里的 game_scene_p self 形参
    ("game_scene_p self, ", ""),
    ("game_scene_p self)", ")"),
]

for p in files:
    with open(p, "r", encoding="utf-8") as f:
        lines = f.readlines()
    # 去掉无用的 self 局部声明与 (void)self; 哑行
    lines = [ln for ln in lines
             if ln.strip() != "(void)self;"
             and ln.strip() != "game_scene_p self = as_gs();"]
    s = "".join(lines)
    for old, new in reps:
        s = s.replace(old, new)
    with open(p, "w", encoding="utf-8") as f:
        f.write(s)
    print("processed:", os.path.basename(p))
print("done")
