#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""数值验证 fog_pass 的边缘检测边界是否贴合真实墙轮廓。

复刻 fog_pass.cpp 的三段逻辑：
  1) fog_box_edges: 每个墙盒提取两个切线轮廓角(4角中相对盒心方向极角最左/最右)，带遮挡校正；
  2) 排序 + 去重；
  3) boundary_dist: 相邻两命中点之间按角度插值(跨缝>FOG_GAP_DEG 视为圆)；每角得"可见边界半径 rFog"。
然后与"每 0.5° 暴力射线撞墙距离 rTrue"逐角比对，报告误差。
纯验证，不改游戏代码。
"""
import math, re, io, sys

BASE = r"d:\github\joy\project_proxy3d\shared\systems\level_maps.h"
FOG_VIS_RADIUS = 14.0
FOG_GAP_DEG = 40.0
FOG_MESH_ANGLE = 5.0

D2R = math.pi / 180.0
R2D = 180.0 / math.pi

def wrap360(a):
    a = a % 360.0
    if a < 0: a += 360.0
    return a

def wrap180(d):
    if d > 180: d -= 360
    elif d < -180: d += 360
    return d

# 读第 level 关的 ASCII
def load_level(level):
    t = open(BASE, encoding="utf-8").read()
    b = t.index("LEVEL_SRCS[LEVEL_COUNT] = {")
    br = t.index("{", b)
    cl = t.index("};", br)
    body = t[br+1:cl]
    parts = re.findall(r'"([^"]*)"', body)
    start = level * 64
    rows = [p.rstrip() for p in parts[start:start + 64]]
    return rows

def walls_from_map(rows):
    # row r -> gz = r-32 ; col c -> gx = c-32 ；墙格中心 (c-32, r-32)，half=0.5
    ws = []
    for r, line in enumerate(rows):
        for c, ch in enumerate(line):
            if ch in "#":
                ws.append((c-32.0, r-32.0, 0.5, 0.5))
    return ws

def raycast_walls(walls, px, pz, dx, dz):
    best = 1e9
    for (x, z, hx, hz) in walls:
        tn, tf = 0.0, 1e9
        if abs(dx) < 1e-9:
            if px < x-hx or px > x+hx: continue
        else:
            t1 = (x-hx-px)/dx; t2 = (x+hx-px)/dx
            if t1 > t2: t1, t2 = t2, t1
            tn = max(tn, t1); tf = min(tf, t2)
        if abs(dz) < 1e-9:
            if pz < z-hz or pz > z+hz: continue
        else:
            t1 = (z-hz-pz)/dz; t2 = (z+hz-pz)/dz
            if t1 > t2: t1, t2 = t2, t1
            tn = max(tn, t1); tf = min(tf, t2)
        if tf < tn: continue
        t = tn if tn >= 0 else tf
        if t > 1e-4 and t < best: best = t
    return best

# 复刻 fog_box_edges
def fog_box_edges(a, px, pz, walls):
    x, z, hx, hz = a
    if abs(x-px) <= hx and abs(z-pz) <= hz:
        return []
    cs = [(x-hx, z-hz), (x+hx, z-hz), (x+hx, z+hz), (x-hx, z+hz)]
    nearest = min(math.hypot(c[0]-px, c[1]-pz) for c in cs)
    if nearest > FOG_VIS_RADIUS:
        return []
    dxc, dzc = x-px, z-pz
    base = wrap360(math.atan2(dxc, dzc)*R2D)
    rel = []
    for cx, cz in cs:
        ang = wrap360(math.atan2(cx-px, cz-pz)*R2D)
        rel.append(wrap180(ang - base))
    imax = rel.index(max(rel)); imin = rel.index(min(rel))
    hits = []
    for i in (imax, imin):
        ex, ez = cs[i]
        d = math.hypot(ex-px, ez-pz)
        if d < 1e-4: continue
        nx, nz = (ex-px)/d, (ez-pz)/d
        rd = raycast_walls(walls, px, pz, nx, nz)
        dist = rd if rd < d else d
        hits.append({"angle": wrap360(math.atan2(nx, nz)*R2D),
                     "px": px+nx*dist, "pz": pz+nz*dist,
                     "dist": dist, "isWall": True})
    return hits

def build_hits(walls, px, pz):
    hits = []
    for a in walls:
        hits += fog_box_edges(a, px, pz, walls)
    hits.sort(key=lambda h: h["angle"])
    ded = []
    for h in hits:
        if ded and abs(h["angle"] - ded[-1]["angle"]) < 0.3:
            if h["dist"] < ded[-1]["dist"]:
                ded[-1] = h
        else:
            ded.append(h)
    return ded

def boundary(hits, n, i, ang, px, pz):
    A = hits[i]; B = hits[(i+1) % n]
    start, end = A["angle"], B["angle"]
    if end < start: end += 360.0
    span = end - start
    if span < 0.3 or span > 358.5:
        return FOG_VIS_RADIUS
    gap = span > FOG_GAP_DEG
    if gap:
        return FOG_VIS_RADIUS
    t = (ang - start) / span
    bx = A["px"] + (B["px"]-A["px"])*t
    bz = A["pz"] + (B["pz"]-A["pz"])*t
    d = math.hypot(bx-px, bz-pz)
    return d if d < FOG_VIS_RADIUS else FOG_VIS_RADIUS

def fog_radius_at(hits, px, pz, ang, n):
    # 找到 ang 归属的段 (按 [0,360) 由 hits 划分)
    for i in range(n):
        A = hits[i]; B = hits[(i+1) % n]
        start, end = A["angle"], B["angle"]
        if end < start: end += 360.0
        a = ang
        if a < start: a += 360.0
        if start <= a < end:
            return boundary(hits, n, i, a, px, pz)  # a 已归一到本段
    return boundary(hits, n, n-1, ang, px, pz)

def true_radius_at(walls, px, pz, ang):
    rd = raycast_walls(walls, px, pz, math.sin(ang*D2R), math.cos(ang*D2R))
    return min(rd, FOG_VIS_RADIUS)

def check(level, px, pz):
    rows = load_level(level)
    walls = walls_from_map(rows)
    hits = build_hits(walls, px, pz)
    if not hits:
        return None, None, None, len(walls), 0
    n = len(hits)
    step = 0.5
    max_err = 0.0; bad = 0; total = 0; sum_err = 0.0
    a = 0.0
    while a < 360.0:
        rf = fog_radius_at(hits, px, pz, a, n)
        rt = true_radius_at(walls, px, pz, a)
        err = abs(rf - rt)
        total += 1; sum_err += err
        if err > 0.25:  # 超过半格视为"不贴合"
            bad += 1
        if err > max_err: max_err = err
        a += step
    return max_err, bad, total, len(walls), n

def main():
    rows = load_level(0)
    # 可通行格（非 # 墙）上采样覆盖全图的探测点
    open_cells = [(c-32.0, r-32.0) for r, line in enumerate(rows)
                  for c, ch in enumerate(line) if ch in ".PEX"]
    if not open_cells:
        print("无可通行格"); return
    import random
    random.seed(7)
    probes = random.sample(open_cells, min(len(open_cells), 9))
    print("第1关 可通行格=%d" % len(open_cells))
    print(f"{'玩家位置':<20}{'fog边界点':>8}{'最大误差m':>9}{'不贴合(>0.25m)':>18}{'样本角':>7}")
    g_worst = (0.0, None)
    for px, pz in probes:
        maxe, bad, total, nwalls, nhits = check(0, px, pz)
        if maxe is None:
            print(f"{str((round(px,1),round(pz,1))):<20}{'无墙':>8}"); continue
        pct = 100.0*bad/total
        if maxe > g_worst[0]: g_worst = (maxe, (px, pz, bad, total))
        print(f"{str((round(px,1),round(pz,1))):<20}{nhits:>8}{maxe:>9.3f}{f'{bad}/{total}({pct:.1f}%)':>18}{total:>7}")
    print("worst : max_err=%.3f m @ %s" % (g_worst[0], g_worst[1]))

if __name__ == "__main__":
    main()