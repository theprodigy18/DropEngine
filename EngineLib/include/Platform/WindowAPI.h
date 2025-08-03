#pragma once

EXTERN_C_BEGIN

typedef struct _WndHandle* WndHandle;
typedef struct _WndInitProps
{
    char* title;
    i32   width;
    i32   height;
} WndInitProps;

EXTERN_C_END
