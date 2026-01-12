#ifndef POINT_H
#define POINT_H

#include <vector>
#include <cmath>
#include <windows.h>

static double eps = 1e-9;

struct TMatrix {
    double val[3][3];
    double ival[3][3];
    TMatrix();
    void init();
    double det(int d = 1);
};
TMatrix Compose(const std::vector<TMatrix>& matrices);
TMatrix Compose(TMatrix m1, TMatrix m2);

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    Point transform(TMatrix m, int d = 1) const;
};
double dis2p(const Point p1, const Point p2);

TMatrix shift(int dx, int dy);
TMatrix rotate(double theta);
TMatrix scale(double dx, double dy);

struct Pixel {
    int x, y;
    COLORREF color;
    Pixel(int x = 0, int y = 0, COLORREF color = RGB(0,0,0)) : x(x), y(y), color(color) {}
};

#endif