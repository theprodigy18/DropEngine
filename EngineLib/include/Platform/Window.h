#pragma once

#include "WindowAPI.h"

EXTERN_C_BEGIN

bool Platform_CreateWindow(DROP_WndHandle* pHandle, const DROP_WndInitProps* pProps);

void Platform_DestroyWindow(DROP_WndHandle* pHandle);

bool Platform_PollEvents();

void Platform_ShowWindow(DROP_WndHandle handle);

void Platform_HideWindow(DROP_WndHandle handle);

EXTERN_C_END
