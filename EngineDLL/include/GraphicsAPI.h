#pragma once

#include "common.h"

EXTERN_C_BEGIN

DLL_API bool API_CreateGraphics(GfxHandle* pHandle, const GfxInitProps* pProps);

DLL_API void API_DestroyGraphics(GfxHandle* pHandle);

DLL_API bool API_SwapBuffers(GfxHandle handle);

DLL_API bool API_MakeCurrent(GfxHandle handle);

DLL_API void API_ClearColor(f32 r, f32 g, f32 b, f32 a);

DLL_API void API_ClearDepth(f32 depth);

DLL_API void API_Clear(u32 flags);

EXTERN_C_END
