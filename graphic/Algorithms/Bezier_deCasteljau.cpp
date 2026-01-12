#define _USE_MATH_DEFINES
#include "Bezier_deCasteljau.h"
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// static const int BEZIER_STEPS = 1000;
static int computeSteps(const std::vector<Point>& control, double tol = 0.5) {
    double minX = control[0].x, maxX = control[0].x;
    double minY = control[0].y, maxY = control[0].y;
    for (const auto& p : control) {
        minX = std::min(minX, (double)p.x);
        maxX = std::max(maxX, (double)p.x);
        minY = std::min(minY, (double)p.y);
        maxY = std::max(maxY, (double)p.y);
    }
    double diagonal = std::sqrt((maxX-minX)*(maxX-minX) + (maxY-minY)*(maxY-minY));
    
    int steps = (int)std::ceil(diagonal / tol);
    return std::min(std::max(steps, 100), 10000);
}

std::vector<Point> deCasteljau(const std::vector<Point>& control) {
	std::vector<Point> points;

    if (control.empty()) return points;
    if (control.size() == 1) {
        points.push_back(control[0]);
        return points;
    }

    int BEZIER_STEPS = computeSteps(control);
    points.reserve(BEZIER_STEPS + 1);
    for (int si = 0; si <= BEZIER_STEPS; si++) {
        double t = static_cast<double>(si) / BEZIER_STEPS;
        std::vector<Point> temp = control;
        int n = temp.size();
        for (int r = 1; r < n; r++) {
            for (int i = 0; i < n - r; i++) {
                temp[i].x = static_cast<int>((1 - t) * temp[i].x + t * temp[i + 1].x);
                temp[i].y = static_cast<int>((1 - t) * temp[i].y + t * temp[i + 1].y);
            }
        }
        points.push_back(temp[0]);
    }

    return points;
}
