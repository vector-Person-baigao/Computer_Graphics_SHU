#include "Actions.h"
#include "fruit_ninja.h"

PaintType curPaintType = PaintType::SELECT;
void* curEngine = new SelectEngine;
int curDashed = 0;
int curLineWidth = 1;
bool ctrlPressed = 0;
bool shiftPressed = 0;


static PolygonEngine* g_polyEngine = nullptr;
static FillEngine* g_fillEngine = nullptr;
static ClipLineEngine* g_clipLineEngine = nullptr;
static ClipWindowEngine* g_clipWindowEngine = nullptr;

void delLineEngine(GraphicsEngine& g_engine, LineEngine* engine) {
    if (engine != nullptr) {
        if (engine->line != nullptr) {
            g_engine.delObject(engine->line);
        }
        delete engine;
    }
}
void delCircleEngine(GraphicsEngine& g_engine, CircleEngine* engine) {
    if (engine != nullptr) {
        if (engine->circle != nullptr) {
            g_engine.delObject(engine->circle);
        }
        delete engine;
    }
}
void delBezierEngine(GraphicsEngine& g_engine, BezierEngine* engine) {
    if (engine != nullptr) {
        if (engine->bezier != nullptr) {
            g_engine.delObject(engine->bezier);
        }
        for (auto& sel : engine->sel_control) {
            g_engine.delObject(sel);
        }
        delete engine;
    }
}
void delSelectEngine(GraphicsEngine& g_engine, SelectEngine* engine) {
    if (engine != nullptr) {
        for (auto& sel : engine->selobj) {
            g_engine.delObject(sel);
        }
        if (engine->selcp) {
            g_engine.delObject(engine->selcp);
        }
        delete engine;
    }
}
void delArccEngine(GraphicsEngine& g_engine, ArccEngine* engine) {
    if (engine != nullptr) {
        if (engine->arc != nullptr) {
            g_engine.delObject(engine->arc);
        }
        delete engine;
    }
}
// copy Bezier
void delBsplineEngine(GraphicsEngine& g_engine, BsplineEngine* engine) {
    if (engine != nullptr) {
        if (engine->bspline != nullptr) {
            g_engine.delObject(engine->bspline);
        }
        for (auto& sel : engine->sel_control) {
            g_engine.delObject(sel);
        }
        delete engine;
    }
}
// =============== 四个扩展引擎的释放函数 ===============

static void delPolygonEngine(GraphicsEngine& g, PolygonEngine* e) {
    if (!e) return;
    if (e->polyObj) {
        // Remove preview object from scene if still present
        g.delObject(e->polyObj, true);
        delete e->polyObj;
        e->polyObj = nullptr;
    }
    delete e;
}

static void delFillEngine(GraphicsEngine&, FillEngine* e) { if (e) delete e; }
static void delClipLineEngine(GraphicsEngine& g, ClipLineEngine* e) {
    if (!e) return;
    if (e->segObj) { g.delObject(e->segObj); delete e->segObj; e->segObj = nullptr; }
    delete e;
}
static void delClipWindowEngine(GraphicsEngine& g, ClipWindowEngine* e) {
    if (!e) return;
    if (e->winObj) g.delObject(e->winObj);
    delete e;
}

TMatrix Compose(TMatrix m1, TMatrix m2, TMatrix m3) { return Compose(m1, Compose(m2, m3)); }
TMatrix getTMatrix(MotionType mvtype, Point p, Point last, Point center) {
    TMatrix t;
    switch (mvtype) {
    case MotionType::Translate: {
        t = shift(p.x - last.x, p.y - last.y);
        break;
    }
    case MotionType::Rotate: {
        double angle1 = atan2(last.y - center.y, last.x - center.x);
        double angle2 = atan2(p.y - center.y, p.x - center.x);
        t = Compose(shift(-center.x, -center.y), rotate(angle2 - angle1), shift(center.x, center.y));
        break;
    }
    case MotionType::Scale: {
        double scalex = static_cast<double>(p.x - center.x) / (last.x - center.x);
        double scaley = static_cast<double>(p.y - center.y) / (last.y - center.y);
        t = Compose(shift(-center.x, -center.y), scale(scalex, scaley), shift(center.x, center.y));
        break;
    }
    }
    return t;
}

