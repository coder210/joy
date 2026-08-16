#ifndef STATE_CHECKSUM_H
#define STATE_CHECKSUM_H
#include <joy/jmath.h>
#include <joy/joy.h>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdio>
#include <cstring>

// 帧同步状态指纹：两端共用同一公式，保证 MD5 完全一致。
// 指纹来源：所有玩家 (id, LogicPosition.x, LogicPosition.y)。
struct ChecksumEntry
{
        int id;
        fp_t x, y;
};

// 计算玩家状态 MD5（hex 32 字符 + '\0' 写入 out，out 需 >= 33 字节）。
// 公式（两端必须逐字节一致）：
//   1. 按 id 升序排序
//   2. 拼接 "%d:%lld:%lld,"（坐标按 long long 取整）
//   3. utils_md5x
// 注意：此函数在"模拟本帧之前"调用，与服务端 collect_input 的时机一致。
static inline void compute_players_md5(std::vector<ChecksumEntry>& entries, char* out, int out_cap)
{
        std::sort(entries.begin(), entries.end(),
                [](const ChecksumEntry& a, const ChecksumEntry& b) {
                        return a.id < b.id;
                });

        std::string s;
        char buf[64];
        for (auto& e : entries) {
                int n = snprintf(buf, sizeof(buf), "%d:%lld:%lld,",
                        e.id, (long long)e.x, (long long)e.y);
                s.append(buf, n);
        }

        if (out_cap <= 0) return;
        memset(out, 0, out_cap);
        utils_md5x(s.c_str(), s.size(), out);
}

#endif
