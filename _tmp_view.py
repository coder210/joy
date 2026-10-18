import zlib, struct, sys

def load(path):
    data = open(path, 'rb').read()
    pos, idat, plte, trns = 8, b'', None, None
    w = h = 0
    while pos < len(data):
        ln = struct.unpack('>I', data[pos:pos+4])[0]
        typ = data[pos+4:pos+8]
        ch = data[pos+8:pos+8+ln]
        pos += 12 + ln
        if typ == b'IHDR':
            w, h = struct.unpack('>II', ch[:8])
        elif typ == b'IDAT': idat += ch
        elif typ == b'PLTE': plte = ch
        elif typ == b'tRNS': trns = ch
        elif typ == b'IEND': break
    raw = zlib.decompress(idat)
    stride = w
    out = bytearray(h * stride); prev = bytearray(stride); i = 0
    for y in range(h):
        f = raw[i]; i += 1
        line = bytearray(raw[i:i+stride]); i += stride
        if f == 1:
            for x in range(1, stride): line[x] = (line[x] + line[x-1]) & 255
        elif f == 2:
            for x in range(stride): line[x] = (line[x] + prev[x]) & 255
        elif f == 3:
            for x in range(stride):
                a = line[x-1] if x >= 1 else 0
                line[x] = (line[x] + ((a + prev[x]) >> 1)) & 255
        elif f == 4:
            for x in range(stride):
                a = line[x-1] if x >= 1 else 0
                b = prev[x]; c = prev[x-1] if x >= 1 else 0
                pp = a + b - c
                pa, pb, pc = abs(pp-a), abs(pp-b), abs(pp-c)
                pr = a if (pa <= pb and pa <= pc) else (b if pb <= pc else c)
                line[x] = (line[x] + pr) & 255
        out[y*stride:(y+1)*stride] = line
        prev = line
    return w, h, bytes(out), trns

BASE = r'E:\c++\joy\joy_topdown_template\assets\joy_topdown_template\sprites\enemies'
names = sys.argv[1:] or ['bat', 'skeleton', 'brute', 'cultist', 'spider']
for name in names:
    w, h, px, trns = load(BASE + '\\' + name + '.png')
    print('\n==== %s.png ====' % name)
    print('     ' + ''.join(str(x % 10) for x in range(w)))
    for y in range(h):
        s = ''
        for x in range(w):
            ii = px[y*w+x]
            a = trns[ii] if (trns and ii < len(trns)) else 255
            s += ' ' if a == 0 else ('#' if ii == 1 else ('.' if ii == 2 else 'G'))
        print('%3d  ' % y + s)
