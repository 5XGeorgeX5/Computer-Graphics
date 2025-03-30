#define UNICODE
#define _UNICODE

#include <Windows.h>
#include <stack>

using namespace std;

void FillArea(HDC hdc, int x, int y, COLORREF fillColor)
{
    COLORREF replace = GetPixel(hdc, x, y);

    stack<pair<int, int>> s;
    s.push({x, y});

    while (!s.empty())
    {
        auto [cx, cy] = s.top();
        s.pop();

        if (GetPixel(hdc, cx, cy) != replace)
            continue;

        SetPixel(hdc, cx, cy, fillColor);

        s.push({cx + 1, cy});
        s.push({cx - 1, cy});
        s.push({cx, cy + 1});
        s.push({cx, cy - 1});
    }
}

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    static int x1, y1;
    switch (m)
    {
    case WM_LBUTTONDOWN:
        x1 = LOWORD(lp);
        y1 = HIWORD(lp);
        break;
    case WM_MOUSEMOVE:
        if (wp & MK_LBUTTON)
        {
            hdc = GetDC(hwnd);

            int x2 = LOWORD(lp);
            int y2 = HIWORD(lp);

            MoveToEx(hdc, x1, y1, NULL);
            LineTo(hdc, x2, y2);

            x1 = x2;
            y1 = y2;

            ReleaseDC(hwnd, hdc);
        }
        break;
    case WM_RBUTTONDOWN:
        hdc = GetDC(hwnd);
        FillArea(hdc, LOWORD(lp), HIWORD(lp), RGB(255, 0, 0));
        ReleaseDC(hwnd, hdc);
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