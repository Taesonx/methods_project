#ifndef MATHFUNCTION_H
#define MATHFUNCTION_H

#include <cmath>

inline double f29(double x, double a, double b, double c, double d, double e) {
    if (x < 0) return 0;                    // x < 0 — не определено
    if (x >= 0 && x < 1) return a * sqrt(x);  // Первая ветка: a·√x
    if (x >= 1 && x < 2) return b / x;        // Вторая ветка: b/x
    return c * x * x - d * x + e;             // Третья ветка: квадратичная
}

#endif // MATHFUNCTION_H
