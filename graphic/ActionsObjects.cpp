#include "ActionsObjects.h"
#include <array>
#include <stdio.h>

void SelPoint::draw(int width, int height) {
    points.clear();
    for (int dx = -SelMarkSize; dx <= SelMarkSize; dx++) {
        points.push_back(Pixel(p.x + dx, p.y, color));
    }
    for (int dy = -SelMarkSize; dy <= SelMarkSize; dy++) {
        points.push_back(Pixel(p.x, p.y + dy, color));
    }
}

void SelPoint::transform(TMatrix m, int d) {
    p = p.transform(m, d);
}

std::array<int,4> getbound(Object* obj) {
    std::array<int,4> bound = {INT_MAX, INT_MIN, INT_MAX, INT_MIN};
        // left, right, top, bottom
    for (const auto& p : obj->points) {
        if (p.x < bound[0]) bound[0] = p.x;
        if (p.x > bound[1]) bound[1] = p.x;
        if (p.y < bound[2]) bound[2] = p.y;
        if (p.y > bound[3]) bound[3] = p.y;
    }
    std::array<int, 4> res = {
        bound[0]-SelMarkSize, bound[1]+SelMarkSize,
        bound[2]-SelMarkSize, bound[3]+SelMarkSize
    };
    return res;
}

SelFrame::SelFrame(Object* _obj): obj(_obj) {
    auto [l, r, t, b] = getbound(obj);
    left = l; right = r; top = t; bottom = b;
}

void SelFrame::draw(int width, int height) {
    points.clear();
    for (int dx = 0; dx <= SelMarkSize; dx++) {
        points.push_back(Pixel(left + dx, top, color));
        points.push_back(Pixel(right - dx, top, color));
        points.push_back(Pixel(left + dx, bottom, color));
        points.push_back(Pixel(right - dx, bottom, color));
    }
    for (int dy = 0; dy <= SelMarkSize; dy++) {
        points.push_back(Pixel(left, top + dy, color));
        points.push_back(Pixel(right, top + dy, color));
        points.push_back(Pixel(left, bottom - dy, color));
        points.push_back(Pixel(right, bottom - dy, color));
    }
}

void SelFrame::transform(TMatrix m, int d) { 
    auto [l, r, t, b] = getbound(obj);
    left = l, right = r, top = t, bottom = b;
}
