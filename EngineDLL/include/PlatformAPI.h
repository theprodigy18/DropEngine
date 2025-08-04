#pragma once

#include "common.h"

EXTERN_C_BEGIN

DLL_API bool API_CreateWindow(WndHandle* pHandle, const WndInitProps* pProps);

DLL_API void API_DestroyWindow(WndHandle* pHandle);

DLL_API bool API_PollEvents();

DLL_API void API_ShowWindow(WndHandle handle);

DLL_API void API_HideWindow(WndHandle handle);

EXTERN_C_END
