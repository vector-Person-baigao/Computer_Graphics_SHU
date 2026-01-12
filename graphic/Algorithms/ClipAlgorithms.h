#ifndef CLIP_ALGORITHMS_H
#define CLIP_ALGORITHMS_H

#include "../Point.h"
#include <vector>
#include <utility>

/** 3) 直线段裁剪：Cohen–Sutherland */
bool clipLineCohenSutherland(
    Point p0, Point p1,
    int xmin, int ymin, int xmax, int ymax,
    Point& out0, Point& out1);

/** 3) 直线段裁剪：中点分割法（可选） */
bool clipLineMidpoint(
    Point p0, Point p1,
    int xmin, int ymin, int xmax, int ymax,
    Point& out0, Point& out1);

/** 4) 任意凸多边形窗口裁剪（Sutherland–Hodgman）
 * subject：被裁剪多边形（任意简单多边形）
 * clipWin：凸裁剪窗口顶点（建议 CCW）
 */
std::vector<Point> clipPolygonSuthHodg(
    const std::vector<Point>& subject,
    const std::vector<Point>& clipWin);

/** 4+) 线段对凸多边形窗口的裁剪（将线段视作2点多边形使用同一套路） */
bool clipSegmentByConvexPolygon(
    Point p0, Point p1,
    const std::vector<Point>& clipWin,
    Point& out0, Point& out1);

#endif
