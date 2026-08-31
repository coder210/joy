import struct, math, wave, os, random

SR = 22050
OUT = r"D:\github\joy\build\Release\project_proxy3d\sounds"

def write_wav(name, samples):
    path = os.path.join(OUT, name)
    with wave.open(path, 'w') as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SR)
        frames = b''.join(struct.pack('<h', max(-32767, min(32767, int(s * 32767)))) for s in samples)
        w.writeframes(frames)
    print("wrote", path, len(samples)/SR, "s")

def env_attack_decay(n, attack=0.005, decay=0.3):
    out = []
    for i in range(n):
        t = i / SR
        a = min(1.0, t / attack) if attack > 0 else 1.0
        d = math.exp(-t / decay)
        out.append(a * d)
    return out

# 1. melee.wav: 近战挥砍 "嗖" —— 频率 900->180Hz 下滑正弦 + 轻微噪声
def make_melee():
    n = int(SR * 0.18)
    e = env_attack_decay(n, 0.004, 0.05)
    s = []
    for i in range(n):
        t = i / SR
        f = 900.0 * math.exp(-t * 12.0) + 180.0
        ph = 2 * math.pi * (900.0 * (1 - math.exp(-t * 12.0)) / 12.0 + 180.0 * t)
        s.append(math.sin(ph) * 0.7 * e[i] + (random.random()*2-1) * 0.15 * e[i])
    write_wav("melee.wav", s)

# 2. hit.wav: 命中 "啪" —— 短噪声脉冲
def make_hit():
    n = int(SR * 0.08)
    e = env_attack_decay(n, 0.001, 0.02)
    s = [(random.random()*2-1) * e[i] for i in range(n)]
    write_wav("hit.wav", s)

# 3. kill.wav: 击杀重音 —— 噪声脉冲 + 低频正弦
def make_kill():
    n = int(SR * 0.22)
    e = env_attack_decay(n, 0.001, 0.05)
    s = []
    for i in range(n):
        t = i / SR
        noise = (random.random()*2-1) * 0.8
        low = math.sin(2 * math.pi * 130 * t) * 0.6
        s.append((noise + low) * e[i])
    write_wav("kill.wav", s)

# 4. wave.wav: 换波提示 "叮咚" —— 两音阶
def make_wave():
    n = int(SR * 0.5)
    s = []
    for i in range(n):
        t = i / SR
        if t < 0.15:
            f = 523.25  # C5
            e = math.sin(math.pi * t / 0.15)
        else:
            f = 783.99  # G5
            tt = t - 0.15
            e = math.sin(math.pi * tt / 0.35)
        s.append(math.sin(2 * math.pi * f * t) * 0.5 * e)
    write_wav("wave.wav", s)

# 5. bullet.wav: 射击（备用，若 ak47 不合适）—— 短促爆破 + 噪声
def make_bullet():
    n = int(SR * 0.12)
    e = env_attack_decay(n, 0.001, 0.03)
    s = []
    for i in range(n):
        t = i / SR
        noise = (random.random()*2-1) * 0.9
        s.append(noise * e[i])
    write_wav("bullet.wav", s)

random.seed(12345)
make_melee()
make_hit()
make_kill()
make_wave()
make_bullet()
print("done")
