#define UNICODE
#define _UNICODE

#include <Windows.h>
#include <stack>
#include <cmath>

using namespace std;

struct Point
{
    double x, y;
    Point operator+(const Point &p) const { return {x + p.x, y + p.y}; }
    Point operator*(double scalar) const
    {
        return {x * scalar, y * scalar};
    }
    friend Point operator*(double scalar, const Point &p)
    {
        return p * scalar;
    }
};

int Round(double x)
{
    return (int)(x + 0.5);
}

int sideLength(Point p1, Point p2)
{
    return (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
}

void FillTriangle(HDC hdc, Point points[3], COLORREF fillColor)
{
    int a = sideLength(points[0], points[1]);
    int b = sideLength(points[1], points[2]);
    int c = sideLength(points[2], points[0]);
    const double step = 1.0 / sqrt(max(a, max(b, c)));
    for (double t1 = 0.0; t1 <= 1.0; t1 += step)
    {
        for (double t2 = 0.0; t2 <= 1.0 - t1; t2 += step)
        {
            Point result = t1 * points[0] + t2 * points[1] + (1 - t1 - t2) * points[2];
            SetPixel(hdc, Round(result.x), Round(result.y), fillColor);
        }
    }
}

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    static Point points[3];
    static int counter = 0;
    switch (m)
    {
    case WM_LBUTTONDOWN:
        points[counter++] = {(double)LOWORD(lp), (double)HIWORD(lp)};
        if (counter == 3)
        {
            hdc = GetDC(hwnd);
            FillTriangle(hdc, points, RGB(255, 0, 0));
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