#pragma once

EXTERN_C_BEGIN

typedef struct _WndHandle* DROP_WndHandle;
typedef struct _WndInitProps
{
    char* title;
    i32   width;
    i32   height;
} DROP_WndInitProps;

EXTERN_C_END
