#include "mathfunction.h"

double MathFunction::calculate(double x, double a, double b, double c, double d, double e)
{
    if (x < 0) return 0;

    if (x >= 0 && x < 1) {
        return a * sqrt(x);
    }
    if (x >= 1 && x < 2) {
        return b / x;
    }
    return c * x * x - d * x + e;
}
