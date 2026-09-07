#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""用 _level5_src.txt（紧凑版）替换 level_maps.h 中第 5 关的 128 行字符串块。"""
import io

BASE = r"d:\github\joy\project_proxy3d\shared\systems\level_maps.h"
SRC = r"d:\github\joy\escape-survival-mvp\_level5_src.txt"

with io.open(BASE, "r", encoding="utf-8") as f:
    text = f.read()
with io.open(SRC, "r", encoding="utf-8") as f:
    new_block = f.read().rstrip("\n")

anchor = "\n};\n\n// 每关敌人数量"
idx = text.index(anchor)
head = text[:idx]
post = text[idx + len(anchor):]

lines = head.split("\n")
start = len(lines) - 128          # 第 5 关块 = 最后 128 行
assert start >= 0, "head 行数不足 128"
for l in lines[start:]:
    assert l.startswith('\t\t"'), "非字符串行混入第5关块: " + l[:20]

new_head = "\n".join(lines[:start]) + "\n" + new_block
new_text = new_head + anchor + post

with io.open(BASE, "w", encoding="utf-8") as f:
    f.write(new_text)
print("OK: 已替换第 5 关为紧凑版（原块 128 行）")