#include "Bspline.h"

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

double basisFunction(int i, int k, double u, const std::vector<double>& knots) {
    if (k == 1) {
        return (u >= knots[i] && u < knots[i+1]) ? 1.0 : 0.0;
    }
    
    double d1 = knots[i+k-1] - knots[i];
    double d2 = knots[i+k] - knots[i+1];
    
    double c1 = (d1 > 1e-10) ? (u - knots[i]) / d1 : 0.0;
    double c2 = (d2 > 1e-10) ? (knots[i+k] - u) / d2 : 0.0;
    
    return c1 * basisFunction(i, k-1, u, knots) + 
           c2 * basisFunction(i+1, k-1, u, knots);
}

std::vector<Point> drawUBS(const std::vector<Point>& control, int degree) {
    if (control.size() < degree + 1) return std::vector<Point>();
    
    std::vector<double> knots(control.size() + degree + 1);
    for (size_t i = 0; i < knots.size(); i++) {
        if (i < degree) knots[i] = 0;
        else if (i > control.size()) knots[i] = control.size() - degree;
        else knots[i] = i - degree;
    }
    
    return drawNUBS(control, knots, degree);
}

std::vector<Point> drawNUBS(
    const std::vector<Point>& control,
    const std::vector<double>& knots,
    int degree) {
    
    if (control.size() < degree + 1) return std::vector<Point>();
    
    std::vector<Point> points;
    int steps = computeSteps(control);
    points.reserve(steps + 1);
    
    double uStart = knots[degree];
    double uEnd = knots[control.size()];
    double uStep = (uEnd - uStart) / steps;
    
    for (int step = 0; step <= steps; step++) {
        double u = uStart + step * uStep;
        if (u > uEnd) u = uEnd;
        
        double x = 0, y = 0;
        for (size_t i = 0; i < control.size(); i++) {
            double basis = basisFunction(i, degree + 1, u, knots);
            x += control[i].x * basis;
            y += control[i].y * basis;
        }
        
        points.push_back(Point((int)std::round(x), (int)std::round(y)));
    }
    
    return points;
}