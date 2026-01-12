#define _USE_MATH_DEFINES
#include "Line_Bersenham.h"
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::vector<Point> lineBresenham(Point p1, Point p2) {
    std::vector<Point> points;
    int x1 = p1.x, y1 = p1.y;
    int x2 = p2.x, y2 = p2.y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        points.push_back(Point(x1, y1));

        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }

    return points;
}

std::vector<Point> thickLine(Point p1, Point p2, int width)
{
    std::vector<Point> points;
    std::vector<Point> baseLine = lineBresenham(p1, p2);

    if (baseLine.empty()) return points;

    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;

    if (dx == 0 && dy == 0) {
        for (int i = -width / 2; i <= width / 2; i++) {
            for (int j = -width / 2; j <= width / 2; j++) {
                if (i * i + j * j <= (width / 2) * (width / 2)) {
                    points.push_back(Point(p1.x + i, p1.y + j));
                }
            }
        }
        return points;
    }

    double length = sqrt(dx * dx + dy * dy);
    double nx = -dy / length;
    double ny = dx / length;

    for (const Point& p : baseLine)
    {
        points.push_back(p);


        for (int i = -width / 2; i <= width / 2; i++) {
            for (int j = -width / 2; j <= width / 2; j++) {
                if (sqrt(i * i + j * j) <= width / 2) {
                    points.push_back(Point(p.x + i, p.y + j));
                }
            }
        }
    }

    return points;
}

std::vector<Point> dashedLine(Point p1, Point p2, int pattern, int lineWidth) {
    std::vector<Point> allPoints = thickLine(p1, p2, lineWidth);
    std::vector<Point> dashedPoints;

    int patternLength = 8;
    int cp = lineWidth / 2 * 2 + 1;
    for (size_t i = 0; i < allPoints.size(); i++) {
        if (pattern & (1 << (i / cp % patternLength))) {
            dashedPoints.push_back(allPoints[i]);
        }
    }

    return dashedPoints;
}