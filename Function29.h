#pragma once
// Функция №29 с параметрами
// Область определения: x >= 0
// Параметры: a, b, c, d, e
double f29(double x, double a, double b, double c, double d, double e) {
    if (x < 0) return 0;
    if (x >= 0 && x < 1) return a * sqrt(x);
    if (x >= 1 && x < 2) return fabs(b / x);
    return fabs(c * x * x - d * x + e);
}