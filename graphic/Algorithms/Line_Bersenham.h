#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "../Point.h"
#include <vector>

std::vector<Point> lineBresenham(Point p1, Point p2);
std::vector<Point> thickLine(Point p1, Point p2, int width);
std::vector<Point> dashedLine(Point p1, Point p2, int pattern, int lineWidth);

#endif