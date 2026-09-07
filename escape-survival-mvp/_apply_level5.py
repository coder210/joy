#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""将 _level5_src.txt 的第 5 关卡数据插入 level_maps.h，并更新计数/敌人数量。"""
import io

BASE = r"d:\github\joy\project_proxy3d\shared\systems\level_maps.h"
SRC = r"d:\github\joy\escape-survival-mvp\_level5_src.txt"

with io.open(BASE, "r", encoding="utf-8") as f:
    text = f.read()

with io.open(SRC, "r", encoding="utf-8") as f:
    block = f.read()

# 1) LEVEL_COUNT 4 -> 5
old_count = "#define LEVEL_COUNT       4"
new_count = "#define LEVEL_COUNT       5"
assert old_count in text, "LEVEL_COUNT 4 not found"
text = text.replace(old_count, new_count, 1)

# 2) 在 LEVEL_SRCS 数组结尾（紧跟的 '};'）之前插入新关卡字符串
anchor = "\n};\n\n// 每关敌人数量"
assert anchor in text, "LEVEL_SRCS end anchor not found"
# block 去掉尾部换行（_level5_src.txt 以换行结尾，去重）
block_clean = block.rstrip("\n")
text = text.replace(anchor, "\n" + block_clean + anchor, 1)

# 3) LEVEL_ENEMY_COUNT 追加第 5 关值
old_ec = "= { 14, 16, 18, 20 };"
new_ec = "= { 14, 16, 18, 20, 18 };"
assert old_ec in text, "LEVEL_ENEMY_COUNT 4 not found"
text = text.replace(old_ec, new_ec, 1)

with io.open(BASE, "w", encoding="utf-8") as f:
    f.write(text)

print("OK: LEVEL_COUNT=", new_count, "| EC=", new_ec)