// ========================= 右键按下 =========================
int RMouseDown(GraphicsEngine& g, Point p) {
    if (curPaintType == PaintType::FRUIT_NINJA) {
        g_fruitNinjaGame.handleMouseDown(p);
        return 1;
    }
    switch (curPaintType) {
    case PaintType::LINE: break;
    case PaintType::CIRCLE: break;
    case PaintType::BEZIER: break;

        // ------- 选择模式逻辑（保持不变） -------
    case PaintType::SELECT: {
        SelectEngine* engine = nullptr;
        if (curEngine == nullptr) {
            engine = new SelectEngine;
            curEngine = engine;
        }
        else {
            engine = (SelectEngine*)curEngine;
        }
        if (engine->selcenter) {
            engine->center = p;
            if (!engine->selcp) {
                engine->selcp = new SelPoint;
                g.pushObject(engine->selcp);
            }
            engine->selcp->p = p; engine->selcp->draw(0, 0);
            engine->selcenter = false;
            printf("Select Mode: Center Set at (%d %d). Exit Center Setting Mode\n", p.x, p.y);
            break;
        }
        else if (!engine->moving) {
            Object* selobj = nullptr;
            std::set<Object*> s;
            for (const auto& obj : engine->selobj) s.insert(obj);
            if (engine->selcp)
                s.insert(engine->selcp);
            for (Object* obj = g.objhead; obj != nullptr; obj = obj->nxt) {
                if (obj->select(p) && s.find(obj) == s.end()) {
                    selobj = obj; break;
                }
            }
            if (selobj == nullptr) break;
            bool chosen = 0;
            std::vector<Object*> objn;
            for (auto& obj : engine->obj) {
                if (obj == selobj) chosen = 1;
                else objn.push_back(obj);
            }
            std::vector<SelFrame*> selobjn;
            for (auto& i : engine->selobj) {
                if (i->obj != selobj) selobjn.push_back(i);
            }
            if (!chosen) {
                objn.push_back(selobj);

                SelFrame* frame = new SelFrame(selobj);
                frame->draw(0, 0);
                g.pushObject(frame);
                selobjn.push_back(frame);
            }
            else {
                for (auto& sel : engine->selobj) {
                    if (sel->obj == selobj) {
                        g.delObject(sel);
                        delete sel;
                    }
                }
            }
            engine->obj.clear(); engine->obj = objn;
            engine->selobj.clear(); engine->selobj = selobjn;
            if (!engine->fixcenter) {
                engine->center = getCenter(engine->obj);
                if (engine->selobj.size()) {
                    if (engine->selcp == nullptr) {
                        engine->selcp = new SelPoint(engine->center);
                        g.pushObject(engine->selcp);
                    }
                    engine->selcp->p = engine->center; engine->selcp->draw(0, 0);
                }
                else {
                    g.delObject(engine->selcp);
                    delete engine->selcp;
                    engine->selcp = nullptr;
                }
            }
            if (chosen) printf("Select Mode: Object Unselected\n");
            else printf("Select Mode: Object Selected\n");
            break;
        }
    } break;

                          // ------- 多边形模式右键结束 & 是否填充 -------
    case PaintType::POLYGON: {
        if (!g_polyEngine) break;
        if (!g_polyEngine->vertices.empty()) {
            // Shift+右键 -> 填充；否则只画轮廓
            g_polyEngine->wantFill = (GetKeyState(VK_SHIFT) < 0);
            // 生成最终多边形对象并加入场景（不删除已完成的上一个多边形）
            auto* poly = new PolygonShape();
            poly->setVertices(g_polyEngine->vertices);
            poly->setFilled(g_polyEngine->wantFill);
            poly->setOutlineColor(RGB(0, 0, 255));
            poly->setFillColor(RGB(100, 200, 255));
            poly->draw(0, 0);
            g.pushObject(poly);
            // 清除用于预览的临时多边形边对象（增量显示）
            if (g_polyEngine->polyObj) {
                g.delObject(g_polyEngine->polyObj);
                delete g_polyEngine->polyObj;
                g_polyEngine->polyObj = nullptr;
            }
            g.Flush();
            // 完成一个多边形后，重置状态以便开始下一个
            g_polyEngine->drawing = false;
            g_polyEngine->vertices.clear();
            g_polyEngine->wantFill = false;
            g_polyEngine->polyObj = nullptr;
            printf("Polygon: %s\n", (GetKeyState(VK_SHIFT) < 0) ? "filled" : "outlined");
        }
        break;
    }

                           // ------- 裁剪窗口右键结束 -------
    case PaintType::CLIP_WINDOW: {
        if (!g_clipWindowEngine) break;
        g_clipWindowEngine->drawing = false;
        g_clipWindowEngine->ready = (g_clipWindowEngine->clipWin.size() >= 3);
        if (g_clipWindowEngine->winObj) { g.delObject(g_clipWindowEngine->winObj); g_clipWindowEngine->winObj = nullptr; }
        if (g_clipWindowEngine->ready) {
            g_clipWindowEngine->winObj = new Object();
            for (size_t i = 0; i < g_clipWindowEngine->clipWin.size(); ++i) {
                Point a = g_clipWindowEngine->clipWin[i];
                Point b = g_clipWindowEngine->clipWin[(i + 1) % g_clipWindowEngine->clipWin.size()];
                auto seg = lineBresenham(a, b);
                for (auto& pp : seg) g_clipWindowEngine->winObj->points.emplace_back(pp.x, pp.y, RGB(255, 0, 0));
            }
            g.pushObject(g_clipWindowEngine->winObj);
            g.Flush();
            printf("Clip Window: set (convex)\n");
        }
        else {
            printf("Clip Window: need >= 3 vertices\n");
        }
        break;
    }

    case PaintType::BSPLINE: break;
    case PaintType::ARC_CIRCLE: break;
    case PaintType::FILL_REGION: break;
    case PaintType::CLIP_SEGMENT: break;
    }
    return 1;
}

//int RMouseUp(GraphicsEngine& g, Point p) { return 0; }

