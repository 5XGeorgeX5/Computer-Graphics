//////////////////////////////
// George Raafat - 20220097 //
//////////////////////////////

#define UNICODE
#define _UNICODE
#include <Windows.h>
#include <cmath>

int Round(double x)
{
    return (int)(x + 0.5);
}

void InterpolatedColoredLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c1, COLORREF c2)
{
    BYTE red1 = GetRValue(c1);
    BYTE green1 = GetGValue(c1);
    BYTE blue1 = GetBValue(c1);
    BYTE alphaRed = GetRValue(c2) - red1;
    BYTE alphaGreen = GetGValue(c2) - green1;
    BYTE alphaBlue = GetBValue(c2) - blue1;
    int alphaX = x2 - x1;
    int alphaY = y2 - y1;
    double step = 1.0 / std::max(abs(alphaX), abs(alphaY));
    for (double t = 0; t <= 1; t += step)
    {
        int red = red1 + Round(t * alphaRed);
        int green = green1 + Round(t * alphaGreen);
        int blue = blue1 + Round(t * alphaBlue);
        int x = x1 + Round(t * alphaX);
        int y = y1 + Round(t * alphaY);
        SetPixel(hdc, x, y, RGB(red, green, blue));
    }
}

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    static int x1, y1;
    HDC hdc;
    switch (m)
    {
    case WM_LBUTTONDOWN:
        x1 = LOWORD(lp);
        y1 = HIWORD(lp);
        break;
    case WM_LBUTTONUP:
    {
        int x2 = LOWORD(lp);
        int y2 = HIWORD(lp);

        hdc = GetDC(hwnd);
        InterpolatedColoredLine(hdc, x1, y1, x2, y2, RGB(255, 0, 0), RGB(0, 0, 255));
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