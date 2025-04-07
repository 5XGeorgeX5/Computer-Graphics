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

void DrawHermiteCurve(HDC hdc, Point points[2], Point slops[2], COLORREF color)
{
    double a1 = 2 * points[0].x - 2 * points[1].x + slops[0].x + slops[1].x;
    double a2 = -3 * points[0].x + 3 * points[1].x - 2 * slops[0].x - slops[1].x;
    double a3 = slops[0].x;
    double a4 = points[0].x;
    double b1 = 2 * points[0].y - 2 * points[1].y + slops[0].y + slops[1].y;
    double b2 = -3 * points[0].y + 3 * points[1].y - 2 * slops[0].y - slops[1].y;
    double b3 = slops[0].y;
    double b4 = points[0].y;
    const double step = 0.0001;
    for (double t = 0; t <= 1; t += step)
    {
        double x = a1 * t * t * t + a2 * t * t + a3 * t + a4;
        double y = b1 * t * t * t + b2 * t * t + b3 * t + b4;
        SetPixel(hdc, Round(x), Round(y), color);
    }
}

void DrawHermite(HDC hdc, Point points[], int n, COLORREF color)
{
    for (int i = 1; i < n - 2; i++)
    {
        Point slops[2] = {
            {points[i + 1].x - points[i - 1].x, points[i + 1].y - points[i - 1].y},
            {points[i + 2].x - points[i].x, points[i + 2].y - points[i].y},
        };
        DrawHermiteCurve(hdc, points + i, slops, color);
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
        points[counter] = {(double)LOWORD(lp), (double)HIWORD(lp)};
        hdc = GetDC(hwnd);
        SetPixel(hdc, points[counter].x, points[counter].y, RGB(0, 0, 0));
        ReleaseDC(hwnd, hdc);
        ++counter;
        break;
    }
    case WM_RBUTTONDOWN:
    {
        if (counter < 4)
            break;
        hdc = GetDC(hwnd);
        DrawHermite(hdc, points, counter, RGB(255, 0, 0));
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
