#pragma once

#include "common.h"

EXTERN_C_BEGIN

DROP_API bool DROP_CreateGraphics(DROP_GfxHandle* pHandle, const DROP_GfxInitProps* pProps);

DROP_API void DROP_DestroyGraphics(DROP_GfxHandle* pHandle);

DROP_API bool DROP_SwapBuffers(DROP_GfxHandle handle);

DROP_API bool DROP_MakeCurrent(DROP_GfxHandle handle);

DROP_API void DROP_ClearColor(f32 r, f32 g, f32 b, f32 a);

DROP_API void DROP_ClearDepth(f32 depth);

DROP_API void DROP_Clear(DROP_GFX_CLEAR flags);

EXTERN_C_END
