#define UNICODE
#define _UNICODE

#include <windows.h>
#include <vector>
using namespace std;

int Round(double x)
{
    return (int)(x + 0.5);
}

struct Point
{
    double x, y;
};

vector<vector<int>> matrixMultiplication(vector<vector<int>> matrix1, vector<vector<int>> matrix2)
{
    int height = matrix1.size();
    int width = matrix2[0].size();
    int n = matrix1[0].size();
    vector<vector<int>> result(height, vector<int>(width));
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            for (int k = 0; k < n; k++)
            {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
    return result;
}

void DrawBezier(HDC hdc, Point points[4], COLORREF colors[4])
{
    const vector<vector<int>> H =
        {
            {-1, 3, -3, 1},
            {3, -6, 3, 0},
            {-3, 3, 0, 0},
            {1, 0, 0, 0},
        };
    vector<vector<int>> values(4, vector<int>(5));
    for (int i = 0; i < 4; i++)
    {

        values[i][0] = points[i].x;
        values[i][1] = points[i].y;
        values[i][2] = GetRValue(colors[i]);
        values[i][3] = GetGValue(colors[i]);
        values[i][4] = GetBValue(colors[i]);
    }
    const vector<vector<int>> coefficients = matrixMultiplication(H, values);
    double step = 0.0005;
    for (double t = 0; t <= 1; t += step)
    {
        double result[5] = {0.0};
        vector<double> powers = {t * t * t, t * t, t, 1};
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                result[i] += powers[j] * coefficients[j][i];
            }
        }
        int red = Round(result[2]);
        int green = Round(result[3]);
        int blue = Round(result[4]);
        SetPixel(hdc, Round(result[0]), Round(result[1]), RGB(red, green, blue));
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    static Point points[4];
    static int count = 0;
    HDC hdc;
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        points[count++] = {(double)LOWORD(lp), (double)HIWORD(lp)};
        if (count == 4)
        {
            COLORREF colors[4] = {
                RGB(255, 0, 0),
                RGB(0, 255, 0),
                RGB(0, 0, 255),
                RGB(255, 255, 0),
            };
            hdc = GetDC(hwnd);
            DrawBezier(hdc, points, colors);
            ReleaseDC(hwnd, hdc);
            count = 0;
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
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