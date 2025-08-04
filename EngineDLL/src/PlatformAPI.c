#include "PlatformAPI.h"
#include "Platform/Window.h"

bool API_CreateWindow(WndHandle* pHandle, const WndInitProps* pProps)
{
    return Platform_CreateWindow(pHandle, pProps);
}

void API_DestroyWindow(WndHandle* pHandle)
{
    Platform_DestroyWindow(pHandle);
}

bool API_PollEvents()
{
    return Platform_PollEvents();
}

void API_ShowWindow(WndHandle handle)
{
    Platform_ShowWindow(handle);
}

void API_HideWindow(WndHandle handle)
{
    Platform_HideWindow(handle);
}
