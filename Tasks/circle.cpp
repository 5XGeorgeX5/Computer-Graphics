#define UNICODE
#define _UNICODE
#include <windows.h>
#include <cmath>
using namespace std;

void drawDDA(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c)
{

    int dx = x2 - x1, dy = y2 - y1;
    double y, x;
    SetPixel(hdc, x1, y1, c);

    if (abs(dx) > abs(dy))
    {
        if (x2 < x1)
        {
            swap(x1, x2);
            swap(y1, y2);
        }
        double m = (double)dy / dx;
        x = x1;
        y = y1;
        while (x < x2)
        {
            x += 1;
            y = m + y;
            SetPixel(hdc, x, round(y), c);
        }
    }
    else
    {
        if (y2 < y1)
        {
            swap(x1, x2);
            swap(y1, y2);
        }
        double mi = (double)dx / dy;
        y = y1;
        x = x1;
        while (y < y2)
        {
            y += 1;
            x = mi + x;
            SetPixel(hdc, round(x), y, c);
        }
    }
}

void DrawPoints(HDC hdc, int xc, int yc, int x, int y, COLORREF c, bool fillWithLines)
{
    SetPixel(hdc, xc + x, yc + y, c);
    SetPixel(hdc, xc - x, yc + y, c);
    SetPixel(hdc, xc + x, yc - y, c);
    SetPixel(hdc, xc - x, yc - y, c);
    SetPixel(hdc, xc + y, yc + x, c);
    SetPixel(hdc, xc - y, yc + x, c);
    SetPixel(hdc, xc - y, yc - x, c);
    SetPixel(hdc, xc + y, yc - x, c);
    if (fillWithLines)
    {
        // draw line between any two diagonal points
        drawDDA(hdc, xc + x, yc + y, xc - x, yc - y, RGB(255, 0, 0));
    }
}

void DrawCircleBres(HDC hdc, int xc, int yc, int R, COLORREF c, bool fillWithLines)
{
    int x = 0, y = R;
    int d = 1 - R;
    int d1 = 3;
    int d2 = -2 * R + 5;

    DrawPoints(hdc, xc, yc, x, y, c, fillWithLines);
    while (x <= y)
    {
        x++;
        if (d <= 0)
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
        DrawPoints(hdc, xc, yc, x, y, c, fillWithLines);
    }
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    static int x1, y1, x2, y2, counter = 0;
    switch (msg)
    {
    case WM_LBUTTONDOWN:
        break;
    case WM_LBUTTONUP:
        if (counter == 0)
        {
            x1 = LOWORD(lp);
            y1 = HIWORD(lp);
            counter++;
        }
        else if (counter == 1)
        {
            x2 = LOWORD(lp);
            y2 = HIWORD(lp);
            counter++;
        }
        break;
    case WM_RBUTTONUP:
        if (counter == 2)
        {
            hdc = GetDC(hwnd);
            // take the point
            int x, y;
            x = LOWORD(lp);
            y = HIWORD(lp);
            // calc the radii
            int R1 = (int)sqrt(pow((x1 - x), 2) + pow((y1 - y), 2));
            int R2 = (int)sqrt(pow((x2 - x), 2) + pow((y2 - y), 2));
            // draw
            DrawCircleBres(hdc, x1, y1, R1, RGB(255, 0, 0), true);
            DrawCircleBres(hdc, x2, y2, R2, RGB(255, 0, 0), false);

            ReleaseDC(hwnd, hdc);
            counter = 0;
        }
        else
        {
            // reset
            counter = 0;
        }
        break;
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

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSW wc = {0};
    wc.hbrBackground = CreateSolidBrush(RGB(50, 50, 50));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.lpszClassName = L"MyClass";
    wc.lpfnWndProc = WndProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = hInstance;
    if (!RegisterClassW(&wc))
    {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error", MB_ICONERROR);
        return 0;
    }
    HWND hwnd = CreateWindowW(L"MyClass", L"Circle Drawing - Polar Algorithm", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, hInstance, NULL);
    if (!hwnd)
    {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error", MB_ICONERROR);
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}