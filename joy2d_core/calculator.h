#ifndef CALCULATOR_H
#define CALCULATOR_H
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

        JOY_API double add(double a, double b);
        JOY_API double subtract(double a, double b);
        JOY_API double multiply(double a, double b);
        JOY_API double divide(double a, double b);

#ifdef __cplusplus
}
#endif

#endif /* CALCULATOR_H */