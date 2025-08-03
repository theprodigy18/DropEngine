#pragma once

#include "WindowAPI.h"

EXTERN_C_BEGIN

bool Platform_CreateWindow(WndHandle* pHandle, const WndInitProps* pProps);

void Platform_DestroyWindow(WndHandle* pHandle);

bool Platform_PollEvents();

void Platform_ShowWindow(WndHandle handle);

void Platform_HideWindow(WndHandle handle);

EXTERN_C_END
