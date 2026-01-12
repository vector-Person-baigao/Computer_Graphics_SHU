#include <stdio.h>
#include <windows.h>
#include "GraphicsEngine.h"
#include "Actions.h"
#include "fruit_ninja.h"

GraphicsEngine Engine;
const wchar_t* CLASS_NAME = L"Window";
const wchar_t* WINDOW_TITLE = L"Application";

extern PaintType curPaintType;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // 为游戏模式设置定时器
        SetTimer(hwnd, 1,16, NULL); // 约60FPS
        return 0;
    }

    case WM_TIMER: {
        if (curPaintType == PaintType::FRUIT_NINJA) {
            g_fruitNinjaGame.update(0.016); // 16ms
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }

    case WM_SIZE: {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        Engine.SetWidth(clientRect.right - clientRect.left);
        Engine.SetHeight(clientRect.bottom - clientRect.top);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // 游戏模式特殊处理
        if (curPaintType == PaintType::FRUIT_NINJA) {
            Engine.Clear(); // 清空原有对象
            g_fruitNinjaGame.render(Engine);
        }

        Engine.draw(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_KEYDOWN: {
        int res = ActionsHandler(Engine, uMsg, wParam, lParam);
        if (res) {
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }

    case WM_ERASEBKGND: {
        return 1;
    }

    case WM_DESTROY: {
        KillTimer(hwnd, 1); // 销毁定时器
        PostQuitMessage(0);
        return 0;
    }

    default: {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        CLASS_NAME, L"graphics",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        Engine.GetWidth(), Engine.GetHeight(), NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

#ifdef _CONSOLE
int main() {
    return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWDEFAULT);
}
#endif