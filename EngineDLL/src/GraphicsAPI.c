#include "GraphicsAPI.h"
#include "Graphics/Graphics.h"

bool DROP_CreateGraphics(DROP_GfxHandle* pHandle, const DROP_GfxInitProps* pProps)
{
    return Graphics_CreateGraphics(pHandle, pProps);
}

void DROP_DestroyGraphics(DROP_GfxHandle* pHandle)
{
    Graphics_DestroyGraphics(pHandle);
}

bool DROP_SwapBuffers(DROP_GfxHandle handle)
{
    return Graphics_SwapBuffers(handle);
}

bool DROP_MakeCurrent(DROP_GfxHandle handle)
{
    return Graphics_MakeCurrent(handle);
}

void DROP_ClearColor(f32 r, f32 g, f32 b, f32 a)
{
    Graphics_ClearColor(r, g, b, a);
}

void DROP_ClearDepth(f32 depth)
{
    Graphics_ClearDepth(depth);
}

void DROP_Clear(DROP_GFX_CLEAR flags)
{
    Graphics_Clear(flags);
}
