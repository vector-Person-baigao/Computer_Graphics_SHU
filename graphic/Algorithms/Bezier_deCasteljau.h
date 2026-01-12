#ifndef BEZIER_H
#define BEZIER_H

#include "../Point.h"
#include <vector>

std::vector<Point> deCasteljau(const std::vector<Point>& control);

#endif