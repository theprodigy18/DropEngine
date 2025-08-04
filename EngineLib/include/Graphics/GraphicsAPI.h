#pragma once

#include "../Platform/WindowAPI.h"

#define GFX_CLEAR_COLOR 0x0001
#define GFX_CLEAR_DEPTH 0x0010

EXTERN_C_BEGIN

typedef struct _GfxHandle* GfxHandle;
typedef struct _GfxInitProps
{
    WndHandle wndHandle;
} GfxInitProps;

EXTERN_C_END
