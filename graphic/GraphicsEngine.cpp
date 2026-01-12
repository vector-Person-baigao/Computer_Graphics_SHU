#include "GraphicsEngine.h"

#include <stdio.h>
#include <algorithm>

GraphicsEngine::GraphicsEngine() {
    objhead = objtail = nullptr;
    width = 800; height = 600;
    buffer = nullptr;
    bufferWidth = bufferHeight = 0;
}

GraphicsEngine::~GraphicsEngine() {
    deleteBuffer();
}

void GraphicsEngine::pushObject(Object* obj) {
    if(objhead == nullptr){
        objhead = objtail = obj;
        obj->pre = obj->nxt = nullptr;
    }else{
        obj->pre = objtail; objtail->nxt = obj;
        objtail = obj; obj->nxt = nullptr;
    }
}

Object* GraphicsEngine::popObject() {
    if(objtail == nullptr) return nullptr;
    Object* ret = objtail;
    objtail = objtail->pre;
    ret->pre = nullptr;
    if(objtail == nullptr) objhead = nullptr;
                      else objtail->nxt = nullptr;
    return ret;
}

void GraphicsEngine::addObject(Object* obj, Object* pre, bool check) {
    if (check) {
        bool match = false;
        for (Object* cur = objhead; cur != nullptr; cur = cur->nxt) {
            if (cur == obj) {
                fprintf(stderr, "object already exists.\n");
                return;
            }
            if (cur == pre) match = true;
        }
        if (pre != nullptr && !match) {
            fprintf(stderr, "predecessor object not found.\n");
            return;
        }
    }
    if (pre == nullptr) {
        obj->nxt = objhead;
        objhead = obj;
        if (obj->nxt) obj->nxt->pre = obj;
                 else objtail = obj;
    } else {
        obj->nxt = pre->nxt;
        obj->pre = pre;
        pre->nxt = obj;
        if (obj->nxt) obj->nxt->pre = obj;
                 else objtail = obj;
    }
}

void GraphicsEngine::delObject(Object* obj, bool check) {
    if (check) {
        bool match = false;
        for (Object* cur = objhead; cur != nullptr; cur = cur->nxt)
            if (cur == obj) {
                match = true;
                break;
            }
        if (!match) {
            fprintf(stderr, "object not found.\n");
            return;
        }
    }
    if (obj->pre) obj->pre->nxt = obj->nxt;
             else objhead = obj->nxt;
    if (obj->nxt) obj->nxt->pre = obj->pre;
             else objtail = obj->pre;
    obj->pre = obj->nxt = nullptr;
    return;

}

void GraphicsEngine::createBuffer(int width, int height) {
    if(buffer && bufferWidth == width && bufferHeight == height) {
        return;
    }

    deleteBuffer();

    HDC screenDC = GetDC(NULL);
    buffer = CreateCompatibleBitmap(screenDC, width, height);
    ReleaseDC(NULL, screenDC);

    bufferWidth = width;
    bufferHeight = height;
}

void GraphicsEngine::deleteBuffer() {
    if(buffer) {
        DeleteObject(buffer);
        buffer = nullptr;
        bufferWidth = bufferHeight = 0;
    }
}

void GraphicsEngine::renderBuffer() {
    if(!buffer) return;


    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, buffer);

    RECT rect = {0, 0, bufferWidth, bufferHeight};
    HBRUSH whiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    FillRect(memDC, &rect, whiteBrush);

    for(Object* obj = objhead; obj != nullptr; obj = obj->nxt) {
        // HPEN hPen;
        // int penStyle = obj->dashed ? PS_DASH : PS_SOLID;
        // hPen = CreatePen(penStyle, obj->lineWidth, RGB(0, 0, 0));
        // HPEN oldPen = (HPEN)SelectObject(memDC, hPen);

        for (const auto& point: obj->points){
            if (0 <= point.x && point.x < bufferWidth && 0 <= point.y && point.y < bufferHeight)
                SetPixel(memDC, point.x, point.y, point.color);
        }

        // SelectObject(memDC, hOldPen);
        // DeleteObject(hPen);
    }

    SelectObject(memDC, oldBitmap);
    DeleteDC(memDC);
    ReleaseDC(NULL, screenDC);
}

void GraphicsEngine::Flush() {
    for(Object* obj = objhead; obj != nullptr; obj = obj->nxt) {
        obj->draw(bufferWidth, bufferHeight);
    }
    renderBuffer();
}

void GraphicsEngine::Clear() {
    for(Object *obj = objhead, *nxt; obj != nullptr; obj = nxt) {
        nxt = obj->nxt;
        delete obj;
    }
    objhead = objtail = nullptr;
    renderBuffer();
}

void GraphicsEngine::draw(HDC hdc) {
    createBuffer(width, height);
    if(!buffer) return;
    renderBuffer();

    HDC actualhdc = static_cast<HDC>(hdc);
    HDC memDC = CreateCompatibleDC(actualhdc);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, buffer);

    BitBlt(actualhdc, 0, 0, bufferWidth, bufferHeight, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBitmap);
    DeleteDC(memDC);
}

void Object::draw(int width, int height) {}
void Object::transform(TMatrix m, int d) {}
void Object::backup() {}
void Object::restore() {}
int Object::select(Point p) {
    for (const auto& point: points){
        int dis = pow(point.x - p.x, 2) + pow(point.y - p.y, 2);
        if(dis <= pow(sel_threshold, 2))
            return 1;
    }
    return 0;
}