#pragma once

#include "GraphicsAPI.h"

EXTERN_C_BEGIN

// Create a new Graphics handle.
bool Graphics_CreateGraphics(DROP_GfxHandle* pHandle, const DROP_GfxInitProps* pProps);

// Destroy a Graphics handle.
void Graphics_DestroyGraphics(DROP_GfxHandle* pHandle);

// Swap backbuffer to frontbuffer.
bool Graphics_SwapBuffers(DROP_GfxHandle handle);

// Make this Graphics handle current.
bool Graphics_MakeCurrent(DROP_GfxHandle handle);

// Clear rendering surface with given color.
void Graphics_ClearColor(f32 r, f32 g, f32 b, f32 a);

// CLear the depth of surface.
void Graphics_ClearDepth(f32 depth);

// Perform all clearing with specified flags.
void Graphics_Clear(DROP_GFX_CLEAR flags);

EXTERN_C_END
