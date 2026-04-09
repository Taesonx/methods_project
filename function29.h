#ifndef FUNCTION29_H
#define FUNCTION29_H

#include <cmath>

double f29(double x, double a, double b, double c, double d, double e) {
    if (x < 0) return 0;
    if (x >= 0 && x < 1) return a * sqrt(x);
    if (x >= 1 && x < 2) return fabs(b / x);
    return fabs(c * x * x - d * x + e);
}

#endif
