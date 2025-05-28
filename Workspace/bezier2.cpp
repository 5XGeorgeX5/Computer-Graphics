#define UNICODE
#define _UNICODE
#include <Windows.h>
#include <vector>
#include <cmath>
using namespace std;

struct Point
{
    double x, y;
};

inline int Round(double x) { return (int)(x + 0.5); }

vector<Point> multiply_matrix(vector<vector<int>> A, vector<Point> B)
{
    vector<Point> result;
    for (const auto &row : A)
    {
        double newX = 0, newY = 0;
        for (int i = 0; i < row.size(); ++i)
        {
            newX += row[i] * B[i].x;
            newY += row[i] * B[i].y;
        }
        result.push_back({newX, newY});
    }
    return result;
}

void DrawBezier(HDC hdc, const vector<Point> &controlPoints, COLORREF color, int number_of_points)
{
    vector<vector<int>> bezierMatrix = {
        {-1, 3, -3, 1},
        {3, -6, 3, 0},
        {-3, 3, 0, 0},
        {1, 0, 0, 0},
    };
    vector<Point> result = multiply_matrix(bezierMatrix, controlPoints);
    double step = 1.0 / number_of_points;
    for (double t = 0; t <= 1; t += step)
    {
        double arr[4] = {t * t * t, t * t, t, 1};
        double x = 0, y = 0;
        for (int i = 0; i < 4; i++)
        {
            x += arr[i] * result[i].x;
            y += arr[i] * result[i].y;
        }
        SetPixel(hdc, Round(x), Round(y), color);
    }
}

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    static vector<Point> points;
    static int counter = 0;
    HDC hdc;
    switch (m)
    {
    case WM_LBUTTONDOWN:
    {
        points.push_back({(double)LOWORD(lp), (double)HIWORD(lp)});
        hdc = GetDC(hwnd);
        SetPixel(hdc, points[counter].x, points[counter].y, RGB(0, 0, 0));
        ReleaseDC(hwnd, hdc);
        ++counter;
        break;
    }
    case WM_RBUTTONDOWN:
    {
        if (counter < 4)
            break;
        hdc = GetDC(hwnd);
        DrawBezier(hdc, points, RGB(255, 0, 0), 1000);
        ReleaseDC(hwnd, hdc);
        points.clear();
        counter = 0;
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
