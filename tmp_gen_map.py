# -*- coding: utf-8 -*-
# 生成"求生之路式"线性地图：一串房间沿 X 轴（列）从左(入口P)到右(出口X)推进，
# 相邻房间之间有带门洞的隔墙，内部放巨石掩体。确定性种子 → 固定图，可直接内联。
import random, sys

N = 128
LEVEL_COUNT = 4
ENEMY_COUNT = [14, 16, 18, 20]   # 每关敌人出生点数（与 level_maps.h 保持一致）

def build(seed, nrooms, widths, ecount):
    rng = random.Random(seed)
    g = [['.' for _ in range(N)] for _ in range(N)]
    # 边界墙
    for c in range(N):
        g[0][c] = g[N-1][c] = '#'
    for r in range(1, N-1):
        g[r][0] = g[r][N-1] = '#'
    # 计算隔墙列位置：先 rooms 再隔墙，隔墙在每两个房间之间
    cols_open = []  # 每个房间可用的列区间 [a,b]
    a = 1
    for w in widths:
        b = a + w - 1
        cols_open.append((a, b))
        a = b + 2  # +2 留 1 列隔墙 + 1 列缓冲
    dividers = [(b+1) for (_, b) in cols_open[:-1]]  # 隔墙列
    # 每张隔墙：整列墙，开 2 个门洞（错开的行 → 蛇行）
    door_rows = []
    p_row = None
    x_row = None
    # 入口/出口门洞行
    p_row = rng.randint(20, N-21)
    x_row = rng.randint(20, N-21)
    # 画隔墙与门洞
    for i, dc in enumerate(dividers):
        for r in range(1, N-1):
            g[r][dc] = '#'
        d1 = rng.randint(15, 50); d2 = rng.randint(75, 112)
        g[d1][dc] = '.'; g[d2][dc] = '.'
        door_rows.append((dc, d1, d2))
    # 每个房间放若干巨石掩体（半高/局部，避免堵死），并保证连通
    def bfs():
        # 从 P 出发的可达区域
        from collections import deque
        visited = [[False]*N for _ in range(N)]
        q = deque([(p_row,1)])
        visited[p_row][1] = True
        while q:
            r,c = q.popleft()
            if r==x_row and c==N-2: return True
            for dr,dc in ((0,1),(0,-1),(1,0),(-1,0)):
                nr,nc = r+dr,c+dc
                if 0<=nr<N and 0<=nc<N and not visited[nr][nc] and g[nr][nc]!='#':
                    visited[nr][nc]=True; q.append((nr,nc))
        return False
    # 放掩体：逐房间放，直到连通校验失败就撤销最后一个（保证连通）
    attempts = 0
    while True:
        room_idx = attempts % (len(cols_open))
        ra, rb = cols_open[room_idx]
        if rb - ra < 6:
            break
        wr = rng.randint(3, 7)
        hr = rng.randint(3, 9)
        cr = rng.randint(2, N-12)
        cc = rng.randint(ra+1, ra+4)
        if cc + wr > rb - 1: break
        # 放之前先检查是否堵死（粗略：放完校验）
        snapshot = [row[:] for row in g]
        for rr in range(cr, min(cr+hr, N-1)):
            for c2 in range(cc, cc+wr):
                g[rr][c2] = '#'
        if not bfs():
            g = snapshot
        attempts += 1
        if attempts > 400: break
    # 强制重连通校验（若有任何断开则整体放弃掩体重来不做 —— 上面已保证）
    if not bfs():
        # 兜底：去掉所有内部掩体（仅保留隔墙），必然连通
        for r in range(1, N-1):
            for c in range(1, N-1):
                if c not in dividers:
                    g[r][c] = '.'
    # 放入口/出口标记
    g[p_row][1] = 'P'
    g[x_row][N-2] = 'X'
    # 放敌人出生点：在房间开放区上，避开出入口与门洞列，均匀散布
    placed = 0
    tries = 0
    room_indices = []
    while placed < ecount and tries < 8000:
        r = rng.randint(2, N-3)
        c = rng.randint(2, N-3)
        tries += 1
        if g[r][c] != '.': continue
        if c in dividers: continue
        if (r==p_row and c<=3) or (r==x_row and c>=N-4): continue
        if (p_row-x_row)==0 and r==p_row and c in (1,N-2): continue
        g[r][c] = 'E'
        placed += 1
        if not room_indices or r != room_indices[-1]:
            room_indices.append(r)
    return g, (p_row, x_row)

def to_src(gs):
    lines = []
    for r in range(N):
        row = ''.join(gs[r])
        lines.append(row)
    # 生成 C 字符串字面量：每行一个 "..." 相邻自动拼接，行尾加 \n
    out = []
    for i, row in enumerate(lines):
        out.append('\t\t"%s\\n"' % row)
    return '\n'.join(out)

blocks = []
for lv in range(1, LEVEL_COUNT+1):
    seed = lv * 971
    nrooms = 6
    widths = [20,20,20,20,20,21]
    g, doors = build(seed, nrooms, widths, ENEMY_COUNT[lv-1])
    src = to_src(g)
    blocks.append(src)
    # 校验每行长度 & 字符
    assert all(len(row)==128 for row in g), "row length !=128 on level %d"%lv
    # 校验一个 PX 一个 X，E 数目
    p = sum(row.count('P') for row in g); x = sum(row.count('X') for row in g)
    e = sum(row.count('E') for row in g)
    print("level %d: P=%d X=%d E=%d (expect %d)" % (lv,p,x,e,ENEMY_COUNT[lv-1]))
    assert p==1 and x==1, "P/X count wrong on level %d"%lv
    assert e==ENEMY_COUNT[lv-1], "E count wrong on level %d"%lv

header = "inline const char* LEVEL_SRCS[LEVEL_COUNT] = {\n" + "\n".join(blocks) + "\n};"
open(r'e:/c++/joy/tmp_level_src.h','w',encoding='utf-8').write(header)
print("OK -> tmp_level_src.h, len=", len(header))