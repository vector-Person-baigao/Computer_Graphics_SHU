#include "Arc_Midpoint.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::vector<Point> arcMidpoint(Point center, int radius, double start, double end) {
    std::vector<Point> points;
    
    start = fmod(start + M_PI, 2 * M_PI) - M_PI;
    end = fmod(end + M_PI, 2 * M_PI) - M_PI;
    if(start < end) start += 2 * M_PI;
    // if(fabs(end - start) * radius < arc_threshold) start = 0, end = 2*M_PI;
    auto in_range = [&](double angle) {
        if(angle < end) angle += 2 * M_PI;
        if(angle > start) angle -= 2 * M_PI;
        return end < angle && angle < start;
    };

    int x = 0;
    int y = radius;
    int d = 1 - radius;

    while (x <= y) {
        auto addPoint = [&](int x, int y) {
            double angle = atan2(y - center.y, x - center.x);
            if (in_range(angle)) {
                points.push_back(Point(x, y));
            }
        };

        addPoint(center.x + x, center.y + y);
        addPoint(center.x - x, center.y + y);
        addPoint(center.x + x, center.y - y);
        addPoint(center.x - x, center.y - y);
        addPoint(center.x + y, center.y + x);
        addPoint(center.x - y, center.y + x);
        addPoint(center.x + y, center.y - x);
        addPoint(center.x - y, center.y - x);

        if (d < 0) {
            d += 2*x+3;
        } else {
            d += 2*(x-y)+5;
            y--;
        }
        x++;
    }
    return points;
}