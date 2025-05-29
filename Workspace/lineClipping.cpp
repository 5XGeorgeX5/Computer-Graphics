#define UNICODE
#define _UNICODE

#include <Windows.h>
#include <cmath>

using namespace std;

int Round(double x)
{
    return (int)(x + 0.5);
}

struct Point
{
    int x, y;
};

union OutCode
{
    struct
    {
        unsigned left : 1;
        unsigned right : 1;
        unsigned top : 1;
        unsigned bottom : 1;
    };
    unsigned all : 4;
};

void DrawLineDDA(HDC hdc, Point p1, Point p2, COLORREF color)
{
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    if (abs(dx) >= abs(dy))
    {
        double m = (double)dy / dx;
        if (p1.x > p2.x)
        {
            swap(p1, p2);
        }
        double y = p1.y;
        for (int x = p1.x; x <= p2.x; x++)
        {
            SetPixel(hdc, x, Round(y), color);
            y += m;
        }
    }
    else
    {
        double mi = (double)dx / dy;
        if (p1.y > p2.y)
        {
            swap(p1, p2);
        }
        double x = p1.x;
        for (int y = p1.y; y <= p2.y; y++)
        {
            SetPixel(hdc, Round(x), y, color);
            x += mi;
        }
    }
}

void LineClipping(HDC hdc, Point p1, Point p2, int left, int top, int right, int bottom)
{
    OutCode code1, code2;
    code1.all = 0;
    code2.all = 0;
    if (p1.x < left)
        code1.left = 1; // left
    else if (p1.x > right)
        code1.right = 1; // right
    if (p1.y < top)
        code1.top = 1; // top
    else if (p1.y > bottom)
        code1.bottom = 1; // bottom

    if (p2.x < left)
        code2.left = 1; // left
    else if (p2.x > right)
        code2.right = 1; // right
    if (p2.y < top)
        code2.top = 1; // top
    else if (p2.y > bottom)
        code2.bottom = 1; // bottom

    double slope = (double)(p2.y - p1.y) / (p2.x - p1.x);

    while (code1.all || code2.all)
    {
        if ((code1.all & code2.all) != 0)
            return; // both points outside the clipping rectangle

        OutCode &outCode = code1.all ? code1 : code2;
        Point &p = code1.all ? p1 : p2;
        double value;

        if (outCode.left) // to the left of the rectangle
        {
            p.y += slope * (left - p.x);
            p.x = left;
        }
        else if (outCode.right) // to the right of the rectangle
        {
            p.y += slope * (right - p.x);
            p.x = right;
        }
        else if (outCode.top) // above the rectangle
        {
            p.x += (top - p.y) / slope;
            p.y = top;
        }
        else if (outCode.bottom) // below the rectangle
        {
            p.x += (bottom - p.y) / slope;
            p.y = bottom;
        }
        outCode.all = 0; // reset outCode after updating point
        if (p.x < left)
            outCode.left = 1; // left
        else if (p.x > right)
            outCode.right = 1; // right
        if (p.y < top)
            outCode.top = 1; // top
        else if (p.y > bottom)
            outCode.bottom = 1; // bottom
    }
    DrawLineDDA(hdc, p1, p2, RGB(0, 0, 0));
}

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    static int left, top, right, bottom;
    static Point p1, p2;
    static bool draw = false;
    switch (m)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);

        // Get the size of the client area
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        // Calculate center and square size
        int squareSize = 100; // You can change this
        left = (width - squareSize) / 2;
        top = (height - squareSize) / 2;
        right = left + squareSize;
        bottom = top + squareSize;

        // Draw the square (as a rectangle)
        Rectangle(hdc, left, top, right + 1, bottom + 1);

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        if (!draw)
        {
            p1 = {LOWORD(lp), HIWORD(lp)};
            draw = true;
        }
        else
        {
            hdc = GetDC(hwnd);
            p2 = {LOWORD(lp), HIWORD(lp)};
            // DrawLineDDA(hdc, p1, p2, RGB(255, 0, 0));
            LineClipping(hdc, p1, p2, left, top, right, bottom);
            draw = false;
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