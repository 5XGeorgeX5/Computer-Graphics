#define UNICODE
#define _UNICODE

#include <Windows.h>
#include <stack>

using namespace std;

bool SaveFileDialog(HWND owner, wchar_t *filename)
{
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = filename;
    wcscpy_s(filename, MAX_PATH, L"untitled.bmp");
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Bitmap Files (*.bmp)\0*.bmp\0";
    ofn.lpstrDefExt = L"bmp";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    return GetSaveFileName(&ofn) == TRUE;
}

bool SaveHDCToBitmapFile(HWND hwnd)
{
    wchar_t filename[MAX_PATH];
    if (!SaveFileDialog(hwnd, filename))
    {
        return false;
    }
    HDC hdc = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 24;
    bi.bmiHeader.biCompression = BI_RGB;

    int rowSize = ((24 * width + 31) / 32) * 4;
    int dataSize = rowSize * height;

    BYTE *data = new BYTE[dataSize];

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(memDC, hBitmap);
    BitBlt(memDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

    GetDIBits(memDC, hBitmap, 0, height, data, &bi, DIB_RGB_COLORS);

    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    BITMAPFILEHEADER bmfHeader = {};
    bmfHeader.bfType = 0x4D42;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = bmfHeader.bfOffBits + dataSize;

    WriteFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), NULL, NULL);
    WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), NULL, NULL);
    WriteFile(hFile, data, dataSize, NULL, NULL);
    CloseHandle(hFile);

    ReleaseDC(hwnd, hdc);
    DeleteObject(hBitmap);
    DeleteDC(memDC);
    delete[] data;

    return true;
}

bool OpenFileDialog(HWND owner, wchar_t *filename)
{
    OPENFILENAME ofn; // common dialog box structure
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = filename;
    // Set initial file name buffer to empty string
    filename[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Bitmap Files (*.bmp)\0*.bmp\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    // Display the Open dialog box
    return GetOpenFileName(&ofn) == TRUE;
}

bool LoadBitmapToHDC(HWND hwnd)
{
    wchar_t filename[MAX_PATH];
    if (!OpenFileDialog(hwnd, filename))
    {
        return false;
    }
    HBITMAP hBitmap = (HBITMAP)LoadImageW(
        NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if (!hBitmap)
    {
        MessageBoxW(NULL, L"Failed to load bitmap.", L"Error", MB_ICONERROR);
        return false;
    }

    HDC hdc = GetDC(hwnd);
    HDC memDC = CreateCompatibleDC(hdc);
    HGDIOBJ oldBitmap = SelectObject(memDC, hBitmap);

    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    int imgW = bmp.bmWidth;
    int imgH = bmp.bmHeight;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int posX = (screenWidth - imgW) / 2;
    int posY = (screenHeight - imgH) / 2;

    RECT rc = {0, 0, imgW, imgH};
    AdjustWindowRect(&rc, GetWindowLong(hwnd, GWL_STYLE), FALSE);

    SetWindowPos(hwnd, NULL,
                 posX, posY,
                 imgW, imgH,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, memDC, 0, 0, SRCCOPY);

    ReleaseDC(hwnd, hdc);
    SelectObject(memDC, oldBitmap);
    DeleteDC(memDC);
    DeleteObject(hBitmap);

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
            SaveHDCToBitmapFile(hwnd);
        }
        else if (wp == 'L') // Press 'L' to load
        {
            LoadBitmapToHDC(hwnd);
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