// ========================= 左键按下 =========================
int LmouseDown(GraphicsEngine& g, Point p) {
    if (curPaintType == PaintType::FRUIT_NINJA) {
        g_fruitNinjaGame.handleMouseDown(p);
        return 1;
    }
    switch (curPaintType) {
        // ========== 直线 ==========
    case PaintType::LINE: {
        if (curEngine == nullptr) {
            LineEngine* engine = new LineEngine;
            curEngine = engine;

            engine->start = p;
            engine->linewidth = curLineWidth;
            engine->dashed = curDashed;
            engine->line = new Line;
            g.pushObject(engine->line);
            engine->line->setLineWidth(engine->linewidth);
            engine->line->setDashed(engine->dashed ? 1 : 0);
            engine->line->setLine(p, p);
            engine->line->draw(0, 0);
            engine->drawing = true;
            printf("Line Mode: Start Drawing Line\n");
        }
        else {
            LineEngine* engine = (LineEngine*)curEngine;

            engine->line->setLine(engine->start, p);
            engine->line->draw(0, 0);
            engine->line = nullptr;
            printf("Line Mode: Finish Drawing Line\n");

            delLineEngine(g, engine);
            curEngine = nullptr;
        }
        break;
    }

                        // ========== 圆 ==========
    case PaintType::CIRCLE: {
        if (curEngine == nullptr) {
            CircleEngine* engine = new CircleEngine;
            curEngine = engine;

            engine->circle = new Circle;
            g.pushObject(engine->circle);
            engine->circle->setLineWidth(1);
            engine->circle->setCenter(p);
            engine->circle->setRadius(0);
            engine->circle->draw(0, 0);
            engine->drawing = true;
            printf("Circle Mode: Start Drawing Circle\n");
        }
        else {
            CircleEngine* engine = (CircleEngine*)curEngine;

            int radius = static_cast<int>(sqrt(dis2p(p, engine->circle->center)));
            engine->circle->setCircle(engine->circle->center, radius);
            engine->circle->draw(0, 0);
            engine->circle = nullptr;
            printf("Circle Mode: Finish Drawing Circle\n");

            delCircleEngine(g, engine);
            curEngine = nullptr;
        }
        break;
    }

                          // ========== Bezier ==========
    case PaintType::BEZIER: {
        auto add_control = [&](BezierEngine* engine, Point p) {
            engine->bezier->addControlPoint(p);
            engine->sel_control.push_back(new SelPoint(p));
            engine->sel_control.back()->draw(0, 0);
            g.pushObject(engine->sel_control.back());
            };
        auto pop_control = [&](BezierEngine* engine) {
            engine->bezier->control.pop_back();
            g.delObject(engine->sel_control.back());
            delete engine->sel_control.back();
            engine->sel_control.pop_back();
            };
        if (curEngine == nullptr) {
            BezierEngine* engine = new BezierEngine;
            curEngine = engine;

            engine->bezier = new Bezier;
            g.pushObject(engine->bezier);
            engine->bezier->setLineWidth(1);

            add_control(engine, p);
            add_control(engine, p);

            engine->bezier->draw(0, 0);
            engine->drawing = true;
            printf("Bezier Mode: Start Drawing Bezier Curve\n");
        }
        else {
            BezierEngine* engine = (BezierEngine*)curEngine;

            if (engine->drawing) {
                auto& control = engine->bezier->control;
                Point lst = control[control.size() - 2];
                int dis = dis2p(lst, p);
                if (dis <= pow(sel_threshold, 2)) {
                    pop_control(engine);
                    engine->drawing = false;
                    engine->modifying = true; engine->modify_chosen = -1;
                    printf("Bezier Mode: Modifying Bezier Curve\n");
                }
                else {
                    control.back() = p;
                    add_control(engine, p);
                    engine->bezier->draw(0, 0);
                    printf("Bezier Mode: Added Control Point (%d %d)\n", p.x, p.y);
                }
            }
            else if (engine->modifying) {
                if (engine->modify_chosen != -1) {
                    engine->bezier->control[engine->modify_chosen] = p;
                    engine->bezier->draw(0, 0);
                    engine->sel_control[engine->modify_chosen]->p = p;
                    engine->sel_control[engine->modify_chosen]->draw(0, 0);
                    printf("Bezier Mode: Moved Control Point %d to (%d %d)\n", engine->modify_chosen, p.x, p.y);
                    engine->modify_chosen = -1;
                }
                else {
                    int idx = 0;
                    for (const auto& i : engine->bezier->control) {
                        if (dis2p(p, i) <= pow(sel_threshold, 2)) break;
                        ++idx;
                    }
                    if (idx < engine->bezier->control.size()) {
                        engine->modify_chosen = idx;
                        printf("Bezier Mode: Chose Control Point %d for Moving\n", idx);
                    }
                    else if (!engine->bezier->select(p)) {
                        engine->modifying = false;
                        engine->bezier = nullptr;
                        printf("Bezier Mode: Finish Drawing Bezier Curve\n");

                        delBezierEngine(g, engine);
                        curEngine = nullptr;
                    }
                }
            }
        }
        break;
    }

                          // ========== 选择 ==========
    case PaintType::SELECT: {
        if (curEngine != nullptr) {
            SelectEngine* engine = (SelectEngine*)curEngine;
            if (engine->selcenter) {
                printf("Select Mode: In Center Setting Mode. Transform failed\n");
                break;
            }
            if (!engine->moving) {
                engine->moving = true;
                engine->start = engine->last = p;
                for (auto& obj : engine->obj) {
                    obj->backup();
                }
                printf("Select Mode: Start Transforming\n");
            }
            else {
                engine->moving = false;
                TMatrix t = getTMatrix(engine->mvtype, p, engine->start, engine->center);
                for (auto& obj : engine->obj) {
                    obj->restore();
                    obj->transform(t);
                    obj->draw(0, 0);
                }
                for (auto& selobj : engine->selobj) {
                    selobj->transform(t);
                    selobj->draw(0, 0);
                }
                if (!engine->fixcenter) {
                    engine->center = engine->center.transform(t);
                    if (engine->selcp) {
                        engine->selcp->p = engine->center; engine->selcp->draw(0, 0);
                    }
                }
                printf("Select Mode: Stop Transforming\n");
                engine->start = engine->last = Point(0, 0);
            }
        }
        break;
    }

                          // ========== 圆弧 ==========
    case PaintType::ARC_CIRCLE: {
        if (curEngine == nullptr) {
            ArccEngine* engine = new ArccEngine;
            curEngine = engine;

            engine->arc = new CircleArc;
            g.pushObject(engine->arc);
            engine->arc->setLineWidth(1);
            engine->arc->setCenter(p);
            engine->arc->setRadius(0);
            engine->arc->setRange(0, 0);
            engine->arc->draw(0, 0);
            engine->drawing = 1;
            printf("Arc(Circle) Mode: Start Drawing Arc\n");
        }
        else {
            ArccEngine* engine = (ArccEngine*)curEngine;

            if (engine->drawing == 1) {
                int radius = static_cast<int>(sqrt(dis2p(p, engine->arc->center)));
                engine->arc->setRadius(radius);

                double start = atan2(p.y - engine->arc->center.y, p.x - engine->arc->center.x);
                engine->arc->setRange(start, start - eps);
                engine->arc->draw(0, 0);

                engine->drawing = 2;
            }
            else {
                double end = atan2(p.y - engine->arc->center.y, p.x - engine->arc->center.x);
                engine->arc->setRange(engine->arc->start, end);
                engine->arc->draw(0, 0);

                engine->arc = nullptr;
                printf("Arc(Circle) Mode: Finish Drawing Arc\n");

                delArccEngine(g, engine);
                curEngine = nullptr;
            }
        }
        break;
    }

                              // ========== B样条 ==========
    case PaintType::BSPLINE: {
        auto add_control = [&](BsplineEngine* engine, Point p) {
            engine->bspline->addControlPoint(p);
            engine->sel_control.push_back(new SelPoint(p));
            engine->sel_control.back()->draw(0, 0);
            g.pushObject(engine->sel_control.back());
            };
        auto pop_control = [&](BsplineEngine* engine) {
            engine->bspline->control.pop_back();
            g.delObject(engine->sel_control.back());
            delete engine->sel_control.back();
            engine->sel_control.pop_back();
            };
        if (curEngine == nullptr) {
            BsplineEngine* engine = new BsplineEngine;
            curEngine = engine;

            engine->bspline = new Bspline;
            g.pushObject(engine->bspline);
            engine->bspline->setLineWidth(1);

            add_control(engine, p);
            add_control(engine, p);

            engine->bspline->draw(0, 0);
            engine->drawing = true;
            printf("Bspline Mode: Start Drawing Bspline Curve\n");
        }
        else {
            BsplineEngine* engine = (BsplineEngine*)curEngine;

            if (engine->drawing) {
                auto& control = engine->bspline->control;
                Point lst = control[control.size() - 2];
                int dis = dis2p(lst, p);
                if (dis <= pow(sel_threshold, 2)) {
                    pop_control(engine);
                    engine->drawing = false;
                    engine->modifying = true; engine->modify_chosen = -1;
                    printf("Bspline Mode: Modifying Bspline Curve\n");
                }
                else {
                    control.back() = p;
                    add_control(engine, p);
                    engine->bspline->draw(0, 0);
                    printf("Bspline Mode: Added Control Point (%d %d)\n", p.x, p.y);
                }
            }
            else if (engine->modifying) {
                if (engine->modify_chosen != -1) {
                    engine->bspline->control[engine->modify_chosen] = p;
                    engine->bspline->draw(0, 0);
                    engine->sel_control[engine->modify_chosen]->p = p;
                    engine->sel_control[engine->modify_chosen]->draw(0, 0);
                    printf("Bspline Mode: Moved Control Point %d to (%d %d)\n", engine->modify_chosen, p.x, p.y);
                    engine->modify_chosen = -1;
                }
                else {
                    int idx = 0;
                    for (const auto& i : engine->bspline->control) {
                        if (dis2p(p, i) <= pow(sel_threshold, 2)) break;
                        ++idx;
                    }
                    if (idx < engine->bspline->control.size()) {
                        engine->modify_chosen = idx;
                        printf("Bspline Mode: Chose Control Point %d for Moving\n", idx);
                    }
                    else if (!engine->bspline->select(p)) {
                        engine->modifying = false;
                        engine->bspline = nullptr;
                        printf("Bspline Mode: Finish Drawing Bespline Curve\n");

                        delBsplineEngine(g, engine);
                        curEngine = nullptr;
                    }
                }
            }
        }
        break;
    }

                           // ========== 多边形绘制 ==========
    case PaintType::POLYGON: {
        if (!g_polyEngine) g_polyEngine = new PolygonEngine();
        g_polyEngine->drawing = true;
        g_polyEngine->vertices.emplace_back(p);
        // 增量显示：创建/更新预览多边形边
        if (!g_polyEngine->polyObj) {
            g_polyEngine->polyObj = new Object();
            g_polyEngine->polyObj->setColor(0, 0, 255);
            g.pushObject(g_polyEngine->polyObj);
        }
        if (g_polyEngine->vertices.size() >= 2) {
            auto* preview = g_polyEngine->polyObj;
            preview->points.clear();
            for (size_t i = 0; i + 1 < g_polyEngine->vertices.size(); ++i) {
                Point a = g_polyEngine->vertices[i];
                Point b = g_polyEngine->vertices[i + 1];
                auto seg = lineBresenham(a, b);
                for (auto& pp : seg) preview->points.emplace_back(pp.x, pp.y, RGB(0, 0, 255));
            }
        }

        // 临时显示当前最后一条边（可选：此处简单到右键结束再统一绘制）
        printf("Polygon: add vertex (%d,%d)\n", p.x, p.y);
        g.renderBuffer();
        return 1;
    }

                           // ========== 区域填充（扫描线种子） ==========
    case PaintType::FILL_REGION: {
        if (!g_fillEngine) { g_fillEngine = new FillEngine(); g_fillEngine->active = true; }
        if (!g_fillEngine->active) return 0;

        int w = g.GetWidth(), h = g.GetHeight();
        // 建立占用位图：把当前所有对象像素标为1
        std::vector<std::vector<unsigned char>> occ(h, std::vector<unsigned char>(w, 0));
        for (Object* obj = g.objhead; obj != nullptr; obj = obj->nxt) {
            for (auto& px : obj->points) {
                if (0 <= px.x && px.x < w && 0 <= px.y && px.y < h) occ[px.y][px.x] = 1;
            }
        }
        auto isFillable = [&](int xx, int yy)->bool {
            if (xx < 0 || yy < 0 || xx >= w || yy >= h) return false;
            return occ[yy][xx] == 0;
            };
        auto setPixel = [&](int xx, int yy) {
            occ[yy][xx] = 2; // 标记为已填充
            };

        scanlineSeedFill(p, 0, 0, w - 1, h - 1, isFillable, setPixel);

        // 生成填充对象并推入场景
        Object* fillObj = new Object();
        COLORREF c = RGB(100, 200, 255);
        for (int yy = 0; yy < h; ++yy)
            for (int xx = 0; xx < w; ++xx)
                if (occ[yy][xx] == 2) fillObj->points.emplace_back(xx, yy, c);

        g.pushObject(fillObj);
        g.Flush();
        printf("Fill: done from seed (%d,%d)\n", p.x, p.y);
        return 1;
    }

                               // ========== 直线段裁剪（Cohen–Sutherland） ==========
    case PaintType::CLIP_SEGMENT: {
        if (!g_clipLineEngine) { g_clipLineEngine = new ClipLineEngine(); g_clipLineEngine->active = true; }
        g_clipLineEngine->temp.emplace_back(p);
        if (g_clipLineEngine->temp.size() == 1) {
            // Create preview object on first click
            if (!g_clipLineEngine->segObj) {
                g_clipLineEngine->segObj = new Object();
                g_clipLineEngine->segObj->setColor(255, 0, 0);
                g.pushObject(g_clipLineEngine->segObj);
            }
            g.renderBuffer();
        }
        if (g_clipLineEngine->temp.size() == 2) {
            Point a = g_clipLineEngine->temp[0];
            Point b = g_clipLineEngine->temp[1];
            Point o0, o1;
            bool ok = false;

            // 若已设置凸窗，优先用凸窗裁剪；否则画布矩形 Cohen–Sutherland
            if (g_clipWindowEngine && g_clipWindowEngine->ready && g_clipWindowEngine->clipWin.size() >= 3) {
                ok = clipSegmentByConvexPolygon(a, b, g_clipWindowEngine->clipWin, o0, o1);
            }
            else {
                ok = clipLineCohenSutherland(a, b, 0, 0, g.GetWidth() - 1, g.GetHeight() - 1, o0, o1);
            }

            if (ok) {
                Line* line = new Line();
                line->setColor(255, 0, 0);
                line->setLine(o0, o1);
                line->draw(0, 0);
                g.pushObject(line);
            }
            else {
                printf("Clip: segment outside\n");
            }
            // Remove preview object after finalizing
            if (g_clipLineEngine->segObj) { g.delObject(g_clipLineEngine->segObj); delete g_clipLineEngine->segObj; g_clipLineEngine->segObj = nullptr; }
            g.Flush();
            g_clipLineEngine->temp.clear();
        }
        return 1;
    }

                                // ========== 裁剪窗口设置 ==========
    case PaintType::CLIP_WINDOW: {
        if (!g_clipWindowEngine) g_clipWindowEngine = new ClipWindowEngine();
        g_clipWindowEngine->drawing = true;
        g_clipWindowEngine->clipWin.emplace_back(p);
        // Incremental preview of window edges after each click
        if (!g_clipWindowEngine->winObj) {
            g_clipWindowEngine->winObj = new Object();
            g_clipWindowEngine->winObj->setColor(255, 0, 0);
            g.pushObject(g_clipWindowEngine->winObj);
        }
        auto* preview = g_clipWindowEngine->winObj;
        preview->points.clear();
        if (g_clipWindowEngine->clipWin.size() >= 2) {
            for (size_t i = 0; i + 1 < g_clipWindowEngine->clipWin.size(); ++i) {
                Point a = g_clipWindowEngine->clipWin[i];
                Point b = g_clipWindowEngine->clipWin[i + 1];
                auto seg = lineBresenham(a, b);
                for (auto& pp : seg) preview->points.emplace_back(pp.x, pp.y, RGB(255, 0, 0));
            }
        }
        g.renderBuffer();
        printf("Clip Window: add vertex (%d,%d)\n", p.x, p.y);
        return 1;
    }
    }
    return 1;
}

