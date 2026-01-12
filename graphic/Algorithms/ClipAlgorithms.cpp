#include "ClipAlgorithms.h"
#include <cmath>
#include <algorithm>

// ================= Cohen–Sutherland =================
enum OutCode : unsigned char {
    INSIDE = 0,
    LEFT=1, RIGHT=2, BOTTOM=4, TOP=8
};
static inline unsigned char regionCode(const Point& p, int xmin, int ymin, int xmax, int ymax){
    unsigned char c = INSIDE;
    if (p.x < xmin) c |= LEFT;  else if (p.x > xmax) c |= RIGHT;
    if (p.y < ymin) c |= BOTTOM; else if (p.y > ymax) c |= TOP;
    return c;
}

bool clipLineCohenSutherland(Point p0, Point p1, int xmin, int ymin, int xmax, int ymax, Point& out0, Point& out1){
    unsigned char c0 = regionCode(p0,xmin,ymin,xmax,ymax);
    unsigned char c1 = regionCode(p1,xmin,ymin,xmax,ymax);

    while (true){
        if ((c0 | c1) == 0){ out0=p0; out1=p1; return true; }      // 全部在内
        if (c0 & c1) return false;                                 // 全部在外
        // 至少有一个端点在外：取它并求与窗口边的交点
        unsigned char out = (c0)? c0 : c1;
        double x=0, y=0;
        if (out & TOP)    { x = p0.x + (p1.x-p0.x)*(double(xmax?1:1) * (double(ymax-p0.y))/double(p1.y-p0.y)); y = ymax; } // 占位
        if (out & BOTTOM) { x = p0.x + (p1.x-p0.x)*(double(ymin-p0.y)/double(p1.y-p0.y)); y = ymin; }
        if (out & RIGHT)  { y = p0.y + (p1.y-p0.y)*(double(xmax-p0.x)/double(p1.x-p0.x)); x = xmax; }
        if (out & LEFT)   { y = p0.y + (p1.y-p0.y)*(double(xmin-p0.x)/double(p1.x-p0.x)); x = xmin; }

        if (out == c0){ p0 = Point((int)std::round(x),(int)std::round(y)); c0 = regionCode(p0,xmin,ymin,xmax,ymax); }
        else          { p1 = Point((int)std::round(x),(int)std::round(y)); c1 = regionCode(p1,xmin,ymin,xmax,ymax); }
    }
}

// ================= Midpoint subdivision =================
static inline bool insideRect(const Point& p, int xmin, int ymin, int xmax, int ymax){
    return xmin<=p.x && p.x<=xmax && ymin<=p.y && p.y<=ymax;
}

bool clipLineMidpoint(Point p0, Point p1, int xmin, int ymin, int xmax, int ymax, Point& out0, Point& out1){
    // 快速拒绝
    unsigned char c0 = regionCode(p0,xmin,ymin,xmax,ymax);
    unsigned char c1 = regionCode(p1,xmin,ymin,xmax,ymax);
    if (c0 & c1) return false;

    // 二分到像素级
    Point a=p0, b=p1;
    const int MAX_IT=32;
    for (int it=0; it<MAX_IT; ++it){
        Point m( (a.x+b.x)>>1, (a.y+b.y)>>1 );
        if (insideRect(m,xmin,ymin,xmax,ymax)){
            // 中点在内，收缩到靠内一段
            if (!insideRect(a,xmin,ymin,xmax,ymax)) a = m;
            if (!insideRect(b,xmin,ymin,xmax,ymax)) b = m;
        }else{
            // 中点在外，去掉在外的一半
            if (!insideRect(a,xmin,ymin,xmax,ymax)) a = m;
            else b = m;
        }
    }
    // 最终在容差内判断端点
    // 把原始端点朝 a,b 的“最近内点”吸附
    auto clampToRect = [&](Point p)->Point{
        p.x = std::min(std::max(p.x, xmin), xmax);
        p.y = std::min(std::max(p.y, ymin), ymax);
        return p;
    };
    Point A = clampToRect(p0), B = clampToRect(p1);
    if (!insideRect(A,xmin,ymin,xmax,ymax) && !insideRect(B,xmin,ymin,xmax,ymax)) return false;
    out0 = insideRect(A,xmin,ymin,xmax,ymax) ? A : a;
    out1 = insideRect(B,xmin,ymin,xmax,ymax) ? B : b;
    return true;
}

