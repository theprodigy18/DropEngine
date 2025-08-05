#include "pch.h"
#include "Platform/Linux/Linux_Window.h"
#include "Platform/Linux/Linux_Events.h"

#pragma region INTERNAL
static bool     s_isInitialized = false;
static Display* s_pDisplay      = NULL;
static Atom     s_wmDelete      = 0;
static i32      s_wndCount      = 0;
static bool     s_keyHeld[256]  = {false};
#pragma endregion

bool Platform_CreateWindow(DROP_WndHandle* pHandle, const DROP_WndInitProps* pProps)
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

        FillKeyCodesLookupTable(s_pDisplay);

        s_isInitialized = true;
    }

    int     screen = XDefaultScreen(s_pDisplay);
    Window  root   = XRootWindow(s_pDisplay, screen);
    Visual* visual = DefaultVisual(s_pDisplay, screen);

    XSetWindowAttributes swa;
    swa.event_mask =
        ExposureMask |
        StructureNotifyMask |
        KeyPressMask |
        KeyReleaseMask |
        ButtonPressMask |
        ButtonReleaseMask |
        PointerMotionMask |
        FocusChangeMask |
        EnterWindowMask |
        LeaveWindowMask |
        PropertyChangeMask;

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

    DROP_WndHandle handle = ALLOC_SINGLE(_WndHandle);
    ASSERT(handle);
    handle->pDisplay = s_pDisplay;
    handle->window   = window;
    *pHandle         = handle;

    ++s_wndCount;

    return true;
}

void Platform_DestroyWindow(DROP_WndHandle* pHandle)
{
    ASSERT_MSG(pHandle && *pHandle, "Handle is NULL.");
    DROP_WndHandle handle = *pHandle;

    if (handle)
    {
        XDestroyWindow(s_pDisplay, handle->window);
        XSync(s_pDisplay, False);

        handle->window   = 0;
        handle->pDisplay = NULL;

        FREE(handle);
        handle = NULL;

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
        case KeyPress:
        {
            if (s_keyHeld[event.xkey.keycode])
                break;

            s_keyHeld[event.xkey.keycode] = true;
            DROP_KEYCODE keycode          = GetKeycode(event.xkey.keycode);

            LOG_TRACE("Key pressed: %s", KeyCodeToString(keycode));
            break;
        }
        case KeyRelease:
        {
            if (XPending(s_pDisplay))
            {
                XEvent nextEvent;
                XPeekEvent(s_pDisplay, &nextEvent);

                if (nextEvent.type == KeyPress &&
                    nextEvent.xkey.time == event.xkey.time &&
                    nextEvent.xkey.keycode == event.xkey.keycode)
                {
                    break;
                }
            }
            DROP_KEYCODE keycode          = GetKeycode(event.xkey.keycode);
            s_keyHeld[event.xkey.keycode] = false;
            LOG_TRACE("Key released: %s", KeyCodeToString(keycode));
            break;
        }
        default:
            break;
        }
    }

    return true;
}

void Platform_ShowWindow(DROP_WndHandle handle)
{
    XMapWindow(s_pDisplay, handle->window);
    XFlush(s_pDisplay);
}

void Platform_HideWindow(DROP_WndHandle handle)
{
    XUnmapSubwindows(s_pDisplay, handle->window);
    XFlush(s_pDisplay);
}
