#ifndef ACTIONS_OBJECTS_H
#define ACTIONS_OBJECTS_H

#include "GraphicsEngine.h"

static int SelMarkSize = 3;

class SelPoint: public Object {
    public:
        Point p;
        SelPoint(Point _p = Point(0,0)): p(_p) { }

        void draw(int width, int height) override;
        void transform(TMatrix m, int d = 1) override;
};

class SelFrame: public Object {
    public:
        int left, right, top, bottom;
        Object* obj;

        SelFrame(Object* _obj);

        void draw(int width, int height) override;
        void transform(TMatrix m, int d = 1) override;
};

#endif
