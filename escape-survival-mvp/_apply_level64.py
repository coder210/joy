#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""用 _levels64_body.txt 替换 level_maps.h 的 LEVEL_SRCS 数组体。"""
import io

BASE = r"d:\github\joy\project_proxy3d\shared\systems\level_maps.h"
BODY = r"d:\github\joy\escape-survival-mvp\_levels64_body.txt"

with io.open(BASE, encoding="utf-8") as f:
    text = f.read()
with io.open(BODY, encoding="utf-8") as f:
    body = f.read().strip("\n")

decl = text.index("LEVEL_SRCS[LEVEL_COUNT] = {")
brace = text.index("{", decl)
closing = text.index("};", brace)
assert closing > brace

# 校验 body：5 个元素、每元素 64 行、每行 64 字符
parts = [p for p in body.split("\n,") ] if False else body
rows = body.split("\n")
row_len = {len(r.replace("\t", "").strip().strip('"')) for r in rows if r.strip()}
cols = set()
blocks = 0; in_block = False; block_rows = []
elements = []
for ln in rows:
    s = ln.strip()
    if s in (",", ""):
        if in_block:
            elements.append(block_rows); block_rows = []; in_block = False
        continue
    # 提取引号内字符串
    q = s[1:] if s.startswith('"') else s[s.index('"'):]
    q = q.strip()
    inner = q[1:-2] if q.endswith('"') else q[1:-1]
    cols.add(len(inner))
    if not in_block:
        in_block = True
    block_rows.append(inner)
if in_block:
    elements.append(block_rows)
assert len(elements) == 5, f"元素数 {len(elements)} != 5"
for i, el in enumerate(elements):
    assert len(el) == 64, f"第{i}关行数 {len(el)} != 64"
    for ln in el:
        assert len(ln) == 64, f"第{i}关某行 {len(ln)} 列 != 64"
print("校验 OK：5 关 × 64 行 × 64 列")

new_text = text[:brace + 1] + "\n" + body + "\n" + text[closing:]
with io.open(BASE, "w", encoding="utf-8") as f:
    f.write(new_text)
print("已替换 LEVEL_SRCS 数组体")