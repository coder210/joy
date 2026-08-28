import sys
from PIL import Image

def analyze(img_path):
    img = Image.open(img_path).convert("RGB")
    w, h = img.size
    px = img.load()

    # 敌人/角色本体：近黑色 (r,g,b 都 < 60 且相近)
    # 血环前景：30,30,35；淡墨背景环：180,180,185@160 叠加地面 ≈ 208
    dark_blobs = []
    visited = set()
    for y in range(h):
        for x in range(w):
            if (x, y) in visited:
                continue
            r, g, b = px[x, y]
            if r < 60 and g < 60 and b < 70:
                # BFS 找连通暗块
                stack = [(x, y)]
                blob = []
                while stack:
                    cx, cy = stack.pop()
                    if (cx, cy) in visited:
                        continue
                    visited.add((cx, cy))
                    blob.append((cx, cy))
                    for dx, dy in ((-1,0),(1,0),(0,-1),(0,1)):
                        nx, ny = cx+dx, cy+dy
                        if 0 <= nx < w and 0 <= ny < h and (nx, ny) not in visited:
                            nr, ng, nb = px[nx, ny]
                            if nr < 60 and ng < 60 and nb < 70:
                                stack.append((nx, ny))
                if len(blob) >= 20:
                    dark_blobs.append(blob)

    print(f"{img_path}: size={w}x{h} dark_blobs={len(dark_blobs)}")
    for blob in sorted(dark_blobs, key=len, reverse=True)[:6]:
        xs = [p[0] for p in blob]
        ys = [p[1] for p in blob]
        cx = sum(xs) / len(blob)
        cy = sum(ys) / len(blob)
        print(f"  blob n={len(blob)} center=({cx:.0f},{cy:.0f}) bbox=({min(xs)},{min(ys)})-({max(xs)},{max(ys)})")

    # 在最大暗块中心周围找血环像素（暗墨 30,30,35 附近，比本体 22,22,26 略亮或同色）
    if dark_blobs:
        blob = max(dark_blobs, key=len)
        xs = [p[0] for p in blob]
        ys = [p[1] for p in blob]
        cx = sum(xs) / len(blob)
        cy = sum(ys) / len(blob)
        ring_px = 0
        for y in range(int(cy)-20, int(cy)+21):
            for x in range(int(cx)-20, int(cx)+21):
                if 0 <= x < w and 0 <= y < h:
                    r, g, b = px[x, y]
                    if 20 <= r <= 50 and 20 <= g <= 50 and 20 <= b <= 60:
                        ring_px += 1
        print(f"  ring-ish pixels near largest blob center: {ring_px}")

def main():
    for p in sys.argv[1:]:
        analyze(p)

if __name__ == "__main__":
    main()
