#ifndef BSPLINE_H
#define BSPLINE_H

#include "../Point.h"
#include <vector>

std::vector<Point> drawUBS(const std::vector<Point>& control, int degree);
std::vector<Point> drawNUBS(
    const std::vector<Point>& control,
    const std::vector<double>& knots,
    int degree);

#endif