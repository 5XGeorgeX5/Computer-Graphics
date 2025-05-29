#define UNICODE
#define _UNICODE

#include <Windows.h>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

using namespace std;

int Round(double x)
{
    return (int)(x + 0.5);
}

struct Point
{
    double x, y;
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
    edgeTable[v1.y].push_back({v1.x, (v2.x - v1.x) / (v2.y - v1.y), Round(v2.y)});
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
    int y = edgeTable.begin()->first;
    vector<Node> active = edgeTable.begin()->second;
    while (!active.empty())
    {
        // Sort active edges by x-coordinate
        sort(active.begin(), active.end());

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
        // increment y and update active edges
        ++y;
        active.erase(remove_if(active.begin(), active.end(), [y](const Node &node)
                               { return node.y == y; }),
                     active.end());
        // Update x-coordinates of active edges
        for (size_t i = 0; i < active.size(); ++i)
        {
            active[i].x += active[i].mi;
        }
        // Add new edges from edge table
        if (edgeTable.find(y) != edgeTable.end())
        {
            active.insert(active.end(), edgeTable[y].begin(), edgeTable[y].end());
        }
    }
}

Point vIntersection(Point p1, Point p2, double x)
{
    Point result;
    result.x = x;
    if (p1.x == p2.x)
        result.y = p1.y;
    else
        result.y = p1.y + (p2.y - p1.y) * (x - p1.x) / (p2.x - p1.x);
    return result;
}

Point hIntersection(Point p1, Point p2, double y)
{
    Point result;
    result.y = y;
    if (p1.y == p2.y)
        result.x = p1.x;
    else
        result.x = p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y);
    return result;
}

void PolygonClipping(HDC hdc, vector<Point> points, int left, int top, int right, int bottom)
{
    GeneralFill(hdc, points, RGB(0, 0, 0)); // Draw the original polygon
    vector<Point> result;
    Point p1 = points.back();
    for (const auto &p2 : points) // left
    {
        if (p1.x >= left && p2.x >= left)
        {
            result.push_back(p2);
        }
        else if (p2.x >= left)
        {
            result.push_back(vIntersection(p1, p2, left));
            result.push_back(p2);
        }
        else if (p1.x >= left)
        {
            result.push_back(vIntersection(p1, p2, left));
        }
        p1 = p2;
    }
    swap(points, result);
    result.clear();
    p1 = points.back();
    for (const auto &p2 : points) // right
    {
        if (p1.x <= right && p2.x <= right)
        {
            result.push_back(p2);
        }
        else if (p2.x <= right)
        {
            result.push_back(vIntersection(p1, p2, right));
            result.push_back(p2);
        }
        else if (p1.x <= right)
        {
            result.push_back(vIntersection(p1, p2, right));
        }
        p1 = p2;
    }
    swap(points, result);
    result.clear();
    p1 = points.back();
    for (const auto &p2 : points) // top
    {
        if (p1.y >= top && p2.y >= top)
        {
            result.push_back(p2);
        }
        else if (p2.y >= top)
        {
            result.push_back(hIntersection(p1, p2, top));
            result.push_back(p2);
        }
        else if (p1.y >= top)
        {
            result.push_back(hIntersection(p1, p2, top));
        }
        p1 = p2;
    }
    swap(points, result);
    result.clear();
    p1 = points.back();
    for (const auto &p2 : points) // bottom
    {
        if (p1.y <= bottom && p2.y <= bottom)
        {
            result.push_back(p2);
        }
        else if (p2.y <= bottom)
        {
            result.push_back(hIntersection(p1, p2, bottom));
            result.push_back(p2);
        }
        else if (p1.y <= bottom)
        {
            result.push_back(hIntersection(p1, p2, bottom));
        }
        p1 = p2;
    }
    GeneralFill(hdc, result, RGB(255, 0, 0)); // Fill the clipped polygon
}

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    static int left, top, right, bottom;
    static vector<Point> points;
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
        points.push_back({(double)LOWORD(lp), (double)HIWORD(lp)});
        hdc = GetDC(hwnd);
        SetPixel(hdc, points.back().x, points.back().y, RGB(0, 0, 0));
        ReleaseDC(hwnd, hdc);
        break;
    case WM_RBUTTONDOWN:
        if (points.size() < 3)
            break;
        hdc = GetDC(hwnd);
        PolygonClipping(hdc, points, left, top, right, bottom);
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