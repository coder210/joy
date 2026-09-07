#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""为 5 关生成 64×64 高密度「蛇形主干 + 深死胡同 + 房内柱子」地图。

设计（用户要求：同尺寸加密度、10~15 分钟、有迷路因素，但要单向明确终点）：
  - 单向蛇形主干：从左上角出发，3 格宽走廊之字蛇行贯穿全图到达右下角出口 X。
    只建奇/偶交替折返，永不回头，出口明确。
  - 深死胡同：从主干侧边伸出 2 格宽的支路，尽头封死 → 玩家折返的探索/迷路成本。
  - 房内柱子/短隔墙：隔段布置，切割视野、增加遮蔽与记忆点（保证两侧留空不阻断）。

寻路 pad 外扩 0.6m：走廊须 >=2 格宽才可通行；柱子两侧各留 >=1 格。
世界：col c -> gx = c-32，row r -> gz = r-32。
"""
import io, sys
from collections import deque

N = 64

def carve(g, r0, r1, c0, c1):
    for r in range(r0, r1 + 1):
        for c in range(c0, c1 + 1):
            g[r][c] = '.'

# 蛇形骨架 = 显式矩形带(横带上下堆叠左右交替 + 竖带两侧连接)，每带 >=4 格宽。
# 之后在走廊内部自动加"隔断墙"(留缺口切视野、制造绕行/迷宫)，仅保留不断联者。
def build_base(gen_p: tuple = (7, 8), gen_x: tuple = (43, 54)):
    g = [['#'] * N for _ in range(N)]
    BAND_ROW = 6
    BAND_GAP = 6
    C_LEFT, C_RIGHT = 7, 56
    n_h = 7
    rects = []
    for i in range(n_h):
        r0 = BAND_ROW + i * BAND_GAP
        rects.append((r0, r0 + 3, C_LEFT, C_RIGHT))
    for i in range(n_h - 1):
        r_top = BAND_ROW + i * BAND_GAP
        # 竖带必须向上/向下各伸入相连两横带内部 2 格（重叠），否则 1 格接缝被 pad 吞掉断连
        r0 = r_top + 2                 # 顶部横带下沿内 2 格
        r1 = r0 + (BAND_GAP + 1)       # 越过下一横带：到下一条横带内 2 格
        if i % 2 == 0:
            rects.append((r0, r1, C_RIGHT - 1, C_RIGHT + 2))   # 右端：跨 4 列，左起含横带末列
        else:
            rects.append((r0, r1, C_LEFT - 1, C_LEFT + 2))     # 左端
    for r0, r1, c0, c1 in rects:
        for r in range(r0, r1 + 1):
            for c in range(c0, c1 + 1):
                g[r][c] = '.'

    # --- 蜂窝隔断：在蛇形走廊内部生成"迷书架"。逐个尝试，破坏连通则放弃。 ---
    # pad 语义：墙自身 + 其 +x/+z 邻格判不可通行。故在宽 W 走廊中放一段墙，缺口须 >=2 格
    # （否则缺口唯一通道会被相邻墙的 pad 封堵）。
    def try_add(r, c):
        if g[r][c] != '.': return False
        g[r][c] = '#'
        seen, _ = pad_reach(g, *gen_p)
        if (gen_x) not in seen:           # 断联 → 回滚
            g[r][c] = '.'
            return False
        return True

    # 横向隔断：横带(h=4)内放"留 2 格缺口"的竖墙，隔一段切一段
    for i in range(n_h):
        r0 = BAND_ROW + i * BAND_GAP
        for cc in range(C_LEFT + 7, C_RIGHT - 2, 10):
            gap_r = r0 + (2 if (i // 1) % 2 else 0)     # 缺口位置（0..1 两格）交替
            for rr in range(r0, r0 + 4):
                if gap_r <= rr <= gap_r + 1: continue   # 留 2 格缺口
                try_add(rr, cc)
    # 纵向隔断：竖带(w=4)内放水平短墙留 2 格缺口
    for i in range(n_h - 1):
        r_top = BAND_ROW + i * BAND_GAP
        r0, r1 = r_top + 4, r_top + 4 + 3
        c0 = (C_RIGHT if i % 2 == 0 else C_LEFT) + 1
        for rr in range(r0 + 3, r1 - 1, 3):
            gap_c = c0 + (1 if (rr // 3) % 2 else 0)     # 缺口 2 格
            for cc in range(c0 - 1, c0 + 2):
                if gap_c <= cc <= gap_c + 1: continue
                try_add(rr, cc)
    return g

# 自动挑出口：固定为底部横带最右端可走格（蛇形末端附近），与骨架 gen_x 一致
EXIT = (43, 54)
def exit_cell(g):
    return EXIT

def build(item):
    g = build_base()
    pg = (7, 8)
    # 按 diff 在走廊开放格确定性加 1x1 柱（不断联），制造各关差异
    need = item["diff"]
    cand = []
    for r in range(6, 40):
        for c in range(7, 56):
            if g[r][c] == '.' and (r, c) != pg:
                cand.append((r, c))
    for (r, c) in cand:
        if need <= 0: break
        g[r][c] = '#'
        seen, _ = pad_reach(g, *pg)
        if exit_cell(g) in seen:
            need -= 1
        else:
            g[r][c] = '.'
    return g

def bfs_len(g, sr, sc, tr, tc):
    if g[sr][sc] == '#' or g[tr][tc] == '#': return -1
    vis = {(sr, sc): 0}
    dq = deque([(sr, sc)])
    while dq:
        r, c = dq.popleft()
        if (r, c) == (tr, tc): return vis[(r, c)]
        for dr, dc in ((1,0),(-1,0),(0,1),(0,-1)):
            nr, nc = r+dr, c+dc
            if 0<=nr<N and 0<=nc<N and g[nr][nc] != '#' and (nr,nc) not in vis:
                vis[(nr,nc)] = vis[(r,c)] + 1
                dq.append((nr,nc))
    return -1

def flood(g, sr, sc):
    seen = [[False]*N for _ in range(N)]
    st = [(sr, sc)]; seen[sr][sc] = True
    while st:
        r, c = st.pop()
        for dr, dc in ((1,0),(-1,0),(0,1),(0,-1)):
            nr, nc = r+dr, c+dc
            if 0<=nr<N and 0<=nc<N and not seen[nr][nc] and g[nr][nc] != '#':
                seen[nr][nc] = True; st.append((nr,nc))
    return seen

def cstr(grid):
    return "\n".join('\t\t"' + "".join(row) + '\\n"' for row in grid)

# 关卡定义：P 固定左上；X 随关微调；pillars=主链内遮视野柱/墙（矩形起止 r0,r1,c0,c1）。
# 柱子必须贴走廊边放、剩余通道 >= 2 格，否则 pad(0.6m) 后 1 格宽不可通行会断连。
def pl(*cells):      # 每个 ((r,c)) → 1x1 柱；柱子单格最安全，3 宽走廊四周仍有通路
    return [(r, r, c, c) for (r, c) in cells]

# 关卡定义：P 固定左上(7,8)；X 固定底部；敌人由 main 在可达格中确定性挑选（远离 P）。
# diff=每关在基地上额外加 1x1 柱的数量（自动挑选不阻断格），制造各关差异。
LEVELS = [
    dict(name="训练场", diff=0),
    dict(name="回廊",   diff=12),
    dict(name="军营",   diff=20),
    dict(name="绝地",   diff=30),
    dict(name="迷宫",   diff=40),
]

# 生成 pd 语义 walkable：墙格 + 墙的 +x/+z 邻格 判不可通行（镜像 C++ pf_build_walkable）
def walkable_grid(g):
    block = [[False]*N for _ in range(N)]
    for r in range(N):
        for c in range(N):
            if g[r][c] != '#': continue
            block[r][c] = True
            if c+1 < N: block[r][c+1] = True      # +x 邻
            if r+1 < N: block[r+1][c] = True      # +z 邻
    return block

def pad_reach(g, sr, sc):
    blk = walkable_grid(g)
    if blk[sr][sc]: return set(), blk
    seen = {(sr, sc)}
    st = [(sr, sc)]
    while st:
        r, c = st.pop()
        for dr, dc in ((1,0),(-1,0),(0,1),(0,-1)):
            nr, nc = r+dr, c+dc
            if 0<=nr<N and 0<=nc<N and not blk[nr][nc] and (nr,nc) not in seen:
                seen.add((nr,nc)); st.append((nr,nc))
    return seen, blk

def main():
    bodies = []
    for it in LEVELS:
        g = build(it)
        pg = (7, 8)
        # 出口格
        xr, xc = exit_cell(g)
        g[pg[0]][pg[1]] = 'P'
        # 敌人在可达格中确定性挑 diff*0 + 8 只，按行主序相隔若干格，靠近深处
        seen, _ = pad_reach(g, *pg)
        open_cells = [(r, c) for r in range(6, 46) for c in range(7, 56)
                      if g[r][c] == '.' and (r, c) != pg and (r, c) != (xr, xc)]
        # 按"离 P 的 BFS 距离"降序取最远一批（分散在走廊深处），最多 10
        dists = sorted(((bfs_len(g, *pg, *o), o) for o in open_cells),
                       key=lambda t: t[0])
        count = it.get("enemies", 8)
        enemies = [o for d, o in dists[-count:]][::-1]
        for e in enemies:
            g[e[0]][e[1]] = 'E'
        # pad 语义最终校验（包含 P/X/E 字符转 '.'）
        gchk = [[c if c not in 'PEX' else '.' for c in row] for row in g]
        seen2, _ = pad_reach(gchk, *pg)
        bad = []
        if (xr, xc) not in seen2: bad.append(f"出口X({xr},{xc})不可达")
        for e in enemies:
            if e not in seen2: bad.append(f"E{e}不可达")
        if bad:
            print(f"FAIL [{it['name']}]: " + "; ".join(bad)); sys.exit(1)
        d = bfs_len(gchk, *pg, xr, xc)
        open_n = sum(1 for r in range(N) for c in range(N) if g[r][c] != '#')
        print(f"OK [{it['name']}]: P{pg} X{(xr,xc)} 主链最短{d}m 可走格{open_n} 利用率{open_n*100//(N*N)}% 敌{len(enemies)}")
        bodies.append(cstr(g))
    out = []
    for i, b in enumerate(bodies):
        if i: out.append(",")
        out.append(b)
    with io.open(r"d:\github\joy\escape-survival-mvp\_levels64_body.txt", "w", encoding="utf-8") as f:
        f.write("\n".join(out))
    print(f"已写出 _levels64_body.txt（5 关 × {N} 行，蜂窝蛇形版）")

if __name__ == "__main__":
    main()