#include <DLLAPI.h>

#ifdef PLATFORM_WINDOWS
#include <crtdbg.h>
#endif // PLATFORM_WINDOWS

int main()
{
    LOG_TRACE("Hello World");

    WndInitProps props;
    props.title  = "DropTest";
    props.width  = 1280;
    props.height = 720;

    WndHandle handle = NULL;
    API_CreateWindow(&handle, &props);

    GfxInitProps gfxProps;
    gfxProps.wndHandle = handle;

    GfxHandle gfxHandle = NULL;
    API_CreateGraphics(&gfxHandle, &gfxProps);

    API_ShowWindow(handle);

    bool running = true;
    while (running)
    {
        running = API_PollEvents();
        API_ClearColor(0.1f, 0.2f, 0.5f, 1.0f);
        API_Clear(GFX_CLEAR_COLOR);
        API_SwapBuffers(gfxHandle);
    }

    API_DestroyGraphics(&gfxHandle);
    API_DestroyWindow(&handle);

#ifdef PLATFORM_WINDOWS
    _CrtDumpMemoryLeaks();
#endif // PLATFORM_WINDOWS
    return 0;
}
