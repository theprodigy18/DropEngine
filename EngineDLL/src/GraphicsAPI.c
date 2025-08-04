#include "GraphicsAPI.h"
#include "Graphics/Graphics.h"

bool API_CreateGraphics(GfxHandle* pHandle, const GfxInitProps* pProps)
{
    return Graphics_CreateGraphics(pHandle, pProps);
}

void API_DestroyGraphics(GfxHandle* pHandle)
{
    Graphics_DestroyGraphics(pHandle);
}

bool API_SwapBuffers(GfxHandle handle)
{
    return Graphics_SwapBuffers(handle);
}

bool API_MakeCurrent(GfxHandle handle)
{
	return Graphics_MakeCurrent(handle);
}

void API_ClearColor(f32 r, f32 g, f32 b, f32 a)
{
    Graphics_ClearColor(r, g, b, a);
}

void API_ClearDepth(f32 depth)
{
    Graphics_ClearDepth(depth);
}

void API_Clear(u32 flags)
{
    Graphics_Clear(flags);
}
