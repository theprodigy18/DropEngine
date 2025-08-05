#include "PlatformAPI.h"
#include "Platform/Window.h"

bool DROP_CreateWindow(DROP_WndHandle* pHandle, const DROP_WndInitProps* pProps)
{
    return Platform_CreateWindow(pHandle, pProps);
}

void DROP_DestroyWindow(DROP_WndHandle* pHandle)
{
    Platform_DestroyWindow(pHandle);
}

bool DROP_PollEvents()
{
    return Platform_PollEvents();
}

void DROP_ShowWindow(DROP_WndHandle handle)
{
    Platform_ShowWindow(handle);
}

void DROP_HideWindow(DROP_WndHandle handle)
{
    Platform_HideWindow(handle);
}
