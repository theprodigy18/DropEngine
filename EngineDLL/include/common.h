#pragma once

#include "../../EngineLib/include/common.h"
#include "../../EngineLib/include/Platform/WindowAPI.h"
#include "../../EngineLib/include/Graphics/GraphicsAPI.h"

#ifdef PLATFORM_LINUX
#define DLL_API
#elif defined(PLATFORM_WINDOWS)
#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif // DLL_EXPORTS
#else
#error "Platform not supported yet."
#endif // PLATFORM_LINUX
