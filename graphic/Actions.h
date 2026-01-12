#ifndef OBJECT_ENGINE_H
#define OBJECT_ENGINE_H

#include "Point.h"
#include "GraphicsEngine.h"
#include "Objects.h"
#include "ActionsObjects.h"
#include "Algorithms/ScanlineAlgorithms.h"
#include "Algorithms/ClipAlgorithms.h"
#include "Algorithms/Line_Bersenham.h"
#include <set>
#include <assert.h>
#include <stdio.h>
#include <windows.h>

// ============ 模式枚举：保留原有 + 新增游戏模式 ============
enum class PaintType {
    LINE,
    CIRCLE,
    BEZIER,
    SELECT,
    ARC_CIRCLE,
    BSPLINE,
    POLYGON,
    FILL_REGION,
    CLIP_SEGMENT,
    CLIP_WINDOW,
    FRUIT_NINJA     // 新增：水果忍者游戏模式
};

// ======================= 原有 Engine 定义（保持不变） =======================
class LineEngine {
public:
    Point start;
    bool drawing;
    int linewidth;
    bool dashed;
    Line* line;
    LineEngine() : start(0, 0), drawing(false), linewidth(1), dashed(false), line(nullptr) {}
    ~LineEngine() { if (line != nullptr)  delete line; }
};
void delLineEngine(GraphicsEngine& g_engine, LineEngine* engine);

class CircleEngine {
public:
    bool drawing;
    Circle* circle;
    CircleEngine() : drawing(false), circle(nullptr) {}
    ~CircleEngine() { if (circle != nullptr) delete circle; }
};
void delCircleEngine(GraphicsEngine& g_engine, CircleEngine* engine);

class BezierEngine {
public:
    bool drawing;
    bool modifying;
    int modify_chosen;
    int linewidth;
    Bezier* bezier;
    std::vector<SelPoint*> sel_control;

    BezierEngine() : drawing(false), modifying(false), modify_chosen(-1), linewidth(1),
        bezier(nullptr), sel_control(std::vector<SelPoint*>(0)) {
    }
    ~BezierEngine() {
        if (bezier != nullptr) delete bezier;
        for (const auto& i : sel_control) {
            delete i;
        }
    }
};
void delBezierEngine(GraphicsEngine& g_engine, BezierEngine* engine);

enum class MotionType { Translate, Rotate, Scale };

class SelectEngine {
public:
    bool moving; MotionType mvtype; Point start, last;
    bool selcenter, fixcenter; Point center;
    std::vector<Object*> obj;

    SelPoint* selcp;
    std::vector<SelFrame*> selobj;

    SelectEngine() : moving(false), mvtype(MotionType::Translate), start(0, 0), last(0, 0),
        selcenter(false), fixcenter(false), center(0, 0),
        obj(std::vector<Object*>(0, nullptr)),
        selcp(nullptr), selobj(std::vector<SelFrame*>(0)) {
    }
    ~SelectEngine() {
        for (const auto& i : selobj) {
            delete i;
        }
        if (selcp) {
            delete selcp;
        }
    }
};
void delSelectEngine(GraphicsEngine& g_engine, SelectEngine* engine);

class ArccEngine {
public:
    int drawing;    // 0 - center unselected, 1 - selecting first points, 2 - selecting second points
    CircleArc* arc;
    ArccEngine() : drawing(0), arc(nullptr) {}
    ~ArccEngine() { if (arc != nullptr) delete arc; }
};
void delArccEngine(GraphicsEngine& g_engine, ArccEngine* arc);

// copy Bezier
class BsplineEngine {
public:
    bool drawing;
    bool modifying;
    int modify_chosen;
    int linewidth;

    Bspline* bspline;
    std::vector<SelPoint*> sel_control;

    BsplineEngine() : drawing(false), modifying(false), modify_chosen(-1), linewidth(1),
        bspline(nullptr),
        sel_control(std::vector<SelPoint*>(0)) {
    }
    ~BsplineEngine() {
        if (bspline != nullptr) delete bspline;
        for (const auto& i : sel_control) {
            delete i;
        }
    }
};
void delBsplineEngine(GraphicsEngine& g_engine, BsplineEngine* engine);

// ======================= 新增四个功能的 Engine 定义 =======================

// 多边形绘制引擎：收集顶点，用 Bresenham 连轮廓；Shift+结束则做扫描线填充
struct PolygonEngine {
    std::vector<Point> vertices;
    Object* polyObj = nullptr;   // 用普通 Object 承载像素（避免改动 Objects.h）
    bool drawing = false;
    bool wantFill = false;
};

// 扫描线种子填充：点击种子后基于当前帧像素占用做填充
struct FillEngine {
    bool active = false;
};

// 线段裁剪：点击两点定义线段；优先使用已设置的凸窗，否则用画布矩形
struct ClipLineEngine {
    bool active = false;
    std::vector<Point> temp;
    Object* segObj = nullptr; // preview line object
};

// 凸裁剪窗口定义：左键加点，右键结束；准备好后 ClipLine 使用它
struct ClipWindowEngine {
    std::vector<Point> clipWin;
    Object* winObj = nullptr; // 画出红色窗口轮廓
    bool drawing = false;
    bool ready = false;
};

// 声明全局游戏实例
class FruitNinjaGame;
extern FruitNinjaGame g_fruitNinjaGame;

std::array<int, 4> getbound(Object* obj);   // left, right, top, bottom

int ActionsHandler(GraphicsEngine& Engine, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 1 - redraw, 0 - do nothing

#endif