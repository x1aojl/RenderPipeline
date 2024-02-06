// WinMain.cpp
// Created by xiaojl Feb/06/2024
// Win32窗口
// https://learn.microsoft.com/zh-cn/windows/win32/learnwin32/your-first-windows-program

#include <windows.h>

// 屏幕尺寸
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 800

// 帧缓冲
int FrameBuffer[SCREEN_WIDTH][SCREEN_HEIGHT];

// 系统窗口句柄
HDC majorHdc;

// 兼容窗口句柄
HDC minorHdc;

// 位图句柄
HBITMAP bmp;

// 位图指针
void *bmpPtr = nullptr;

// 窗口处理函数的声明
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 窗口初始化
void WindowInit(HWND hWnd)
{
    // 系统窗口句柄
    majorHdc = GetDC(hWnd);

    // 兼容窗口句柄
    minorHdc = CreateCompatibleDC(majorHdc);

    // 位图句柄
    BITMAPINFO bmpInfo = { { sizeof(BITMAPINFOHEADER), SCREEN_WIDTH, -SCREEN_HEIGHT, 1, 32, BI_RGB, SCREEN_WIDTH * SCREEN_HEIGHT * 4, 0, 0, 0, 0 } };
    bmp = CreateDIBSection(minorHdc, &bmpInfo, DIB_RGB_COLORS, &bmpPtr, NULL, 0);
}

// 窗口销毁
void WindowDestroy(HWND hWnd)
{
    //释放主窗口句柄
    ReleaseDC(hWnd, majorHdc);

    // 销毁窗口相关对象
    DeleteObject(bmp);
    DeleteObject(minorHdc);
    DeleteObject(majorHdc);

    // 销毁窗口
    DestroyWindow(hWnd);
}

// 窗口绘制
void WindowDraw()
{
    for (auto i = 0; i < SCREEN_WIDTH; ++i)
    {
        for (auto j = 0; j < SCREEN_HEIGHT; ++j)
        {
            auto index = i + j * SCREEN_HEIGHT;
            FrameBuffer[i][j] = RGB(207, 216, 129); // 蒂芙尼蓝
        }
    }

    // 将像素数据填充到位图对应的内存空间内
    memcpy(bmpPtr, FrameBuffer[0], SCREEN_WIDTH * SCREEN_HEIGHT * 4);

    // 将位图绑定到兼容窗口
    SelectObject(minorHdc, bmp);

    // 将兼容窗口内容复制到系统窗口
    BitBlt(majorHdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, minorHdc, 0, 0, SRCCOPY);
}

// 主入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 定义名称
    LPCTSTR className = "WindowClass";
    LPCTSTR windowName = "RenderPipeline";

    // 注册窗口
    WNDCLASS wc = {};
    wc.lpszClassName = className;
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WindowProc;
    RegisterClass(&wc);

    // 创建窗口
    HWND hWnd = CreateWindow(
        className,           // window class
        windowName,          // window name
        WS_OVERLAPPEDWINDOW, // window style
        CW_USEDEFAULT,       // position x
        CW_USEDEFAULT,       // position y 
        SCREEN_WIDTH,        // size x
        SCREEN_HEIGHT,       // size y
        NULL,                // parent window
        NULL,                // menu
        hInstance,           // instance
        NULL                 // param
    );

    WindowInit(hWnd);
    ShowWindow(hWnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(className, hInstance);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        WindowDestroy(hWnd);
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        WindowDraw();
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}