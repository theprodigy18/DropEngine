#pragma once

#include "common.h"

EXTERN_C_BEGIN

DROP_API bool DROP_CreateWindow(DROP_WndHandle* pHandle, const DROP_WndInitProps* pProps);

DROP_API void DROP_DestroyWindow(DROP_WndHandle* pHandle);

DROP_API bool DROP_PollEvents();

DROP_API void DROP_ShowWindow(DROP_WndHandle handle);

DROP_API void DROP_HideWindow(DROP_WndHandle handle);

EXTERN_C_END
