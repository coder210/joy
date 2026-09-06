# -*- coding: utf-8 -*-
import io, sys
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')

LM = r'e:/c++/joy/project_proxy3d/shared/systems/level_maps.h'
SRC = r'e:/c++/joy/tmp_level_src.h'

t = open(LM, encoding='utf-8').read()
head = open(SRC, encoding='utf-8').read()

start_marker = 'inline const char* LEVEL_SRCS'
s = t.index(start_marker)
e = t.index('};', s) + len('};')   # 数组结束

new_t = t[:s] + head + t[e:]

# 校验 E 数量与 LEVEL_ENEMY_COUNT 一致
import re
base_level = new_t[t.rindex('};', 0, t.index('inline constexpr int LEVEL_ENEMY_COUNT')) ]
ec = re.search(r'LEVEL_ENEMY_COUNT\[LEVEL_COUNT\] = \{([^}]*)\}', new_t)
print('ENEMY_COUNT line:', ec.group(0) if ec else None)

open(LM, 'w', encoding='utf-8').write(new_t)
print('spliced OK, level_maps.h len=', len(new_t))

# 预览第一张图的行（解码 C 转义 \n）
rows = re.findall(r'"([^"]*)"', head)
idx = 0
r0 = rows[0]
print('rows total:', len(rows))
print('sample row0:', r0[:128])
print('sample row1:', rows[1][:128])