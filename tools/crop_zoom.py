# -*- coding: utf-8 -*-
# 裁剪放大截图区域，验证脚下血环；并统计深色像素分布
import sys
from PIL import Image

def main():
    img_path = sys.argv[1] if len(sys.argv) > 1 else r"D:\github\joy\tools\hp_shots\hp_04.png"
    out_path = sys.argv[2] if len(sys.argv) > 2 else r"D:\github\joy\tools\hp_shots\zoom.png"
    cx = int(sys.argv[3]) if len(sys.argv) > 3 else 248
    cy = int(sys.argv[4]) if len(sys.argv) > 4 else 180
    half = int(sys.argv[5]) if len(sys.argv) > 5 else 110
    scale = int(sys.argv[6]) if len(sys.argv) > 6 else 3

    img = Image.open(img_path).convert("RGB")
    w, h = img.size
    x0 = max(0, cx - half)
    y0 = max(0, cy - half)
    x1 = min(w, cx + half)
    y1 = min(h, cy + half)
    crop = img.crop((x0, y0, x1, y1))
    zoom = crop.resize((crop.width * scale, crop.height * scale), Image.NEAREST)
    zoom.save(out_path)
    print(f"image {w}x{h} crop {x0},{y0} {x1-x0}x{y1-y0} -> {out_path}")

    # 像素统计
    px = img.load()
    dark = 0
    ring_like = 0
    for y in range(y0, y1):
        for x in range(x0, x1):
            r, g, b = px[x, y]
            if r < 90 and g < 90 and b < 100:
                dark += 1
            if 45 <= r <= 75 and 45 <= g <= 75 and 50 <= b <= 85:
                ring_like += 1
    print(f"dark={dark} ringlike={ring_like}")

if __name__ == "__main__":
    main()
