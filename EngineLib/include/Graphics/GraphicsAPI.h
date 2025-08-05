#pragma once

#include "../Platform/WindowAPI.h"

EXTERN_C_BEGIN

typedef enum DROP_GFX_CLEAR
{
    DROP_GFX_CLEAR_NONE  = 0,
    DROP_GFX_CLEAR_COLOR = BIT(0),
    DROP_GFX_CLEAR_DEPTH = BIT(1)
} DROP_GFX_CLEAR;

typedef struct _GfxHandle* DROP_GfxHandle;
typedef struct _GfxInitProps
{
    DROP_WndHandle wndHandle;
} DROP_GfxInitProps;

EXTERN_C_END
