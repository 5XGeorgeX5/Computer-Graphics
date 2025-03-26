#define UNICODE
#define _UNICODE

#include <Windows.h>
#include <algorithm>
#include <iostream>
using namespace std;

int Round(double x)
{
    return (int)(x + 0.5);
}
void DrawLineDirect(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (abs(dx) >= abs(dy))
    {
        double m = (double)dy / dx;
        if (x1 > x2)
        {
            swap(x1, x2);
            swap(y1, y2);
        }
        for (int x = x1; x <= x2; x++)
        {
            int y = y1 + Round((x - x1) * m);
            SetPixel(hdc, x, y, color);
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
        for (int y = y1; y <= y2; y++)
        {
            int x = x1 + Round((y - y1) * mi);
            SetPixel(hdc, x, y, color);
        }
    }
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

void DrawLineBresenham(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    double slope = (double)dy / dx;
    int x = x1;
    int y = y1;
    SetPixel(hdc, x, y, c);
    SetPixel(hdc, x2, y2, c);
    if (0 <= slope && slope <= 1)
    {
        if (x1 > x2)
        {
            swap(x1, x2);
            swap(y1, y2);
            dx = x2 - x1;
            dy = y2 - y1;
            x = x1;
            y = y1;
        }
        int d = dx - 2 * dy;
        int change1 = -2 * dy;
        int change2 = 2 * (dx - dy);
        while (x < x2)
        {
            x++;
            if (d > 0)
            {
                d += change1;
            }
            else
            {
                d += change2;
                y++;
            }
            SetPixel(hdc, x, y, c);
        }
    }
    else if (slope > 1)
    {
        if (y1 > y2)
        {
            swap(x1, x2);
            swap(y1, y2);
            dx = x2 - x1;
            dy = y2 - y1;
            x = x1;
            y = y1;
        }
        int d = 2 * dx - dy;
        int change1 = 2 * dx;
        int change2 = 2 * (dx - dy);
        while (y < y2)
        {
            y++;
            if (d < 0)
            {
                d += change1;
            }
            else
            {
                d += change2;
                x++;
            }
            SetPixel(hdc, x, y, c);
        }
    }
    else if (slope >= -1)
    {
        if (x1 > x2)
        {
            swap(x1, x2);
            swap(y1, y2);
            dx = x2 - x1;
            dy = y2 - y1;
            x = x1;
            y = y1;
        }
        int d = -(dx + 2 * dy);
        int change1 = -2 * dy;
        int change2 = -2 * (dx + dy);
        while (x < x2)
        {
            x++;
            if (d < 0)
            {
                d += change1;
            }
            else
            {
                d += change2;
                y--;
            }
            SetPixel(hdc, x, y, c);
        }
    }
    else
    {
        if (y1 > y2)
        {
            swap(x1, x2);
            swap(y1, y2);
            dx = x2 - x1;
            dy = y2 - y1;
            x = x1;
            y = y1;
        }
        int d = 2 * dx + dy;
        int change1 = 2 * dx;
        int change2 = 2 * (dx + dy);
        while (y < y2)
        {
            y++;
            if (d > 0)
            {
                d += change1;
            }
            else
            {
                d += change2;
                x--;
            }
            SetPixel(hdc, x, y, c);
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

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    static int x1, y1;
    static bool draw = false;
    switch (m)
    {
    case WM_LBUTTONDOWN:
    {
        if (!draw)
        {
            x1 = LOWORD(lp);
            y1 = HIWORD(lp);
            draw = true;
        }
        else
        {
            hdc = GetDC(hwnd);
            int x2 = LOWORD(lp);
            int y2 = HIWORD(lp);
            DrawLineBresenham(hdc, x1, y1, x2, y2, RGB(255, 0, 0));
            ReleaseDC(hwnd, hdc);
            draw = false;
        }
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
    AllocConsole(); // Creates a new console window
    FILE *f;
    freopen_s(&f, "CONOUT$", "w", stdout);
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