# -*- coding: utf-8 -*-
# 把"血环色"像素(45-75灰度)高亮为红色，其余置灰，可视化血环位置
import sys
from PIL import Image

def main():
    img_path = sys.argv[1] if len(sys.argv) > 1 else r"D:\github\joy\tools\hp_shots\hp_04.png"
    out_path = sys.argv[2] if len(sys.argv) > 2 else r"D:\github\joy\tools\hp_shots\ring_vis.png"

    img = Image.open(img_path).convert("RGB")
    w, h = img.size
    px = img.load()
    vis = Image.new("RGB", (w, h), (255, 255, 255))
    vp = vis.load()
    clusters = []

    for y in range(h):
        for x in range(w):
            r, g, b = px[x, y]
            if 45 <= r <= 75 and 45 <= g <= 75 and 50 <= b <= 85:
                vp[x, y] = (255, 0, 0)
            else:
                vp[x, y] = (r, g, b)

    vis.save(out_path)
    print(f"saved {out_path}")

if __name__ == "__main__":
    main()
