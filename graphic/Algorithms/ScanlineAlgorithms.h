#ifndef SCANLINE_ALGORITHMS_H
#define SCANLINE_ALGORITHMS_H

#include "../Point.h"
#include <vector>
#include <functional>
#include <algorithm>
#include <limits>
#include <cmath>

/**
 * 1) 任意多边形的扫描线光栅化（填充像素）
 * 输入：顶点序列（建议 CCW，最后一个顶点可等于或不等于第一个）
 * 输出：填充到多边形内部的像素网格坐标
 */
std::vector<Point> polygonScanlineFill(const std::vector<Point>& poly);

/**
 * 2) 扫描线种子填充（连通区域填充）
 * 说明：给定一个“可查询像素是否可填充”的回调 isFillable(x,y)，以及一个“写入像素”的回调 setPixel(x,y)，
 * 从种子 seed 出发做扫描线填充（4-邻接）。
 * 注意：这是“算法模块”，用于你在 Actions 中抓取当前帧像素状态后完成区域填充。
 */
void scanlineSeedFill(
    Point seed,
    int xmin, int ymin, int xmax, int ymax,
    const std::function<bool(int,int)>& isFillable,
    const std::function<void(int,int)>& setPixel);

/**
 * 3) 多边形边界（轮廓）像素生成（用于“任意多边形绘制”的轮廓）
 * 实现：直接调用现有 Bresenham，把 poly 各相邻点与首尾连起来。
 */
std::vector<Point> polygonOutlineBresenham(const std::vector<Point>& poly);

#endif
