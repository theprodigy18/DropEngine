#pragma once

#include "Platform/Window.h"
#include <X11/Xlib.h>

typedef struct _WndHandle
{
    Display* pDisplay;
    Window   window;
} _WndHandle;
