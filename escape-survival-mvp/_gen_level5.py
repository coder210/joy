#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""生成 128x128 之字形逃离底图（level 5），含 BFS 连通校验与 C 字符串输出。"""
import sys

N = 128  # 与 PF_GRID_N 一致，1 格 = 1 m

# 世界坐标：col c -> x = c-64，row r -> z = r-64（SZ 以 r 增大为"深入/后段"）
# 默认全 '#'，以下开放矩形（inclusive）挖成 '.'。矩形相重叠的格即连通口。
OPEN = [
    ("入口前厅",  21, 23, 48, 64),   # (r1,r2,c1,c2)
    ("扇区1",     23, 36, 30, 70),   # 与入口共享 r23
    ("走廊1",     36, 44, 60, 66),   # 从扇区1 右缘下探
    ("扇区2",     44, 54, 40, 74),   # 从走廊1 顶部进入（右中房）
    ("走廊2",     54, 62, 36, 42),   # 从扇区2 左下下探
    ("扇区3",     62, 74, 26, 64),  # 从走廊2 进入（下左房）
]

# 扇区内记忆点墙柱（'#'），四周留足拼距（pad 需大于 2 格）
PILLARS = [
    (28, 29, 42, 44),  # 扇区1
    (48, 49, 56, 58),  # 扇区2
    (66, 67, 34, 36),  # 扇区3
]

PLAYER = (22, 56)          # P：入口前厅
ENEMIES = [                # E：散布各房/走廊
    (27, 36), (32, 58),    # 扇区1
    (40, 63),              # 走廊1
    (48, 46), (52, 70),    # 扇区2
    (58, 39),              # 走廊2
    (66, 30), (70, 60),    # 扇区3
]
EXIT = (71, 58)            # X：终点闸口（扇区3 右端）


def build():
    g = [['#'] * N for _ in range(N)]
    for name, r1, r2, c1, c2 in OPEN:
        for r in range(r1, r2 + 1):
            for c in range(c1, c2 + 1):
                g[r][c] = '.'
    for r1, r2, c1, c2 in PILLARS:
        for r in range(r1, r2 + 1):
            for c in range(c1, c2 + 1):
                g[r][c] = '#'
    pr, pc = PLAYER
    g[pr][pc] = 'P'
    for e in ENEMIES:
        er, ec = e
        assert g[er][ec] == '.', f"E 落在墙/柱上: {e}"
        g[er][ec] = 'E'
    xr, xc = EXIT
    assert g[xr][xc] == '.', "X 落在墙/柱上"
    g[xr][xc] = 'X'
    return g


def flood(g, sr, sc):
    """从 (sr,sc) 出发，标记所有 4 邻可达的开放格。"""
    seen = [[False] * N for _ in range(N)]
    stack = [(sr, sc)]
    seen[sr][sc] = True
    while stack:
        r, c = stack.pop()
        for dr, dc in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nr, nc = r + dr, c + dc
            if 0 <= nr < N and 0 <= nc < N and not seen[nr][nc]:
                if g[nr][nc] != '#':
                    seen[nr][nc] = True
                    stack.append((nr, nc))
    return seen


def verify(g):
    pr, pc = PLAYER
    seen = flood(g, pr, pc)
    xr, xc = EXIT
    ok = True
    if not seen[xr][xc]:
        print("FAIL: 出口 X 不可达"); ok = False
    for er, ec in ENEMIES:
        if not seen[er][ec]:
            print(f"FAIL: E @({er},{ec}) 不可达"); ok = False
    # 检查最小通道宽度（防止单格通道在 pad 规则下不可走）
    # 局部：任何开放格的 +x/+z 邻若都为墙即单格回头路，至少保证不是唯一要道——这里仅做提示性检查
    return ok


def visual(g):
    for r in range(14, 108):          # 只显示活动区便于目测
        print(''.join(g[r][c] for c in range(26, 102)))


def c_block(g):
    out = []
    for r in range(N):
        line = ''.join(g[r][c] for c in range(N))
        out.append('\t\t"' + line + '\\n"')
    return '\n'.join(out)


if __name__ == "__main__":
    g = build()
    print("=== 连通性校验 ===")
    ok = verify(g)
    print("OK" if ok else "HAS_FAILURE")
    print()
    print("=== 活动区（r14..107, c26..101）===")
    visual(g)
    print()
    print("=== LEVEL_SRCS[4] 片段（供插入）===")
    block = c_block(g)
    print(block)
    with open(__file__.rsplit('\\', 1)[0] + '\\_level5_src.txt', 'w', encoding='utf-8') as f:
        f.write(block)
    print()
    print("已写入 _level5_src.txt，共", len(g), "行")
    sys.exit(0 if ok else 1)