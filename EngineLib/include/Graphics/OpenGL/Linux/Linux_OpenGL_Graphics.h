#pragma once

#include "Graphics/Graphics.h"
#include "Platform/Linux/Linux_Window.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <GL/gl.h>


typedef struct _GfxHandle
{
    EGLSurface surface;
    EGLContext context;
} _GfxHandle;
