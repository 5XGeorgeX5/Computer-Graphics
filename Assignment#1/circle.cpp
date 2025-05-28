//////////////////////////////
// George Raafat - 20220097 //
//////////////////////////////

#define UNICODE
#define _UNICODE
#include <Windows.h>
#include <cmath>
#include <iostream>
using namespace std;

int Round(double x)
{
    return (int)(x + 0.5);
}

void DrawCirclePoints(HDC hdc, int xc, int yc, int x, int y, COLORREF c)
{
    SetPixel(hdc, xc + x, yc + y, c);
    SetPixel(hdc, xc - x, yc + y, c);
    SetPixel(hdc, xc + x, yc - y, c);
    SetPixel(hdc, xc - x, yc - y, c);

    SetPixel(hdc, xc + y, yc + x, c);
    SetPixel(hdc, xc - y, yc + x, c);
    SetPixel(hdc, xc + y, yc - x, c);
    SetPixel(hdc, xc - y, yc - x, c);
}

void DrawCircleBres(HDC hdc, int xc, int yc, int R, COLORREF c)
{
    int x = 0, y = R;
    int d = 1 - R;
    int d1 = 3, d2 = 5 - 2 * R;
    DrawCirclePoints(hdc, xc, yc, x, y, c);
    cout << "X: " << x << ", Y: " << y << ", d: " << d << ", d1: " << d1 << ", d2: " << d2 << endl;
    while (x < y)
    {
        if (d < 0)
        {
            d += d1;
            d2 += 2;
        }
        else
        {
            d += d2;
            d2 += 4;
            y--;
        }
        d1 += 2;
        x++;
        cout << "X: " << x << ", Y: " << y << ", d: " << d << ", d1: " << d1 << ", d2: " << d2 << endl;
        DrawCirclePoints(hdc, xc, yc, x, y, c);
    }
}

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    static int xc, yc; // center point
    HDC hdc;
    switch (m)
    {
    case WM_LBUTTONDOWN:
        xc = LOWORD(lp);
        yc = HIWORD(lp);
        break;
    case WM_LBUTTONUP:
    {
        int dx = xc - LOWORD(lp);
        int dy = yc - HIWORD(lp);
        int R = Round(sqrt(dx * dx + dy * dy)); // radius

        hdc = GetDC(hwnd);
        // DrawCircleBres(hdc, xc, yc, R, RGB(0, 0, 0));
        DrawCircleBres(hdc, 10, 10, 5, RGB(0, 0, 0));
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