//int LmouseUp(GraphicsEngine& g, Point p) { return 0; }
// 添加鼠标释放处理
int LmouseUp(GraphicsEngine& g, Point p) {
    if (curPaintType == PaintType::FRUIT_NINJA) {
        g_fruitNinjaGame.handleMouseUp(p);
        return 1;
    }
    return 0;
}

int RMouseUp(GraphicsEngine& g, Point p) {
    if (curPaintType == PaintType::FRUIT_NINJA) {
        g_fruitNinjaGame.handleMouseUp(p);
        return 1;
    }
    return 0;
}

// ========================= 鼠标移动 =========================
int mouseMove(GraphicsEngine& g, Point p) {
    if (curPaintType == PaintType::FRUIT_NINJA) {
        g_fruitNinjaGame.handleMouseMove(p);
        return 1;
    }
    if (curEngine == nullptr) {

        switch (curPaintType) {
        case PaintType::POLYGON: {
            // Rubber-band preview: show last edge to cursor
            if (g_polyEngine && g_polyEngine->drawing && !g_polyEngine->vertices.empty()) {
                if (!g_polyEngine->polyObj) {
                    g_polyEngine->polyObj = new Object();
                    g_polyEngine->polyObj->setColor(0, 0, 255);
                    g.pushObject(g_polyEngine->polyObj);
                }
                auto* preview = g_polyEngine->polyObj;
                preview->points.clear();
                // Draw already-fixed edges
                if (g_polyEngine->vertices.size() >= 2) {
                    for (size_t i = 0; i + 1 < g_polyEngine->vertices.size(); ++i) {
                        Point a = g_polyEngine->vertices[i];
                        Point b = g_polyEngine->vertices[i + 1];
                        auto seg = lineBresenham(a, b);
                        for (auto& pp : seg) preview->points.emplace_back(pp.x, pp.y, RGB(0, 0, 255));
                    }
                }
                // Add rubber-band edge from last vertex to cursor
                Point last = g_polyEngine->vertices.back();
                auto rseg = lineBresenham(last, p);
                for (auto& pp : rseg) preview->points.emplace_back(pp.x, pp.y, RGB(0, 0, 255));
                g.renderBuffer();
                return 1;
            }
            return 0;
        }
        case PaintType::CLIP_SEGMENT: {
            // Rubber-band preview for line clipping: show segment from first point to cursor
            if (g_clipLineEngine && g_clipLineEngine->active && g_clipLineEngine->temp.size() == 1) {
                if (!g_clipLineEngine->segObj) {
                    g_clipLineEngine->segObj = new Object();
                    g_clipLineEngine->segObj->setColor(255, 0, 0);
                    g.pushObject(g_clipLineEngine->segObj);
                }
                auto* preview = g_clipLineEngine->segObj;
                preview->points.clear();
                Point a = g_clipLineEngine->temp[0];
                auto seg = lineBresenham(a, p);
                for (auto& pp : seg) preview->points.emplace_back(pp.x, pp.y, RGB(255, 0, 0));
                g.renderBuffer();
                return 1;
            }
            return 0;
        }
        case PaintType::CLIP_WINDOW: {
            // Rubber-band preview for convex clip window
            if (g_clipWindowEngine && g_clipWindowEngine->drawing && !g_clipWindowEngine->clipWin.empty()) {
                if (!g_clipWindowEngine->winObj) {
                    g_clipWindowEngine->winObj = new Object();
                    g_clipWindowEngine->winObj->setColor(255, 0, 0);
                    g.pushObject(g_clipWindowEngine->winObj);
                }
                auto* preview = g_clipWindowEngine->winObj;
                preview->points.clear();
                // Draw fixed edges
                if (g_clipWindowEngine->clipWin.size() >= 2) {
                    for (size_t i = 0; i + 1 < g_clipWindowEngine->clipWin.size(); ++i) {
                        Point a = g_clipWindowEngine->clipWin[i];
                        Point b = g_clipWindowEngine->clipWin[i + 1];
                        auto seg = lineBresenham(a, b);
                        for (auto& pp : seg) preview->points.emplace_back(pp.x, pp.y, RGB(255, 0, 0));
                    }
                }
                // Rubber-band from last vertex to cursor
                Point last = g_clipWindowEngine->clipWin.back();
                auto rseg = lineBresenham(last, p);
                for (auto& pp : rseg) preview->points.emplace_back(pp.x, pp.y, RGB(255, 0, 0));
                g.renderBuffer();
                return 1;
            }
            return 0;
        }
        case PaintType::FILL_REGION:
            return 0;
        default: break;
        }
        return 0;
    }

    switch (curPaintType) {
    case PaintType::LINE: {
        LineEngine* engine = (LineEngine*)curEngine;
        if (engine->drawing) {
            engine->line->setLine(engine->start, p);
            engine->line->draw(0, 0);
            return 1;
        }
        break;
    }

    case PaintType::CIRCLE: {
        CircleEngine* engine = (CircleEngine*)curEngine;
        if (engine->drawing) {
            int radius = static_cast<int>(sqrt(dis2p(p, engine->circle->center)));
            engine->circle->setCircle(engine->circle->center, radius);
            engine->circle->draw(0, 0);
            return 1;
        }
        break;
    }

    case PaintType::BEZIER: {
        BezierEngine* engine = (BezierEngine*)curEngine;
        if (engine->drawing) {
            engine->bezier->control.back() = p;
            engine->bezier->draw(0, 0);
            engine->sel_control.back()->p = p; engine->sel_control.back()->draw(0, 0);
            return 1;
        }
        else if (engine->modifying) {
            if (engine->modify_chosen != -1) {
                engine->bezier->control[engine->modify_chosen] = p;
                engine->bezier->draw(0, 0);
                engine->sel_control[engine->modify_chosen]->p = p; engine->sel_control[engine->modify_chosen]->draw(0, 0);
                return 1;
            }
        }
        break;
    }

    case PaintType::SELECT: {
        SelectEngine* engine = (SelectEngine*)curEngine;
        if (engine->moving) {
            TMatrix t = getTMatrix(engine->mvtype, p, engine->start, engine->center);
            for (auto& obj : engine->obj) {
                obj->restore();
                obj->transform(t);
                obj->draw(0, 0);
            }
            engine->last = p;
            for (auto& selobj : engine->selobj) {
                selobj->transform(t);
                selobj->draw(0, 0);
            }
            return 1;
        }
        break;
    }

    case PaintType::ARC_CIRCLE: {
        ArccEngine* engine = (ArccEngine*)curEngine;
        if (engine->drawing == 2) {
            double end = atan2(p.y - engine->arc->center.y, p.x - engine->arc->center.x);
            if (fabs(end - engine->arc->start) < eps) end = engine->arc->start - eps;
            engine->arc->setRange(engine->arc->start, end);
            engine->arc->draw(0, 0);
            return 1;
        }
        break;
    }

    case PaintType::BSPLINE: {
        BsplineEngine* engine = (BsplineEngine*)curEngine;
        if (engine->drawing) {
            engine->bspline->control.back() = p;
            engine->bspline->draw(0, 0);
            engine->sel_control.back()->p = p; engine->sel_control.back()->draw(0, 0);
            return 1;
        }
        else if (engine->modifying) {
            if (engine->modify_chosen != -1) {
                engine->bspline->control[engine->modify_chosen] = p;
                engine->bspline->draw(0, 0);
                engine->sel_control[engine->modify_chosen]->p = p; engine->sel_control[engine->modify_chosen]->draw(0, 0);
                return 1;
            }
        }
        break;
    }


    case PaintType::POLYGON:
    case PaintType::FILL_REGION:
    case PaintType::CLIP_SEGMENT:
    case PaintType::CLIP_WINDOW:
        break;
    }
    return 0;
}

