#define UNICODE
#define _UNICODE

#include <Windows.h>
#include <stack>

using namespace std;

struct Point
{
    int x, y;
};

int Round(double x)
{
    return (int)(x + 0.5);
}

void FillArea(HDC hdc, int x, int y, COLORREF fillColor)
{
    COLORREF replace = GetPixel(hdc, x, y);

    stack<pair<int, int>> s;
    s.push({x, y});

    while (!s.empty())
    {
        auto [cx, cy] = s.top();
        s.pop();

        if (GetPixel(hdc, cx, cy) != replace)
            continue;

        SetPixel(hdc, cx, cy, fillColor);

        s.push({cx + 1, cy});
        s.push({cx - 1, cy});
        s.push({cx, cy + 1});
        s.push({cx, cy - 1});
    }
}

void FillTriangle(HDC hdc, Point points[3], COLORREF fillColor)
{
    const double step = 0.01;
    for (double t1 = 0.0; t1 <= 1.0; t1 += step)
    {
        for (double t2 = 0.0; t2 <= 1.0 - t1; t2 += step)
        {
            double x = t1 * points[0].x + t2 * points[1].x + (1 - t1 - t2) * points[2].x;
            double y = t1 * points[0].y + t2 * points[1].y + (1 - t1 - t2) * points[2].y;
            SetPixel(hdc, Round(x), Round(y), fillColor);
        }
    }
}

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        SetPixel(hdc, x1, y1, color);
        if (x1 == x2 && y1 == y2)
            break;
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void DrawTriangle(HDC hdc, Point points[3], COLORREF color)
{
    DrawLine(hdc, points[0].x, points[0].y, points[1].x, points[1].y, color);
    DrawLine(hdc, points[1].x, points[1].y, points[2].x, points[2].y, color);
    DrawLine(hdc, points[2].x, points[2].y, points[0].x, points[0].y, color);
    FillTriangle(hdc, points, RGB(255, 0, 0));
}

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    static Point points[3];
    static int counter = 0;
    switch (m)
    {
    case WM_LBUTTONDOWN:
        points[counter++] = {LOWORD(lp), HIWORD(lp)};
        if (counter == 3)
        {
            hdc = GetDC(hwnd);
            DrawTriangle(hdc, points, RGB(0, 0, 255));
            counter = 0;
            ReleaseDC(hwnd, hdc);
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, m, wp, lp);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hi, HINSTANCE pi, LPSTR cmd, int nsh)
{
    WNDCLASS wc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.lpszClassName = L"MyClass";
    wc.lpszMenuName = NULL;
    wc.lpfnWndProc = WndProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = hi;
    RegisterClass(&wc);
    HWND hwnd = CreateWindow(L"MyClass", L"Hello World!", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hi, 0);
    ShowWindow(hwnd, nsh);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}