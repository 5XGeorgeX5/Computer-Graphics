#define UNICODE
#define _UNICODE

#include <Windows.h>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

int Round(double x)
{
    return (int)(x + 0.5);
}

struct Point
{
    double x, y;
};

void edgeToTable(map<int, pair<int, int>> &edgeTable, Point v1, Point v2)
{
    if (v1.y == v2.y)
        return;
    if (v1.y > v2.y)
        swap(v1, v2);

    double x = v1.x;
    double mi = (v2.x - v1.x) / (v2.y - v1.y);
    for (int y = (int)v1.y; y < (int)v2.y; ++y)
    {
        auto it = edgeTable.find(y);
        if (it == edgeTable.end())
            edgeTable[y] = {ceil(x), (int)x};
        else
        {
            if (it->second.first > x)
                it->second.first = (int)ceil(x);
            if (it->second.second < x)
                it->second.second = (int)x;
        }
        x += mi;
    }
}

void ConvixFill(HDC hdc, vector<Point> points, COLORREF fillColor)
{
    map<int, pair<int, int>> edgeTable;
    Point v1 = points.back();
    for (const auto &v2 : points)
    {
        edgeToTable(edgeTable, v1, v2);
        v1 = v2;
    }
    for (const auto &entry : edgeTable)
    {
        int y = entry.first;
        int x1 = entry.second.first;
        int x2 = entry.second.second;
        for (int x = x1; x <= x2; ++x)
        {
            SetPixel(hdc, x, y, fillColor);
        }
    }
}

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    static vector<Point> points;
    switch (m)
    {
    case WM_LBUTTONDOWN:
        points.push_back({(double)LOWORD(lp), (double)HIWORD(lp)});
        hdc = GetDC(hwnd);
        SetPixel(hdc, points.back().x, points.back().y, RGB(0, 0, 0));
        ReleaseDC(hwnd, hdc);
        break;
    case WM_RBUTTONDOWN:
        if (points.size() < 3)
            break;
        hdc = GetDC(hwnd);
        ConvixFill(hdc, points, RGB(255, 0, 0));
        ReleaseDC(hwnd, hdc);
        points.clear();
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