// ========================= 中断处理 =========================
void Interrupted(GraphicsEngine& g) {
    if (curEngine != nullptr) {
        switch (curPaintType) {
        case PaintType::LINE: {
            LineEngine* engine = (LineEngine*)curEngine;
            delLineEngine(g, engine);
            curEngine = nullptr;
            printf("Line Mode: Drawing Interrupted\n");
            break;
        }
        case PaintType::CIRCLE: {
            CircleEngine* engine = (CircleEngine*)curEngine;
            delCircleEngine(g, engine);
            curEngine = nullptr;
            printf("Circle Mode: Drawing Interrupted\n");
            break;
        }
        case PaintType::BEZIER: {
            BezierEngine* engine = (BezierEngine*)curEngine;
            delBezierEngine(g, engine);
            curEngine = nullptr;
            printf("Bezier Mode: Drawing Interrupted\n");
            break;
        }
        case PaintType::SELECT: {
            SelectEngine* engine = (SelectEngine*)curEngine;
            if (engine->moving) {
                engine->moving = false;
                for (auto& obj : engine->obj) {
                    obj->restore();
                    obj->draw(0, 0);
                }
                engine->start = engine->last = Point(0, 0);
            }
            printf("Select Mode: Interrupted\n");
            delSelectEngine(g, engine);
            curEngine = nullptr;
            break;
        }
        case PaintType::ARC_CIRCLE: {
            ArccEngine* engine = (ArccEngine*)curEngine;
            delArccEngine(g, engine);
            curEngine = nullptr;
            printf("Arc(Circle) Mode: Drawing Interrupted\n");
            break;
        }
        case PaintType::BSPLINE: {
            BsplineEngine* engine = (BsplineEngine*)curEngine;
            delBsplineEngine(g, engine);
            curEngine = nullptr;
            printf("Bspline Mode: Drawing Interrupted\n");
            break;
        }
                               // 新模式使用独立全局指针，不走 curEngine
        case PaintType::POLYGON:
        case PaintType::FILL_REGION:
        case PaintType::CLIP_SEGMENT:
        case PaintType::CLIP_WINDOW:
            break;
        }
    }

    // 同时清掉新模式的临时状态
    delPolygonEngine(g, g_polyEngine);       g_polyEngine = nullptr;
    delFillEngine(g, g_fillEngine);          g_fillEngine = nullptr;
    delClipLineEngine(g, g_clipLineEngine);  g_clipLineEngine = nullptr;
    delClipWindowEngine(g, g_clipWindowEngine); g_clipWindowEngine = nullptr;
}

