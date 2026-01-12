#ifndef OBJECTS_H
#define OBJECTS_H

#include "GraphicsEngine.h"

class Line : public Object {
public:
    int dashed, lineWidth;
    Point start, end;
    Line() : dashed(0), lineWidth(1), start(Point(0, 0)), end(Point(0, 0)) {}
    void setStart(Point s) { start = s; }
    void setEnd(Point e) { end = e; }
    void setLine(Point s, Point e) { start = s; end = e; }
    void setDashed(int d) { dashed = d; }
    void setLineWidth(int w) { lineWidth = w; }

    void draw(int width, int height) override;
    void transform(TMatrix m, int d = 1) override;

    Point backup_start, backup_end;
    void backup() { backup_start = start; backup_end = end; }
    void restore() { start = backup_start; end = backup_end; }
};

class Circle : public Object {
public:
    Point center;
    int radius;
    int lineWidth;
    Circle() : center(Point(0, 0)), radius(0), lineWidth(1) {}
    void setCenter(Point c) { center = c; }
    void setRadius(int r) { radius = r; }
    void setCircle(Point c, int r) { center = c; radius = r; }
    void setLineWidth(int w) { lineWidth = w; }

    void draw(int width, int height) override;
    void transform(TMatrix m, int d = 1) override;

    Point backup_center;
    int backup_radius;
    void backup() override { backup_center = center; backup_radius = radius; }
    void restore() override { center = backup_center; radius = backup_radius; }
};

class Bezier : public Object {
public:
    std::vector<Point> control;
    int lineWidth;
    Bezier() : lineWidth(1) {}
    void addControlPoint(Point p) { control.push_back(p); }
    void setLineWidth(int w) { lineWidth = w; }

    void draw(int width, int height) override;
    void transform(TMatrix m, int d = 1) override;

    std::vector<Point> backup_control;
    void backup() override { backup_control = control; }
    void restore() override { control = backup_control; }
};

// Polygon shape that supports selection and transforms
class PolygonShape : public Object {
public:
    std::vector<Point> vertices;
    bool filled;
    int lineWidth; // reserved for potential stroked outlines
    COLORREF outlineColor;
    COLORREF fillColor;

    PolygonShape()
        : filled(false), lineWidth(1), outlineColor(RGB(0,0,255)), fillColor(RGB(100,200,255)) {}

    void setVertices(const std::vector<Point>& v) { vertices = v; }
    void setFilled(bool f) { filled = f; }
    void setOutlineColor(COLORREF c) { outlineColor = c; }
    void setFillColor(COLORREF c) { fillColor = c; }

    void draw(int width, int height) override;
    void transform(TMatrix m, int d = 1) override;

    std::vector<Point> backup_vertices;
    bool backup_filled;
    void backup() override { backup_vertices = vertices; backup_filled = filled; }
    void restore() override { vertices = backup_vertices; filled = backup_filled; }
};

Point getCenter(const std::vector<Object*>& objs);

class CircleArc : public Object {
public:
    Point center;
    int radius;
    double start, end;
    int lineWidth;
    CircleArc() : center(Point(0, 0)), radius(0), start(0), end(0), lineWidth(1) {}
    void setCenter(Point c) { center = c; }
    void setRadius(int r) { radius = r; }
    void setRange(double l, double r) { start = l; end = r; }
    void setLineWidth(int w) { lineWidth = w; }

    void draw(int width, int height) override;
    void transform(TMatrix m, int d = 1) override;

    Point backup_center;
    int backup_radius;
    double backup_start, backup_end;
    void backup() override { backup_center = center; backup_radius = radius; backup_start = start, backup_end = end; }
    void restore() override { center = backup_center; radius = backup_radius; start = backup_start, end = backup_end; }
};

class Bspline : public Object {
public:
    std::vector<Point> control;
    int degree;
    int lineWidth;
    Bspline(int d = 3) : lineWidth(1), degree(d) {}
    void addControlPoint(Point p) { control.push_back(p); }
    void setDegree(int d) { degree = d; }
    void setLineWidth(int w) { lineWidth = w; }

    void draw(int width, int height) override;
    void transform(TMatrix m, int d = 1) override;

    std::vector<Point> backup_control;
    void backup() override { backup_control = control; }
    void restore() override { control = backup_control; }
};

#endif
