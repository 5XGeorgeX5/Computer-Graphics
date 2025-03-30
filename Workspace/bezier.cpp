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

template <typename T>
struct Point
{
    T x, y;
};

int Round(double x)
{
    return (int)(x + 0.5);
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

Point<double> LinearInterpolation(Point<double> p1, Point<double> p2)
{
    return {p1.x + (p2.x - p1.x) * 0.5, p1.y + (p2.y - p1.y) * 0.5};
}

Point<double> BezierInterpolation(Point<double> points[], int n)
{
    Point<double> copyPoints[n + 1];
    for (int i = 0; i <= n; i++)
    {
        copyPoints[i] = points[i];
    }
    for (int r = 1; r <= n; r++)
    {
        for (int i = 0; i <= n - r; i++)
        {
            copyPoints[i] = LinearInterpolation(copyPoints[i], copyPoints[i + 1]);
        }
    }
    return copyPoints[0];
}

void DrawBezier(HDC hdc, Point<int> points[], int n, COLORREF color)
{
    const double step = 0.0001;
    Point<double> myPoints[n + 1];
    for (int i = 0; i <= n; i++)
    {
        myPoints[i] = {(double)points[i].x, (double)points[i].y};
    }
    Point<double> p = BezierInterpolation(myPoints, n);
    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            SetPixel(hdc, Round(p.x) + i, Round(p.y) + j, color);
        }
    }
}

void BezierIterative(HDC hdc, Point<int> points[], int n, COLORREF color)
{
    const double step = 0.0001;
    for (double t = 0; t <= 1; t += step)
    {
        double x = pow(1 - t, n) * points[0].x;
        double y = pow(1 - t, n) * points[0].y;
        long long nCr = 1;
        for (int r = 1; r <= n; r++)
        {
            nCr = nCr * (n - r + 1) / r;
            x += nCr * pow(t, r) * pow(1 - t, n - r) * points[r].x;
            y += nCr * pow(t, r) * pow(1 - t, n - r) * points[r].y;
        }
        SetPixel(hdc, Round(x), Round(y), color);
    }

    double midX = pow(0.5, n) * points[0].x;
    double midY = pow(0.5, n) * points[0].y;
    long long nCr2 = 1;
    for (int r = 1; r <= n; r++)
    {
        nCr2 = nCr2 * (n - r + 1) / r;
        midX += nCr2 * pow(0.5, n) * points[r].x;
        midY += nCr2 * pow(0.5, n) * points[r].y;
    }
    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            SetPixel(hdc, Round(midX) + i, Round(midY) + j, RGB(0, 0, 255));
        }
    }
}

Point<double> calcBezier(double t, Point<int> points[], int i, int j)
{
    if (i == j)
        return {(double)points[i].x, (double)points[i].y};

    Point<double> p1 = calcBezier(t, points, i, j - 1);
    Point<double> p2 = calcBezier(t, points, i + 1, j);
    return {(1 - t) * p1.x + t * p2.x, (1 - t) * p1.y + t * p2.y};
}

void BezierRecursive(HDC hdc, Point<int> points[], int n, COLORREF color)
{
    const double step = 0.0001;
    for (double t = 0; t <= 1; t += step)
    {
        Point<double> p = calcBezier(t, points, 0, n);
        SetPixel(hdc, Round(p.x), Round(p.y), color);
    }
}

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    static Point<int> points[50];
    static int counter = 0;
    HDC hdc;
    switch (m)
    {
    case WM_LBUTTONDOWN:
    {
        points[counter] = {LOWORD(lp), HIWORD(lp)};
        if (counter != 0)
        {
            hdc = GetDC(hwnd);
            DrawLine(hdc, points[counter - 1].x, points[counter - 1].y, points[counter].x, points[counter].y, RGB(0, 0, 0));
            ReleaseDC(hwnd, hdc);
        }
        ++counter;
        break;
    }
    case WM_RBUTTONDOWN:
    {
        hdc = GetDC(hwnd);
        BezierIterative(hdc, points, counter - 1, RGB(255, 0, 0));
        // BezierRecursive(hdc, points, counter - 1, RGB(0, 0, 255));
        // DrawBezier(hdc, points, counter - 1, RGB(0, 255, 0));
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
