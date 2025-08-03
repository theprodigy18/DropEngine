#include <LibAPI.h>

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

    WndHandle handle;

    Platform_CreateWindow(&handle, &props);

    Platform_ShowWindow(handle);

    bool running = true;
    while (running)
    {
        running = Platform_PollEvents();
    }

    Platform_DestroyWindow(&handle);

#ifdef PLATFORM_WINDOWS
    _CrtDumpMemoryLeaks();
#endif // PLATFORM_WINDOWS
    return 0;
}
