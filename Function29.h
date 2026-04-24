#ifndef FUNCTION29_H
#define FUNCTION29_H

#include <cmath>

double f29(double x, double a, double b, double c, double d, double e) {
    if (x >= 0 && x < 1) return a * sqrt(x);
    if (x >= 1 && x < 2) return b / x;
    if (x >= 2) return c * x * x - d * x + e;
    return 0; // для x < 0
}

#endif
