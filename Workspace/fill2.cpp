#define UNICODE
#define _UNICODE

#include <Windows.h>
#include <stack>
#include <fstream>

using namespace std;

bool SaveScreen(HWND hwnd, string filename)
{
    // Open file stream in binary mode
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    HDC hdc = GetDC(hwnd);
    RECT rc;
    GetClientRect(hwnd, &rc);

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    // Allocate buffer for pixels
    COLORREF *pixels = new COLORREF[width * height];

    // Read pixels with GetPixel (slow but simple)
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            pixels[y * width + x] = GetPixel(hdc, x, y);

    // Write width and height
    file.write(reinterpret_cast<const char *>(&width), sizeof(int));
    file.write(reinterpret_cast<const char *>(&height), sizeof(int));

    // Write pixel data
    file.write(reinterpret_cast<const char *>(pixels), sizeof(COLORREF) * width * height);

    file.close();
    ReleaseDC(hwnd, hdc);
    delete[] pixels;
    return true;
}

bool LoadScreen(HWND hwnd, string filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    HDC hdc = GetDC(hwnd);

    int width, height;

    file.read(reinterpret_cast<char *>(&width), sizeof(int));
    file.read(reinterpret_cast<char *>(&height), sizeof(int));

    RECT rc = {0, 0, width, height};

    AdjustWindowRect(&rc, GetWindowLong(hwnd, GWL_STYLE), FALSE);

    SetWindowPos(hwnd, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

    COLORREF *pixels = new COLORREF[width * height];
    if (!pixels)
        return false;

    file.read(reinterpret_cast<char *>(pixels), sizeof(COLORREF) * width * height);
    file.close();

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            SetPixel(hdc, x, y, pixels[y * width + x]);

    ReleaseDC(hwnd, hdc);
    delete[] pixels;
    return true;
}

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
    case WM_KEYDOWN:
        if (wp == 'S') // Press 'S' to save
        {
            SaveScreen(hwnd, "screen.bin");
        }
        else if (wp == 'L') // Press 'L' to load
        {
            LoadScreen(hwnd, "screen.bin");
        }
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