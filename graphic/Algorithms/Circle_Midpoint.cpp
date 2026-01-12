#define _USE_MATH_DEFINES
#include "Circle_Midpoint.h"
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::vector<Point> circleMidpoint(Point center, int radius) {
	std::vector<Point> points;
    int x = 0; 
    int y = radius;
    int d = 1 - radius;

	while (x <= y) {
        points.push_back(Point(center.x + x, center.y + y));
        points.push_back(Point(center.x - x, center.y + y));
        points.push_back(Point(center.x + x, center.y - y));
        points.push_back(Point(center.x - x, center.y - y));
        points.push_back(Point(center.x + y, center.y + x));
        points.push_back(Point(center.x - y, center.y + x));
        points.push_back(Point(center.x + y, center.y - x));
        points.push_back(Point(center.x - y, center.y - x));

        if (d < 0)  {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }

    return points;
}
