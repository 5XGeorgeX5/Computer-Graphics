#define UNICODE
#define _UNICODE

#include <windows.h>
#include <cmath>
#include <bits/stdc++.h>

using namespace std;

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

void DrawStar(HDC hdc, COLORREF colors[5], pair<int, int> points[5])
{
    for (int i = 0; i < 5; i++)
    {
        int skip = (i + 2) % 5;

        DrawLine(hdc, points[i].first, points[i].second,
                 points[skip].first, points[skip].second, colors[i]);
    }
}

int CrossProduct(pair<int, int> a, pair<int, int> b, pair<int, int> c)
{
    return (b.first - a.first) * (c.second - a.second) - (b.second - a.second) * (c.first - a.first);
}

void SortCounterClockwise(pair<int, int> points[5])
{
    pair<int, int> pivot = points[4];
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3 - i; ++j)
        {
            if (CrossProduct(pivot, points[j], points[j + 1]) < 0)
            {
                swap(points[j], points[j + 1]);
            }
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    static pair<int, int> points[5];
    static int cur = 0;
    COLORREF colors[5] = {RGB(0, 0, 0), RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(255, 0, 255)};

    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        HDC hdc = GetDC(hwnd);
        int x = LOWORD(lp);
        int y = HIWORD(lp);
        points[cur] = {x, y};

        SetPixel(hdc, x, y, RGB(255, 0, 0));

        cur++;
        if (cur == 5)
        {
            SortCounterClockwise(points);
            DrawStar(hdc, colors, points);
            cur = 0;
        }

        ReleaseDC(hwnd, hdc);
        break;
    }

    case WM_RBUTTONDOWN:
    {
        HDC hdc = GetDC(hwnd);
        COLORREF bgColor = RGB(255, 255, 255);

        for (int i = 0; i < cur; ++i)
        {
            SetPixel(hdc, points[i].first, points[i].second, bgColor);
        }

        cur = 0;

        ReleaseDC(hwnd, hdc);
        break;
    }

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hi, HINSTANCE pi, LPSTR cmd, int nsh)
{
    WNDCLASS wc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.lpszClassName = L"Star Drawing Algorithm";
    wc.lpszMenuName = NULL;
    wc.lpfnWndProc = WndProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = hi;
    RegisterClass(&wc);
    HWND hwnd = CreateWindow(L"Star Drawing Algorithm", L"Star Drawer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hi, 0);
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