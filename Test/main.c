#include <DLLAPI.h>

#ifdef PLATFORM_WINDOWS
#include <crtdbg.h>
#endif // PLATFORM_WINDOWS

int main()
{
    LOG_TRACE("Hello World");

    DROP_WndInitProps props;
    props.title  = "DropTest";
    props.width  = 1280;
    props.height = 720;

    DROP_WndHandle handle = NULL;
    DROP_CreateWindow(&handle, &props);

    DROP_GfxInitProps gfxProps;
    gfxProps.wndHandle = handle;

    DROP_GfxHandle gfxHandle = NULL;
    DROP_CreateGraphics(&gfxHandle, &gfxProps);

    DROP_ShowWindow(handle);

    bool running = true;
    while (running)
    {
        running = DROP_PollEvents();
        DROP_ClearColor(0.1f, 0.2f, 0.5f, 1.0f);
        DROP_Clear(DROP_GFX_CLEAR_COLOR);
        DROP_SwapBuffers(gfxHandle);
    }

    DROP_DestroyGraphics(&gfxHandle);
    DROP_DestroyWindow(&handle);

#ifdef PLATFORM_WINDOWS
    _CrtDumpMemoryLeaks();
#endif // PLATFORM_WINDOWS
    return 0;
}
