#define UNICODE
#define _UNICODE
#include <Windows.h>
int Round(double x)
{
    return (int)(x + 0.5);
}
void DrawLineDDA(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c)
{
    int dx = x2 - x1, dy = y2 - y1;
    SetPixel(hdc, x1, y1, c);
    if (abs(dx) >= abs(dy))
    {
        double m = (double)dy / dx;
        int x = x1;
        double y = y1;
        while (x < x2)
        {
            x++;
            y += m;
            SetPixel(hdc, x, Round(y), c);
        }
    }
    else
    {
        double mi = (double)dx / dy;
        int y = y1;
        double x = x1;
        while (y < y2)
        {
            y++;
            x += mi;
            SetPixel(hdc, Round(x), y, c);
        }
    }
}

void DrawLineBresenham(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c)
{
    if (x1 > x2)
    {
        int t = x1;
        x1 = x2;
        x2 = t;
        t = y1;
        y1 = y2;
        y2 = t;
    }
    int dx = x2 - x1, dy = y2 - y1;
    int d1 = -2 * dy;
    int d2 = 2 * (dx - dy);
    int d = dx - d1;
    int x = x1, y = y1;
    SetPixel(hdc, x, y, c);
    while (x < x2)
    {
        x++;
        if (d < 0)
        {
            d += d2;
            y++;
        }
        else
        {
            d += d1;
        }
        SetPixel(hdc, x, y, c);
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
    switch (m)
    {
    case WM_LBUTTONDOWN:
        hdc = GetDC(hwnd);
        DrawLineDDA(hdc, 10, 10, 300, 100, RGB(255, 0, 0));
        ReleaseDC(hwnd, hdc);
        break;
    case WM_LBUTTONUP:
        hdc = GetDC(hwnd);
        DrawLineBresenham(hdc, 15, 15, 305, 105, RGB(0, 255, 0));
        ReleaseDC(hwnd, hdc);
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