// ========================= 模式切换 =========================
void switch_mode(GraphicsEngine& g, PaintType mode, bool flush = false) {
    if (curPaintType == mode && !flush) return;

    if (curEngine != nullptr) {
        switch (curPaintType) {
        case PaintType::LINE: {
            LineEngine* engine = (LineEngine*)curEngine;
            delLineEngine(g, engine);
            break;
        }
        case PaintType::CIRCLE: {
            CircleEngine* engine = (CircleEngine*)curEngine;
            delCircleEngine(g, engine);
            break;
        }
        case PaintType::SELECT: {
            SelectEngine* engine = (SelectEngine*)curEngine;
            delSelectEngine(g, engine);
            break;
        }
        case PaintType::BEZIER: {
            BezierEngine* engine = (BezierEngine*)curEngine;
            delBezierEngine(g, engine);
            break;
        }
        case PaintType::ARC_CIRCLE: {
            ArccEngine* engine = (ArccEngine*)curEngine;
            delArccEngine(g, engine);
            break;
        }
        case PaintType::BSPLINE: {
            BsplineEngine* engine = (BsplineEngine*)curEngine;
            delBsplineEngine(g, engine);
            break;
        }
                               // 新模式不占用 curEngine
        case PaintType::POLYGON:
        case PaintType::FILL_REGION:
        case PaintType::CLIP_SEGMENT:
        case PaintType::CLIP_WINDOW:
            break;
        }
        curEngine = nullptr;
    }

    // 离开旧模式时，清理新模式的资源
    if (curPaintType == PaintType::POLYGON) { delPolygonEngine(g, g_polyEngine); g_polyEngine = nullptr; }
    if (curPaintType == PaintType::FILL_REGION) { delFillEngine(g, g_fillEngine); g_fillEngine = nullptr; }
    if (curPaintType == PaintType::CLIP_SEGMENT) { delClipLineEngine(g, g_clipLineEngine); g_clipLineEngine = nullptr; }
    // 保留裁剪窗口以便后续直线裁剪使用；仅在回到 SELECT 时清理
    if (curPaintType == PaintType::CLIP_WINDOW && mode == PaintType::SELECT) { delClipWindowEngine(g, g_clipWindowEngine); g_clipWindowEngine = nullptr; }

    curPaintType = mode;
    switch (curPaintType) {
    case PaintType::LINE:         printf("Line Mode\n"); break;
    case PaintType::CIRCLE:       printf("Circle Mode\n"); break;
    case PaintType::BEZIER:       printf("Bezier Mode\n"); break;
    case PaintType::SELECT:       curEngine = new SelectEngine; printf("Select Mode\n"); break;
    case PaintType::ARC_CIRCLE:   printf("Arc(Circle) Mode\n"); break;
    case PaintType::BSPLINE:      printf("Bspline Mode\n"); break;
    case PaintType::POLYGON:      printf("Polygon Mode (P): L-add, R-end, Shift+R to fill\n"); break;
    case PaintType::FILL_REGION:  printf("Fill Mode (F): L-click seed\n"); break;
    case PaintType::CLIP_SEGMENT: printf("Clip Segment Mode (K): L two points\n"); break;
    case PaintType::CLIP_WINDOW:  printf("Clip Window Mode (O): L-add, R-end (convex)\n"); break;
    case PaintType::FRUIT_NINJA:  printf("Fruit Ninja Game Mode (N)\n"); break;
    }
}

