#ifndef CHARACTER_DEF_H
#define CHARACTER_DEF_H

// 角色定义表（2D 版）：数值仅用于角色选择展示与（可选）本地角色体验。
// 参与逻辑结算的数值均为整数，保证帧同步确定性。
enum CharacterId {
    CHAR_WARRIOR  = 0,
    CHAR_GUNNER   = 1,
    CHAR_ASSASSIN = 2,
    CHAR_COUNT    = 3,
};

struct CharacterDef {
    int         char_id;
    const char* name;
    int         max_hp;        // 最大血量
    int         melee_damage;  // 近战伤害
    int         bullet_damage; // 远程子弹伤害
    float       move_speed;    // 移速（仅展示参考）
    // 视觉（仅渲染用）
    uint8_t     fill_r, fill_g, fill_b;
    uint8_t     stroke_r, stroke_g, stroke_b;
};

static const CharacterDef CHARACTER_DEFS[CHAR_COUNT] = {
    // 战士：高血高近战，移速偏慢
    { CHAR_WARRIOR,  "战士", 140, 30, 15, 4.5f, 190, 60,  50,  30, 30, 35 },
    // 射手：低血高远程，移速中等
    { CHAR_GUNNER,   "射手",  80, 18, 30, 5.5f, 60,  120, 80,  30, 30, 35 },
    // 刺客：低血高移速，近战爆发
    { CHAR_ASSASSIN, "刺客",  70, 28, 12, 7.0f, 110, 70,  130, 30, 30, 35 },
};

static inline const CharacterDef& character_def(int char_id)
{
    if (char_id < 0 || char_id >= CHAR_COUNT) char_id = CHAR_WARRIOR;
    return CHARACTER_DEFS[char_id];
}

#endif