#include "pch.h"
#include "Platform/Windows/Windows_Window.h"

#pragma region INTERNAL
static const wchar_t* ConvertUtf8ToUtf16(const char* pUtf8)
{
    i32      len    = MultiByteToWideChar(CP_UTF8, 0, pUtf8, -1, NULL, 0);
    wchar_t* pUtf16 = ALLOC_ARRAY(wchar_t, len);
    MultiByteToWideChar(CP_UTF8, 0, pUtf8, -1, pUtf16, len);
    return pUtf16;
}

static LRESULT CALLBACK InternalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool      s_isInitialized = false;
static HINSTANCE s_hInstance     = NULL;
static i32       s_wndCount      = 0;
#pragma endregion

bool Platform_CreateWindow(WndHandle* pHandle, const WndInitProps* pProps)
{
    *pHandle = NULL;

    if (!s_isInitialized)
    {
        s_hInstance = GetModuleHandleW(NULL);
        if (!s_hInstance)
        {
            ASSERT_MSG(false, "Failed to get module handle.");
            return false;
        }

        WNDCLASSEXW wcex;
        ZeroMemory(&wcex, sizeof(wcex));
        wcex.cbSize        = sizeof(wcex);
        wcex.style         = CS_OWNDC;
        wcex.hInstance     = s_hInstance;
        wcex.lpfnWndProc   = InternalWndProc;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = 0;
        wcex.hCursor       = LoadCursorW(NULL, IDC_ARROW);
        wcex.hIcon         = LoadIconW(NULL, IDI_APPLICATION);
        wcex.hIconSm       = LoadIconW(NULL, IDI_APPLICATION);
        wcex.lpszMenuName  = NULL;
        wcex.lpszClassName = L"DropEngine";

        if (!RegisterClassExW(&wcex))
        {
            ASSERT_MSG(false, "Failed to register window class.");
            return false;
        }

        SetProcessDPIAware();

        s_isInitialized = true;
    }

    DWORD dwStyle = WS_OVERLAPPEDWINDOW;

    RECT rc = {0, 0, pProps->width, pProps->height};
    AdjustWindowRect(&rc, dwStyle, FALSE);

    const wchar_t* title = ConvertUtf8ToUtf16(pProps->title);

    HWND hwnd = CreateWindowExW(0,
                                L"DropEngine", title,
                                dwStyle,
                                CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
                                NULL, NULL, s_hInstance, NULL);
    FREE(title);

    if (!hwnd)
    {
        ASSERT_MSG(false, "Failed to Window Handle.");
        return false;
    }

    WndHandle handle = ALLOC_SINGLE(_WndHandle);
    ASSERT(handle);
    handle->hwnd = hwnd;
    handle->hdc  = GetDC(hwnd);
    *pHandle     = handle;

    ++s_wndCount;

    return true;
}

void Platform_DestroyWindow(WndHandle* pHandle)
{
    ASSERT_MSG(pHandle, "pHandle is NULL.");
    WndHandle handle = *pHandle;
    ASSERT_MSG(handle, "Handle is NULL.");

    if (handle)
    {
        ReleaseDC(handle->hwnd, handle->hdc);
        DestroyWindow(handle->hwnd);
        FREE(handle);

        --s_wndCount;
    }

    *pHandle = NULL;

    if (s_wndCount == 0)
    {
        UnregisterClassW(L"DropEngine", s_hInstance);
		s_hInstance = NULL;
        s_isInitialized = false;
    }
}

bool Platform_PollEvents()
{
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return true;
}

void Platform_ShowWindow(WndHandle handle)
{
    ShowWindow(handle->hwnd, SW_SHOW);
}

void Platform_HideWindow(WndHandle handle)
{
    ShowWindow(handle->hwnd, SW_HIDE);
}