// ========================= 窗口过程桥接 =========================
int ActionsHandler(GraphicsEngine& Engine, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_RBUTTONDOWN: {
        Point p(LOWORD(lParam), HIWORD(lParam));
        return RMouseDown(Engine, p);
    }
    case WM_RBUTTONUP: {
        Point p(LOWORD(lParam), HIWORD(lParam));
        return RMouseUp(Engine, p);
    }
    case WM_LBUTTONDOWN: {
        Point p(LOWORD(lParam), HIWORD(lParam));
        return LmouseDown(Engine, p);
    }
    case WM_LBUTTONUP: {
        Point p(LOWORD(lParam), HIWORD(lParam));
        return LmouseUp(Engine, p);
    }
    case WM_MOUSEMOVE: {
        Point p(LOWORD(lParam), HIWORD(lParam));
        return mouseMove(Engine, p);
    }
    case WM_KEYDOWN: {
        int ret = 0;
        switch (wParam) {

            // ---- 原有快捷键（保持不变） ----
        case 'N': {
            switch_mode(Engine, PaintType::FRUIT_NINJA);
            ret = 1;
            break;
        }
        case 'A': {
            if (curPaintType == PaintType::CIRCLE) {
                switch_mode(Engine, PaintType::ARC_CIRCLE);
                ret = 1;
            }
            else {
                printf("Not in Circle Mode. Mode switch failed.\n");
            }
            break;
        }
        case 'L': { switch_mode(Engine, PaintType::LINE); ret = 1; break; }
        case 'C': { switch_mode(Engine, PaintType::CIRCLE); ret = 1; break; }
        case 'B': { switch_mode(Engine, PaintType::BEZIER); ret = 1; break; }
        case '1': { curLineWidth = 1; break; }
        case '2': { curLineWidth = 2; break; }
        case '4': { curLineWidth = 4; break; }
        case 'D': { if (curPaintType == PaintType::LINE) curDashed = true; break; }
        case 'S': {
            if (curPaintType == PaintType::LINE) {
                curDashed = false;
            }
            else if (curPaintType == PaintType::BEZIER) {
                switch_mode(Engine, PaintType::BSPLINE);
                ret = 1;
            }
            else {
                printf("Not in Line Mode or Bezier Mode. Line mode/Mode switch failed.\n");
            }
            break;
        }

                // ---- 选择模式变换 ----
        case 'T': {
            if (curPaintType == PaintType::SELECT && curEngine) {
                ((SelectEngine*)curEngine)->mvtype = MotionType::Translate;
                printf("Select Mode: Translate Selected\n");
            }
            break;
        }
        case 'R': {
            if (curPaintType == PaintType::SELECT && curEngine) {
                auto* e = (SelectEngine*)curEngine;
                e->mvtype = MotionType::Rotate;
                printf("Select Mode: Rotate Selected, Center (%d %d)\n", e->center.x, e->center.y);
            }
            break;
        }
        case 'Z': {
            if (curPaintType == PaintType::SELECT && curEngine) {
                auto* e = (SelectEngine*)curEngine;
                e->mvtype = MotionType::Scale;
                printf("Select Mode: Scale Selected, Center (%d %d)\n", e->center.x, e->center.y);
            }
            break;
        }

                // ---- Ctrl+P：取消固定中心（原逻辑保留）----
        case 'P': {
            if (ctrlPressed && curPaintType == PaintType::SELECT && curEngine) {
                auto* engine = (SelectEngine*)curEngine;
                engine->center = getCenter(engine->obj);
                engine->fixcenter = false;
                engine->selcenter = false;
                if (engine->selcp) {
                    engine->selcp->p = engine->center;
                    engine->selcp->draw(0, 0);
                }
                printf("Select Mode: Cancel fixed center\n");
                ret = 1;
            }
            break;
        }

                // ---- M：多边形模式（替代 P，避免冲突）----
        case 'M': {
            switch_mode(Engine, PaintType::POLYGON);
            printf("Switched to Polygon Mode (Hotkey: M)\n");
            ret = 1;
            break;
        }

                // ---- 新增：F / K / O ----
        case 'F': { switch_mode(Engine, PaintType::FILL_REGION); ret = 1; break; }
        case 'K': { switch_mode(Engine, PaintType::CLIP_SEGMENT); ret = 1; break; }
        case 'O': { switch_mode(Engine, PaintType::CLIP_WINDOW);  ret = 1; break; }

                // ---- 通用控制 ----
        case VK_SHIFT: { shiftPressed = !shiftPressed; break; }
        case VK_CONTROL: { ctrlPressed = !ctrlPressed;  break; }

                       //  不再清空画布，仅中断当前操作
        case VK_SPACE: { Interrupted(Engine); Engine.Clear(); ret = 1; break; }

                     // ESC：回到选择模式
        case VK_ESCAPE: { Interrupted(Engine); switch_mode(Engine, PaintType::SELECT, true); ret = 1; break; }

                      // 删除选中对象
        case VK_DELETE: {
            if (curPaintType == PaintType::SELECT && curEngine) {
                SelectEngine* engine = (SelectEngine*)curEngine;
                for (const auto& i : engine->obj) Engine.delObject(i);
                delSelectEngine(Engine, engine);
                curEngine = nullptr;
                ret = 1;
            }
            break;
        }
        }
        return ret;
    }

    default:
        return 0;
    }
}
