#pragma once

#include "Platform/Events.h"
#include "Platform/Linux/Linux_Window.h"

#include <X11/keysym.h>
#include <X11/XKBlib.h>

// Fill key codes lookup table.
void FillKeyCodesLookupTable(Display* pDisplay);

DROP_KEYCODE GetKeycode(i32 keycode);

const char* KeyCodeToString(DROP_KEYCODE keycode);
