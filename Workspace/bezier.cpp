//////////////////////////////
// George Raafat - 20220097 //
//////////////////////////////

#define UNICODE
#define _UNICODE
#include <Windows.h>
#include <cmath>
#include <iostream>
#include <functional>
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
    Point operator/(double scalar) const
    {
        return {x / scalar, y / scalar};
    }
};

int Round(double x)
{
    return (int)(x + 0.5);
}

void BezierIterative(HDC hdc, Point points[], int n, COLORREF color)
{
    const double step = 0.0001;
    for (double t = 0; t <= 1; t += step)
    {
        double term1 = pow(1 - t, n);
        double term2 = 1;
        double x = term1 * points[0].x;
        double y = term1 * points[0].y;
        long long nCr = 1;
        for (int r = 1; r <= n; r++)
        {
            term1 /= 1.0 - t;
            term2 *= t;
            nCr = nCr * (n - r + 1) / r;
            x += nCr * term1 * term2 * points[r].x;
            y += nCr * term1 * term2 * points[r].y;
        }
        SetPixel(hdc, Round(x), Round(y), color);
    }
}

Point calcBezier(double t, Point points[], int i, int j)
{
    if (i == j)
        return points[i];

    return {(1 - t) * calcBezier(t, points, i, j - 1) + t * calcBezier(t, points, i + 1, j)};
}

void BezierRecursive(HDC hdc, Point points[], int n, COLORREF color)
{
    const double step = 0.0001;
    for (double t = 0; t <= 1; t += step)
    {
        Point p = calcBezier(t, points, 0, n);
        SetPixel(hdc, Round(p.x), Round(p.y), color);
    }
}

void BezierRecursive2(HDC hdc, Point p[4], COLORREF color)
{
    if (abs(p[0].x - p[3].x) < 1 && abs(p[0].y - p[3].y) < 1)
        return;

    // Compute midpoints of segments
    Point q0 = (p[0] + p[1]) / 2.0;
    Point q1 = (p[1] + p[2]) / 2.0;
    Point q2 = (p[2] + p[3]) / 2.0;

    // Midpoints of q's
    Point r0 = (q0 + q1) / 2.0;
    Point r1 = (q1 + q2) / 2.0;

    // Final midpoint
    Point m = (r0 + r1) / 2.0;

    SetPixel(hdc, Round(m.x), Round(m.y), color);

    // Recursive subdivision
    Point left[4] = {p[0], q0, r0, m};
    Point right[4] = {m, r1, q2, p[3]};

    BezierRecursive2(hdc, left, color);
    BezierRecursive2(hdc, right, color);
}

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    static Point points[50];
    static int counter = 0;
    HDC hdc;
    switch (m)
    {
    case WM_LBUTTONDOWN:
    {
        points[counter] = {(double)LOWORD(lp), (double)HIWORD(lp)};
        ++counter;
        break;
    }
    case WM_RBUTTONDOWN:
    {
        hdc = GetDC(hwnd);
        BezierIterative(hdc, points, counter - 1, RGB(255, 0, 0));
        BezierRecursive2(hdc, points, RGB(0, 0, 255));
        ReleaseDC(hwnd, hdc);
        counter = 0;
        break;
    }
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
