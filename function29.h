#ifndef FUNCTION29_H
#define FUNCTION29_H

#include <cmath>

inline double calculateFunction(double x, double a, double b, double c, double d, double e)
{
    if (x < 0) return 0;
    if (x < 1) return a * sqrt(x);
    if (x < 2) return b / x;
    return c * x * x - d * x + e;
}

#endif // FUNCTION29_H
