#pragma once

#include "Platform/Window.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

typedef struct _WndHandle
{
	HWND hwnd;
	HDC hdc;
} _WndHandle;