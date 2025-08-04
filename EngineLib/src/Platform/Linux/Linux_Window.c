#include "pch.h"
#include "Platform/Linux/Linux_Window.h"

#pragma region INTERNAL
static bool     s_isInitialized = false;
static Display* s_pDisplay      = NULL;
static Atom     s_wmDelete      = 0;
static i32      s_wndCount      = 0;
#pragma endregion

bool Platform_CreateWindow(WndHandle* pHandle, const WndInitProps* pProps)
{
    *pHandle = NULL;

    if (!s_isInitialized)
    {
        if (!XInitThreads())
        {
            ASSERT_MSG(false, "Failed to initialize X11 Threads.");
            return false;
        }

        s_pDisplay = XOpenDisplay(NULL);
        if (!s_pDisplay)
        {
            ASSERT_MSG(false, "Failed to open X11 Display.");
            return false;
        }

        s_wmDelete = XInternAtom(s_pDisplay, "WM_DELETE_WINDOW", False);
        if (!s_wmDelete)
        {
            ASSERT_MSG(false, "Failed to get WM Delete Atom.");
            return false;
        }

        s_isInitialized = true;
    }

    int     screen = XDefaultScreen(s_pDisplay);
    Window  root   = XRootWindow(s_pDisplay, screen);
    Visual* visual = DefaultVisual(s_pDisplay, screen);

    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | KeyPressMask;

    Window window = XCreateWindow(s_pDisplay, root,
                                  0, 0, pProps->width, pProps->height,
                                  0, CopyFromParent, InputOutput, visual, CWEventMask, &swa);
    if (!window)
    {
        ASSERT_MSG(false, "Failed to create X Window.");
        return false;
    }

    if (!XStoreName(s_pDisplay, window, pProps->title))
    {
        ASSERT_MSG(false, "Failed to store the X Window Title.");
        XDestroyWindow(s_pDisplay, window);
        return false;
    }

    if (!XSetWMProtocols(s_pDisplay, window, &s_wmDelete, 1))
    {
        ASSERT_MSG(false, "Failed to Attach Delete Protocol on X Window.");
        XDestroyWindow(s_pDisplay, window);
        return false;
    }

    XSync(s_pDisplay, False);

    WndHandle handle = ALLOC_SINGLE(_WndHandle);
    ASSERT(handle);
    handle->pDisplay = s_pDisplay;
    handle->window   = window;
    *pHandle         = handle;

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
        XDestroyWindow(s_pDisplay, handle->window);
        XSync(s_pDisplay, False);
        FREE(handle);

        --s_wndCount;
    }

    *pHandle = NULL;

    if (s_wndCount == 0)
    {
        XCloseDisplay(s_pDisplay);
        s_pDisplay      = NULL;
        s_wmDelete      = 0;
        s_isInitialized = false;
    }
}

bool Platform_PollEvents()
{
    while (XPending(s_pDisplay))
    {
        XEvent event;
        XNextEvent(s_pDisplay, &event);

        switch (event.type)
        {
        case ClientMessage:
            if ((Atom) event.xclient.data.l[0] == s_wmDelete)
                return false;
            break;
        default:
            break;
        }
    }

    return true;
}

void Platform_ShowWindow(WndHandle handle)
{
    XMapWindow(s_pDisplay, handle->window);
    XFlush(s_pDisplay);
}

void Platform_HideWindow(WndHandle handle)
{
    XUnmapSubwindows(s_pDisplay, handle->window);
    XFlush(s_pDisplay);
}
