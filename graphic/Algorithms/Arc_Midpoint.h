#ifndef ARC_H
#define ARC_H

#include "../Point.h"
#include <vector>
#include <cmath>

// static int arc_threshold = 5;
std::vector<Point> arcMidpoint(Point center, int radius, double start, double end);

#endif