#include "ScanlineAlgorithms.h"
#include "Line_Bersenham.h"
#include <queue>
#include <functional>

struct Edge {
    int yMax;
    double x;       // 当前扫描线上的交点 x
    double invSlope;// 1/k = dx/dy
};

static inline bool isHorizontal(const Point& a, const Point& b){
    return a.y == b.y;
}

std::vector<Point> polygonScanlineFill(const std::vector<Point>& polyIn) {
    std::vector<Point> poly = polyIn;
    if(poly.size() < 3) return {};

    // 去掉可能重复的末尾首顶点
    if (poly.front().x == poly.back().x && poly.front().y == poly.back().y)
        poly.pop_back();
    if (poly.size() < 3) return {};

    // 计算 y 范围
    int ymin = poly[0].y, ymax = poly[0].y;
    for (auto &p: poly){ ymin = std::min(ymin, p.y); ymax = std::max(ymax, p.y); }

    // 建立边表 ET[y]：每个 y 存放从该 y 开始活化的边
    int H = ymax - ymin + 1;
    std::vector<std::vector<Edge>> ET(H);

    auto addEdge = [&](Point a, Point b){
        if (a.y == b.y) return; // 水平边不入表（经典做法，避免双计数）
        if (a.y > b.y) std::swap(a,b);
        Edge e;
        e.yMax = b.y;
        e.x = a.x;
        e.invSlope = double(b.x - a.x) / double(b.y - a.y);
        ET[a.y - ymin].push_back(e);
    };

    // 多边形各边加入 ET
    for (size_t i=0;i<poly.size();++i){
        Point a = poly[i];
        Point b = poly[(i+1)%poly.size()];
        addEdge(a,b);
    }

    // 活动边表 AET
    std::vector<Edge> AET;
    std::vector<Point> filled;

    // 扫描
    for (int y = ymin; y <= ymax; ++y) {
        // 把当前 y 新激活的边加入 AET
        auto& bucket = ET[y - ymin];
        AET.insert(AET.end(), bucket.begin(), bucket.end());

        // 删除 yMax == y 的边（上顶点不取）
        AET.erase(std::remove_if(AET.begin(), AET.end(),
            [y](const Edge& e){ return e.yMax == y; }), AET.end());

        // 按 x 排序
        std::sort(AET.begin(), AET.end(),
                  [](const Edge& a, const Edge& b){ return a.x < b.x; });

        // 成对填充区间
        for (size_t i=0; i+1 < AET.size(); i+=2){
            int xl = (int)std::ceil(AET[i].x);
            int xr = (int)std::floor(AET[i+1].x);
            for (int x = xl; x <= xr; ++x) filled.emplace_back(x, y);
        }

        // x 前进
        for (auto& e: AET) e.x += e.invSlope;
    }
    return filled;
}

void scanlineSeedFill(
    Point seed, int xmin, int ymin, int xmax, int ymax,
    const std::function<bool(int,int)>& isFillable,
    const std::function<void(int,int)>& setPixel)
{
    if (seed.x < xmin || seed.x > xmax || seed.y < ymin || seed.y > ymax) return;
    if (!isFillable(seed.x, seed.y)) return;

    std::queue<Point> Q;
    Q.push(seed);

    auto spanFill = [&](int x, int y)->std::pair<int,int>{
        int L = x, R = x;
        // 左扩展
        while (L-1 >= xmin && isFillable(L-1, y)) --L;
        // 右扩展
        while (R+1 <= xmax && isFillable(R+1, y)) ++R;
        for (int xx=L; xx<=R; ++xx) setPixel(xx, y);
        return {L,R};
    };

    // 为了避免重复访问，用一个 lambda 简单缓存：在上层调用时，isFillable/setPixel 通常会更新一个访问标记。
    while (!Q.empty()){
        Point p = Q.front(); Q.pop();
        if (!isFillable(p.x, p.y)) continue;
        auto seg = spanFill(p.x, p.y);
        int L = seg.first, R = seg.second;

        // 上一行
        if (p.y-1 >= ymin){
            int xx = L;
            while (xx <= R){
                if (isFillable(xx, p.y-1)){
                    int st = xx;
                    while (xx<=R && isFillable(xx, p.y-1)) ++xx;
                    Q.emplace(st, p.y-1);
                } else ++xx;
            }
        }
        // 下一行
        if (p.y+1 <= ymax){
            int xx = L;
            while (xx <= R){
                if (isFillable(xx, p.y+1)){
                    int st = xx;
                    while (xx<=R && isFillable(xx, p.y+1)) ++xx;
                    Q.emplace(st, p.y+1);
                } else ++xx;
            }
        }
    }
}

std::vector<Point> polygonOutlineBresenham(const std::vector<Point>& polyIn){
    std::vector<Point> res;
    if (polyIn.size() < 2) return res;
    std::vector<Point> poly = polyIn;
    if (poly.front().x == poly.back().x && poly.front().y == poly.back().y)
        poly.pop_back();
    for (size_t i=0;i<poly.size();++i){
        auto a = poly[i];
        auto b = poly[(i+1)%poly.size()];
        auto seg = lineBresenham(a,b);
        res.insert(res.end(), seg.begin(), seg.end());
    }
    return res;
}
