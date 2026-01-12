#include "Objects.h"
#include "Algorithms/Line_Bersenham.h"
#include "Algorithms/Circle_Midpoint.h"
#include "Algorithms/Bezier_deCasteljau.h"
#include "Algorithms/Arc_Midpoint.h"
#include "Algorithms/Bspline.h"
#include "Algorithms/ScanlineAlgorithms.h"

#include <stdio.h>
#include <vector>

void Line::draw(int width, int height) {
    std::vector<Point> pt;
    if (dashed) {
        pt = dashedLine(start, end, 0xF0F0, lineWidth);
    }
    else {
        pt = thickLine(start, end, lineWidth);
    }
    points.clear();
    for (const auto& p : pt) {
        points.push_back(Pixel(p.x, p.y, color));
    }
}

void Line::transform(TMatrix m, int d) {
    start = start.transform(m, d);
    end = end.transform(m, d);
}

void Circle::draw(int width, int height) {
    std::vector<Point> pt;
    pt = circleMidpoint(center, radius);
    points.clear();
    for (const auto& p : pt) {
        points.push_back(Pixel(p.x, p.y, color));
    }
}

void Circle::transform(TMatrix m, int d) {
    center = center.transform(m, d);
    radius = static_cast<int>(radius * sqrt(m.det(d)));
}

void Bezier::draw(int width, int height) {
    std::vector<Point> pt;
    pt = deCasteljau(control);
    points.clear();
    for (const auto& p : pt) {
        points.push_back(Pixel(p.x, p.y, color));
    }
}

void Bezier::transform(TMatrix m, int d) {
    for (auto& p : control) {
        p = p.transform(m, d);
    }
}

Point getCenter(const std::vector<Object*>& objs) {
    if (objs.empty()) return Point(0, 0);
    long long sumx = 0, sumy = 0;
    for (const auto& obj : objs) {
        for (const auto& pixel : obj->points) {
            sumx += pixel.x;
            sumy += pixel.y;
        }
    }
    int totalPoints = 0;
    for (const auto& obj : objs) {
        totalPoints += static_cast<int>(obj->points.size());
    }
    return Point(static_cast<int>(sumx / totalPoints), static_cast<int>(sumy / totalPoints));
}

void CircleArc::draw(int width, int height) {
    std::vector<Point> pt;
    pt = arcMidpoint(center, radius, start, end);
    points.clear();
    for (const auto& p : pt) {
        points.push_back(Pixel(p.x, p.y, color));
    }
}

void CircleArc::transform(TMatrix m, int d) {
    center = center.transform(m, d);
    radius = static_cast<int>(radius * sqrt(m.det(d)));
    double r = atan2(m.val[1][0], m.val[0][0]);
    start += r; end += r;
}

void Bspline::draw(int width, int height) {
    std::vector<Point> pt;
    pt = drawUBS(control, degree);
    points.clear();
    for (const auto& p : pt) {
        points.push_back(Pixel(p.x, p.y, color));
    }
}

void Bspline::transform(TMatrix m, int d) {
    for (auto& p : control) {
        p = p.transform(m, d);
    }
}

void PolygonShape::draw(int width, int height) {
    points.clear();
    if (vertices.empty()) return;

    // Outline via Bresenham between consecutive vertices
    for (size_t i = 0; i < vertices.size(); ++i) {
        Point a = vertices[i];
        Point b = vertices[(i + 1) % vertices.size()];
        auto seg = lineBresenham(a, b);
        for (auto &pp : seg) points.emplace_back(pp.x, pp.y, outlineColor);
    }

    // Optional fill via scanline fill
    if (filled && vertices.size() >= 3) {
        auto fillpix = polygonScanlineFill(vertices);
        for (auto &pp : fillpix) points.emplace_back(pp.x, pp.y, fillColor);
    }
}

void PolygonShape::transform(TMatrix m, int d) {
    for (auto &v : vertices) {
        v = v.transform(m, d);
    }
}