// ================== Sutherland–Hodgman for convex polygon ==================
static inline Point intersect(const Point& p1, const Point& p2, const Point& a, const Point& b){
    // 直线 p1p2 与 ab 的交点（参数式）
    double x1=p1.x, y1=p1.y, x2=p2.x, y2=p2.y;
    double x3=a.x, y3=a.y, x4=b.x, y4=b.y;
    double den = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4);
    if (std::abs(den) < 1e-9) return p2; // 平行：回退
    double t = ((x1-x3)*(y3-y4)-(y1-y3)*(x3-x4))/den;
    return Point((int)std::round(x1 + t*(x2-x1)), (int)std::round(y1 + t*(y2-y1)));
}

static inline int crossZ(const Point& a, const Point& b, const Point& c){
    // (b-a) x (c-a) 的 z 分量
    return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);
}

std::vector<Point> clipPolygonSuthHodg(const std::vector<Point>& subject, const std::vector<Point>& clipWin){
    if (subject.empty() || clipWin.size()<3) return {};
    // 逐边裁剪。clipWin 可能是顺时针或逆时针，这里根据面积方向自适应 inside 判定
    std::vector<Point> out = subject;

    // 计算裁剪窗口顶点方向：area>0 => CCW，area<0 => CW
    long long area2 = 0; // 两倍有向面积
    for (size_t i = 0; i < clipWin.size(); ++i) {
        const Point& a = clipWin[i];
        const Point& b = clipWin[(i + 1) % clipWin.size()];
        area2 += 1LL * a.x * b.y - 1LL * a.y * b.x;
    }
    const bool ccw = (area2 > 0);
    for (size_t i=0;i<clipWin.size();++i){
        Point A = clipWin[i];
        Point B = clipWin[(i+1)%clipWin.size()];
        std::vector<Point> in = out;
        out.clear();
        if (in.empty()) break;

        for (size_t j=0;j<in.size();++j){
            Point P = in[j];
            Point Q = in[(j+1)%in.size()];
            int cP = crossZ(A,B,P);
            int cQ = crossZ(A,B,Q);
            // CCW 窗口时，边的左侧为内；CW 时，边的右侧为内
            bool inP = ccw ? (cP >= 0) : (cP <= 0);
            bool inQ = ccw ? (cQ >= 0) : (cQ <= 0);

            if (inP && inQ){
                // P、Q 都在内：保留 Q
                out.push_back(Q);
            }else if (inP && !inQ){
                // P 在内 Q 在外：保留交点
                out.push_back(intersect(P,Q,A,B));
            }else if (!inP && inQ){
                // P 在外 Q 在内：保留交点 + Q
                out.push_back(intersect(P,Q,A,B));
                out.push_back(Q);
            }else{
                // 都在外：无事发生
            }
        }
    }
    return out;
}

bool clipSegmentByConvexPolygon(Point p0, Point p1, const std::vector<Point>& clipWin, Point& out0, Point& out1){
    std::vector<Point> seg = {p0, p1};
    auto poly = clipPolygonSuthHodg(seg, clipWin);
    if (poly.size() < 2) return false;
    // 结果可能多于2个点（线段与顶点重合等），选最远两点
    double best= -1;
    for (size_t i=0;i<poly.size();++i){
        for (size_t j=i+1;j<poly.size();++j){
            double dx = poly[i].x - poly[j].x;
            double dy = poly[i].y - poly[j].y;
            double d = dx*dx+dy*dy;
            if (d>best){ best=d; out0=poly[i]; out1=poly[j]; }
        }
    }
    return true;
}
