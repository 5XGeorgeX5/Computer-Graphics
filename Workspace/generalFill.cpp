#define UNICODE
#define _UNICODE

#include <Windows.h>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

int Round(double x)
{
    return (int)(x + 0.5);
}

struct Point
{
    int x, y;
};

struct Node
{
    double x;
    double mi;
    int y;
    bool operator<(const Node &other) const
    {
        return x < other.x;
    }
};

void edgeToTable(map<int, vector<Node>> &edgeTable, Point v1, Point v2)
{
    if (v1.y == v2.y)
        return;
    if (v1.y > v2.y)
        swap(v1, v2);

    if (edgeTable.find(v1.y) == edgeTable.end())
        edgeTable[v1.y] = vector<Node>();
    edgeTable[v1.y].push_back({(double)v1.x, (double)(v2.x - v1.x) / (v2.y - v1.y), v2.y});
}

void GeneralFill(HDC hdc, vector<Point> points, COLORREF fillColor)
{
    map<int, vector<Node>> edgeTable;
    Point v1 = points.back();
    for (const auto &v2 : points)
    {
        edgeToTable(edgeTable, v1, v2);
        v1 = v2;
    }
    cout << "Edge Table:\n";
    for (const auto &entry : edgeTable)
    {
        cout << "y = " << entry.first << ": ";
        for (const Node &node : entry.second)
        {
            cout << "(x = " << node.x << ", mi = " << node.mi << ") ";
        }
        cout << endl;
    }
    int y = edgeTable.begin()->first;
    vector<Node> active = edgeTable.begin()->second;
    while (!active.empty())
    {
        // Sort active edges by x-coordinate
        sort(active.begin(), active.end());
        cout << "Active edges at y = " << y << ":\n|| ";
        for (const Node &node : active)
        {
            cout << "x = " << node.x << ", mi = " << node.mi << " || ";
        }
        cout << endl;
        // Fill between pairs of x-coordinates
        for (size_t i = 0; i < active.size() - 1; i += 2)
        {
            int x1 = Round(active[i].x);
            int x2 = Round(active[i + 1].x);
            for (int x = x1; x <= x2; ++x)
            {
                SetPixel(hdc, x, y, fillColor);
            }
        }
        ++y;
        active.erase(remove_if(active.begin(), active.end(), [y](const Node &node)
                               { return node.y == y; }),
                     active.end());
        for (size_t i = 0; i < active.size(); ++i)
        {
            active[i].x += active[i].mi;
        }
        if (edgeTable.find(y) != edgeTable.end())
        {
            active.insert(active.end(), edgeTable[y].begin(), edgeTable[y].end());
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
        points.push_back({LOWORD(lp), HIWORD(lp)});
        hdc = GetDC(hwnd);
        SetPixel(hdc, points.back().x, points.back().y, RGB(0, 0, 0));
        ReleaseDC(hwnd, hdc);
        break;
    case WM_RBUTTONDOWN:
        if (points.size() < 3)
            break;
        hdc = GetDC(hwnd);
        GeneralFill(hdc, points, RGB(255, 0, 0));
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