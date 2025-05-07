#define UNICODE
#define _UNICODE
#include <Windows.h>

struct Point
{
    double x, y;
};

int Round(double x)
{
    return (int)(x + 0.5);
}

void print3X3(HDC hdc, int x, int y)
{
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            SetPixel(hdc, x + i, y + j, RGB(0, 0, 0));
        }
    }
}

Point calcBezier(double t, Point points[], int i, int j)
{
    if (i == j)
        return {points[i].x, points[i].y};

    Point p1 = calcBezier(t, points, i, j - 1);
    Point p2 = calcBezier(t, points, i + 1, j);
    return {(1.0 - t) * p1.x + t * p2.x, (1.0 - t) * p1.y + t * p2.y};
}

void BezierRecursive(HDC hdc, Point points[4], COLORREF color)
{
    const double step = 0.0001;
    for (double t = 0; t <= 1.0; t += step)
    {
        Point p = calcBezier(t, points, 0, 3);
        SetPixel(hdc, Round(p.x), Round(p.y), color);
    }
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

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    static Point points[4];
    static int counter = 0;
    HDC hdc;
    switch (m)
    {
    case WM_LBUTTONDOWN:
    {
        if (counter == 4)
        {
            hdc = GetDC(hwnd);
            Point slops[2] = {
                {points[1].x - points[0].x, points[1].y - points[0].y},
                {points[2].x - points[3].x, points[2].y - points[3].y},
            };
            Point hermitePoints[2] = {points[0], points[3]};
            DrawHermiteCurve(hdc, hermitePoints, slops, RGB(255, 0, 0));
            counter = 0;
            ReleaseDC(hwnd, hdc);
            break;
        }
        points[counter] = {(double)LOWORD(lp), (double)HIWORD(lp)};
        hdc = GetDC(hwnd);
        print3X3(hdc, LOWORD(lp), HIWORD(lp));
        ReleaseDC(hwnd, hdc);
        ++counter;
        break;
    }
    case WM_RBUTTONDOWN:
    {
        if (counter == 4)
        {
            hdc = GetDC(hwnd);
            BezierRecursive(hdc, points, RGB(0, 0, 255));
            counter = 0;
            ReleaseDC(hwnd, hdc);
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
    WNDCLASS wc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.lpszClassName = L"BezierHermite";
    wc.lpszMenuName = NULL;
    wc.lpfnWndProc = WndProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = hi;
    RegisterClass(&wc);
    HWND hwnd = CreateWindow(L"BezierHermite", L"Bezier & Hermite!", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hi, 0);
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
