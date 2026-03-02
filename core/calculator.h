#ifndef CALCULATOR_H
#define CALCULATOR_H

// 跨平台符号导出/导入宏
// 使用方式：
//   - 构建动态库时：定义 CALCULATOR_EXPORTS
//   - 使用动态库时：不要定义 CALCULATOR_EXPORTS（自动导入）
//   - 构建或使用静态库时：定义 CALCULATOR_STATIC（宏展开为空）

#if defined(CALCULATOR_STATIC)
    // 静态库：不需要任何导入导出声明
#define CALCULATOR_API
#elif defined(_WIN32) || defined(__CYGWIN__)
    // Windows 或 Cygwin：使用 __declspec(dllexport/import)
#ifdef CALCULATOR_EXPORTS
#define CALCULATOR_API __declspec(dllexport)
#else
#define CALCULATOR_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) && (__GNUC__ >= 4) || defined(__clang__)
    // GCC >= 4 或 Clang：使用可见性属性
#define CALCULATOR_API __attribute__((visibility("default")))
#else
    // 其他编译器：无特殊声明
#define CALCULATOR_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

        CALCULATOR_API double add(double a, double b);
        CALCULATOR_API double subtract(double a, double b);
        CALCULATOR_API double multiply(double a, double b);
        CALCULATOR_API double divide(double a, double b);

#ifdef __cplusplus
}
#endif

#endif /* CALCULATOR_H */