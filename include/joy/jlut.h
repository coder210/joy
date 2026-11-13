#ifndef JOY_JLUT_H
#define JOY_JLUT_H

/* 两张定点三角查表，唯一定义在 src/joy_lut.c。
 *
 * 历史上它们是本头文件里的 static 数组，经 jmath.h -> joy.h 被每个包含 joy.h
 * 的 .c 各复制一份（约 3.3 MB 数据 x 数十个 TU）。现在改成 extern 声明 +
 * 单一定义。长度必须显式给出：jmath.h 用 sizeof(tbl)/sizeof(tbl[0]) 推导表长，
 * 不完整数组类型会让 sizeof 编译失败。 */
#define JOY_SIN_LUT_LENGTH 205887
#define JOY_TAN_LUT_LENGTH 205887

extern const long long SIN_LUT[JOY_SIN_LUT_LENGTH];
extern const long long tan_lut[JOY_TAN_LUT_LENGTH];

#endif /* JOY_JLUT_H */
