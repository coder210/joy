#include "calculator.h"

double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    // 简单处理除零：返回 0（实际应用可返回 NaN 或设置错误码）
    if (b == 0.0) {
        return 0.0;
    }
    return a / b;
}