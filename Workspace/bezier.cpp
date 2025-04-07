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
        return {points[i].x, points[i].y};

    Point p1 = calcBezier(t, points, i, j - 1);
    Point p2 = calcBezier(t, points, i + 1, j);
    return {(1 - t) * p1.x + t * p2.x, (1 - t) * p1.y + t * p2.y};
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

void BezierRecursive2(HDC hdc, Point points[4], COLORREF color)
{
    if (abs(points[0].x - points[3].x) < 1 && abs(points[0].y - points[3].y) < 1)
    {
        return;
    }
    Point q[3];
    q[0] = {(points[0].x + points[1].x) / 2, (points[0].y + points[1].y) / 2};
    q[1] = {(points[1].x + points[2].x) / 2, (points[1].y + points[2].y) / 2};
    q[2] = {(points[2].x + points[3].x) / 2, (points[2].y + points[3].y) / 2};
    Point r[2];
    r[0] = {(q[0].x + q[1].x) / 2, (q[0].y + q[1].y) / 2};
    r[1] = {(q[1].x + q[2].x) / 2, (q[1].y + q[2].y) / 2};
    Point p = {(r[0].x + r[1].x) / 2, (r[0].y + r[1].y) / 2};
    SetPixel(hdc, Round(p.x), Round(p.y), color);
    Point x1[4] = {points[0], q[0], r[0], p};
    Point x2[4] = {p, r[1], q[2], points[3]};
    BezierRecursive2(hdc, x1, color);
    BezierRecursive2(hdc, x2, color);
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
        BezierRecursive(hdc, points, counter - 1, RGB(0, 0, 255));
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
