#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

#include "Point.h"
#include <cmath>
#include <vector>
#include <windows.h>

class Object;


class GraphicsEngine {
    private:
        int width, height;
    
        HBITMAP buffer;
        int bufferWidth, bufferHeight;
    
    public:
        Object *objhead, *objtail;
        GraphicsEngine();
        ~GraphicsEngine();
        int GetWidth() const{ return width; }
        int GetHeight() const{ return height; }
        int SetWidth(int nwidth) { width = nwidth; return 0; }
        int SetHeight(int nheight) { height = nheight; return 0; }

        void pushObject(Object* obj);
        Object* popObject();
        void addObject(Object* obj, Object* pre, bool check = true);
        void delObject(Object* obj, bool check = true);
        void Clear();

        void createBuffer(int width, int height);
        void deleteBuffer();
        void renderBuffer();
        void draw(HDC hdc);
        void Flush();
};

static int sel_threshold = 10;
class Object {
    public:
        Object *pre, *nxt;
        std::vector<Pixel> points;
        COLORREF color;

        Object(): pre(nullptr), nxt(nullptr), color(RGB(0,0,0)) {}
        virtual ~Object() = default;
        void setColor(byte r, byte g, byte b) { color = RGB(r, g, b); }
        virtual void draw(int width, int height);
        virtual void transform(TMatrix m, int d = 1);
        virtual int select(Point p);

        virtual void backup();
        virtual void restore();
};

#endif