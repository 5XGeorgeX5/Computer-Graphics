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

struct DoublePoint
{
    double x, y;
};

struct Point
{
    int x, y;
};

int Round(double x)
{
    return (int)(x + 0.5);
}

void DrawLineDDA(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    SetPixel(hdc, x1, y1, color);
    if (abs(dx) >= abs(dy))
    {
        double m = (double)dy / dx;
        if (x1 > x2)
        {
            swap(x1, x2);
            swap(y1, y2);
        }
        double y = y1;
        for (int x = x1; x <= x2; x++)
        {
            y += m;
            SetPixel(hdc, x, Round(y), color);
        }
    }
    else
    {
        double mi = (double)dx / dy;
        if (y1 > y2)
        {
            swap(x1, x2);
            swap(y1, y2);
        }
        double x = x1;
        for (int y = y1; y <= y2; y++)
        {
            x += mi;
            SetPixel(hdc, Round(x), y, color);
        }
    }
}

DoublePoint LinearInterpolation(DoublePoint p1, DoublePoint p2)
{
    return {p1.x + (p2.x - p1.x) * 0.5, p1.y + (p2.y - p1.y) * 0.5};
}

DoublePoint BezierInterpolation(DoublePoint points[], int n)
{
    DoublePoint copyPoints[n + 1];
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

void DrawBezier(HDC hdc, Point points[], int n, COLORREF color)
{
    const double step = 0.0001;
    DoublePoint myPoints[n + 1];
    for (int i = 0; i <= n; i++)
    {
        myPoints[i] = {(double)points[i].x, (double)points[i].y};
    }
    DoublePoint p = BezierInterpolation(myPoints, n);
    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            SetPixel(hdc, Round(p.x) + i, Round(p.y) + j, color);
        }
    }
}

void BezierIterative(HDC hdc, Point points[], int n, COLORREF color)
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

pair<double, double> calcBezier(double t, Point points[], int i, int j)
{
    if (i == j)
        return {points[i].x, points[i].y};

    pair<double, double> p1 = calcBezier(t, points, i, j - 1);
    pair<double, double> p2 = calcBezier(t, points, i + 1, j);
    return {(1 - t) * p1.first + t * p2.first, (1 - t) * p1.second + t * p2.second};
}

void BezierRecursive(HDC hdc, Point points[], int n, COLORREF color)
{
    const double step = 0.0001;
    for (double t = 0; t <= 1; t += step)
    {
        pair<double, double> p = calcBezier(t, points, 0, n);
        SetPixel(hdc, Round(p.first), Round(p.second), color);
    }
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
        points[counter] = {LOWORD(lp), HIWORD(lp)};
        if (counter != 0)
        {
            hdc = GetDC(hwnd);
            DrawLineDDA(hdc, points[counter - 1].x, points[counter - 1].y, points[counter].x, points[counter].y, RGB(0, 0, 0));
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
        //DrawBezier(hdc, points, counter - 1, RGB(0, 255, 0));
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
