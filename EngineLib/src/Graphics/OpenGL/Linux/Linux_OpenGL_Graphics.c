#include "pch.h"
#include "Graphics/OpenGL/Linux/Linux_OpenGL_Graphics.h"

#include <string.h>

#pragma region INTERNAL
#pragma region GLFunctionProcs
#define LoadGLFunction(type, name)          \
    name = (type) eglGetProcAddress(#name); \
    ASSERT_MSG(name, "Failed to load OpenGL function: %s" #name);

static PFNGLCREATEPROGRAMPROC           glCreateProgram;
static PFNGLCREATESHADERPROC            glCreateShader;
static PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
static PFNGLUNIFORM1FPROC               glUniform1f;
static PFNGLUNIFORM2FVPROC              glUniform2fv;
static PFNGLUNIFORM3FVPROC              glUniform3fv;
static PFNGLUNIFORM1IPROC               glUniform1i;
static PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;
static PFNGLVERTEXATTRIBDIVISORPROC     glVertexAttribDivisor;
static PFNGLBUFFERSUBDATAPROC           glBufferSubData;
static PFNGLDRAWARRAYSINSTANCEDPROC     glDrawArraysInstanced;
static PFNGLBINDFRAMEBUFFERPROC         glBindFramebuffer;
static PFNGLCHECKFRAMEBUFFERSTATUSPROC  glCheckFramebufferStatus;
static PFNGLGENFRAMEBUFFERSPROC         glGenFramebuffers;
static PFNGLFRAMEBUFFERTEXTURE2DPROC    glFramebufferTexture2D;
static PFNGLDRAWBUFFERSPROC             glDrawBuffers;
static PFNGLDELETEFRAMEBUFFERSPROC      glDeleteFramebuffers;
static PFNGLBLENDFUNCIPROC              glBlendFunci;
static PFNGLCLEARBUFFERFVPROC           glClearBufferfv;
static PFNGLSHADERSOURCEPROC            glShaderSource;
static PFNGLCOMPILESHADERPROC           glCompileShader;
static PFNGLGETSHADERIVPROC             glGetShaderiv;
static PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
static PFNGLATTACHSHADERPROC            glAttachShader;
static PFNGLLINKPROGRAMPROC             glLinkProgram;
static PFNGLVALIDATEPROGRAMPROC         glValidateProgram;
static PFNGLGETPROGRAMIVPROC            glGetProgramiv;
static PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
static PFNGLGENBUFFERSPROC              glGenBuffers;
static PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
static PFNGLGETATTRIBLOCATIONPROC       glGetAttribLocation;
static PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
static PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
static PFNGLBINDBUFFERPROC              glBindBuffer;
static PFNGLBINDBUFFERBASEPROC          glBindBufferBase;
static PFNGLBUFFERDATAPROC              glBufferData;
static PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
static PFNGLUSEPROGRAMPROC              glUseProgram;
static PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
static PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
static PFNGLDELETEPROGRAMPROC           glDeleteProgram;
static PFNGLDETACHSHADERPROC            glDetachShader;
static PFNGLDELETESHADERPROC            glDeleteShader;
static PFNGLDRAWELEMENTSINSTANCEDPROC   glDrawElementsInstanced;
static PFNGLGENERATEMIPMAPPROC          glGenerateMipmap;
static PFNGLDEBUGMESSAGECALLBACKPROC    glDebugMessageCallback;

void LoadOpenGLFunctions()
{
    LoadGLFunction(PFNGLCREATEPROGRAMPROC, glCreateProgram);
    LoadGLFunction(PFNGLCREATESHADERPROC, glCreateShader);
    LoadGLFunction(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
    LoadGLFunction(PFNGLUNIFORM1FPROC, glUniform1f);
    LoadGLFunction(PFNGLUNIFORM2FVPROC, glUniform2fv);
    LoadGLFunction(PFNGLUNIFORM3FVPROC, glUniform3fv);
    LoadGLFunction(PFNGLUNIFORM1IPROC, glUniform1i);
    LoadGLFunction(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
    LoadGLFunction(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor);
    LoadGLFunction(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
    LoadGLFunction(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced);
    LoadGLFunction(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
    LoadGLFunction(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
    LoadGLFunction(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
    LoadGLFunction(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
    LoadGLFunction(PFNGLDRAWBUFFERSPROC, glDrawBuffers);
    LoadGLFunction(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers);
    LoadGLFunction(PFNGLBLENDFUNCIPROC, glBlendFunci);
    LoadGLFunction(PFNGLCLEARBUFFERFVPROC, glClearBufferfv);
    LoadGLFunction(PFNGLSHADERSOURCEPROC, glShaderSource);
    LoadGLFunction(PFNGLCOMPILESHADERPROC, glCompileShader);
    LoadGLFunction(PFNGLGETSHADERIVPROC, glGetShaderiv);
    LoadGLFunction(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
    LoadGLFunction(PFNGLATTACHSHADERPROC, glAttachShader);
    LoadGLFunction(PFNGLLINKPROGRAMPROC, glLinkProgram);
    LoadGLFunction(PFNGLVALIDATEPROGRAMPROC, glValidateProgram);
    LoadGLFunction(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
    LoadGLFunction(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
    LoadGLFunction(PFNGLGENBUFFERSPROC, glGenBuffers);
    LoadGLFunction(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
    LoadGLFunction(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation);
    LoadGLFunction(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
    LoadGLFunction(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
    LoadGLFunction(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
    LoadGLFunction(PFNGLBINDBUFFERPROC, glBindBuffer);
    LoadGLFunction(PFNGLBINDBUFFERBASEPROC, glBindBufferBase);
    LoadGLFunction(PFNGLBUFFERDATAPROC, glBufferData);
    LoadGLFunction(PFNGLGETVERTEXATTRIBPOINTERVPROC, glGetVertexAttribPointerv);
    LoadGLFunction(PFNGLUSEPROGRAMPROC, glUseProgram);
    LoadGLFunction(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
    LoadGLFunction(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
    LoadGLFunction(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
    LoadGLFunction(PFNGLDETACHSHADERPROC, glDetachShader);
    LoadGLFunction(PFNGLDELETESHADERPROC, glDeleteShader);
    LoadGLFunction(PFNGLDRAWELEMENTSINSTANCEDPROC, glDrawElementsInstanced);
    LoadGLFunction(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap);
    LoadGLFunction(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback);
}

void GLDebugCallback(GLenum source, GLenum type, GLuint id,
                     GLenum severity, GLsizei length, const GLchar* message,
                     const void* userParam)
{
    if (severity == GL_DEBUG_SEVERITY_LOW ||
        severity == GL_DEBUG_SEVERITY_MEDIUM ||
        severity == GL_DEBUG_SEVERITY_HIGH)
    {
        ASSERT_MSG(false, "OpenGL error: %s", message);
    }
    else
    {
        LOG_TRACE((char*) message);
    }
}
#pragma endregion
static bool       s_isInitialized = false;
static EGLDisplay s_eglDisplay    = NULL;
static i32        s_gfxCount      = 0;
#pragma endregion

bool Graphics_CreateGraphics(DROP_GfxHandle* pHandle, const DROP_GfxInitProps* pProps)
{
    *pHandle = NULL;

    if (!s_isInitialized)
    {

        s_eglDisplay = eglGetDisplay((EGLNativeDisplayType) pProps->wndHandle->pDisplay);
        if (!s_eglDisplay)
        {
            ASSERT_MSG(false, "Failed to get EGL Display.");
            return false;
        }
        if (!eglInitialize(s_eglDisplay, NULL, NULL))
        {
            ASSERT_MSG(false, "Failed to initialize EGL.");
            return false;
        }

        const char* ext = eglQueryString(s_eglDisplay, EGL_EXTENSIONS);
        if (!ext || !strstr(ext, "EGL_KHR_gl_colorspace"))
        {
            ASSERT_MSG(false, "EGL_KHR_gl_colorspace not supported.");
            return false;
        }

        DROP_WndInitProps dummyProps;
        dummyProps.title  = "Dummy";
        dummyProps.width  = 1280;
        dummyProps.height = 720;

        DROP_WndHandle dummyWndHandle;

        if (!Platform_CreateWindow(&dummyWndHandle, &dummyProps))
        {
            ASSERT(false);
            return false;
        }

        const EGLint configAttribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE};

        EGLConfig config;
        EGLint    numConfigs;
        if (!eglChooseConfig(s_eglDisplay, configAttribs, &config, 1, &numConfigs))
        {
            ASSERT_MSG(false, "Failed to choose EGL Config.");
            Platform_DestroyWindow(&dummyWndHandle);
            return false;
        }

        EGLSurface dummySurface = eglCreateWindowSurface(s_eglDisplay, config, dummyWndHandle->window, NULL);
        if (dummySurface == EGL_NO_SURFACE)
        {
            ASSERT(false);
            Platform_DestroyWindow(&dummyWndHandle);
            return false;
        }

        if (!eglBindAPI(EGL_OPENGL_API))
        {
            ASSERT_MSG(false, "Failed to bind OpenGL API.");
            eglDestroySurface(s_eglDisplay, dummySurface);
            Platform_DestroyWindow(&dummyWndHandle);
            return false;
        }

        const EGLint contextAttribs[] = {
            EGL_CONTEXT_MAJOR_VERSION_KHR, 4,
            EGL_CONTEXT_MINOR_VERSION_KHR, 3,
            EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
            EGL_CONTEXT_FLAGS_KHR, EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR,
            EGL_NONE};

        EGLContext dummyContext = eglCreateContext(s_eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);
        if (dummyContext == EGL_NO_CONTEXT)
        {
            ASSERT(false);
            eglDestroySurface(s_eglDisplay, dummySurface);
            Platform_DestroyWindow(&dummyWndHandle);
            return false;
        }

        if (!eglMakeCurrent(s_eglDisplay, dummySurface, dummySurface, dummyContext))
        {
            ASSERT(false);
            eglDestroyContext(s_eglDisplay, dummyContext);
            eglDestroySurface(s_eglDisplay, dummySurface);
            Platform_DestroyWindow(&dummyWndHandle);
            return false;
        }

        LoadOpenGLFunctions();

        glDebugMessageCallback(&GLDebugCallback, NULL);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glEnable(GL_DEBUG_OUTPUT);

        eglMakeCurrent(s_eglDisplay, EGL_NO_CONTEXT, EGL_NO_CONTEXT, EGL_NO_CONTEXT);
        eglDestroyContext(s_eglDisplay, dummyContext);
        eglDestroySurface(s_eglDisplay, dummySurface);

        Platform_DestroyWindow(&dummyWndHandle);

        s_isInitialized = true;
    }

    DROP_WndHandle wndHandle = pProps->wndHandle;

    const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE};

    EGLConfig config;
    EGLint    numConfigs;
    if (!eglChooseConfig(s_eglDisplay, configAttribs, &config, 1, &numConfigs) || !numConfigs)
    {
        ASSERT_MSG(false, "Failed to get EGL Config.");
        return false;
    }

    const EGLint surfaceAttribs[] = {
        EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR,
        EGL_NONE};

    EGLSurface surface = eglCreateWindowSurface(s_eglDisplay, config, wndHandle->window, surfaceAttribs);
    if (surface == EGL_NO_SURFACE)
    {
        ASSERT_MSG(false, "Failed to created EGL Surface.");
        return false;
    }

    const EGLint contextAttribs[] = {
        EGL_CONTEXT_MAJOR_VERSION_KHR, 4,
        EGL_CONTEXT_MINOR_VERSION_KHR, 3,
        EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
        EGL_CONTEXT_FLAGS_KHR, EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR,
        EGL_NONE};

    EGLContext context = eglCreateContext(s_eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT)
    {
        ASSERT_MSG(false, "Failed to create EGL Context.");
        eglDestroySurface(s_eglDisplay, surface);
        return false;
    }

    if (!eglMakeCurrent(s_eglDisplay, surface, surface, context))
    {
        ASSERT(false);
        eglDestroyContext(s_eglDisplay, context);
        eglDestroySurface(s_eglDisplay, surface);
        return false;
    }

    DROP_GfxHandle handle = ALLOC_SINGLE(_GfxHandle);
    ASSERT(handle);
    handle->surface = surface;
    handle->context = context;
    *pHandle        = handle;

    XWindowAttributes wAttribs;
    XGetWindowAttributes(wndHandle->pDisplay, wndHandle->window, &wAttribs);
    glViewport(0, 0, wAttribs.width, wAttribs.height);
    glEnable(GL_FRAMEBUFFER_SRGB);

    ++s_gfxCount;

    return true;
}

void Graphics_DestroyGraphics(DROP_GfxHandle* pHandle)
{
    ASSERT_MSG(pHandle && *pHandle, "Handle is NULL.");
    DROP_GfxHandle handle = *pHandle;

    if (handle)
    {
        eglMakeCurrent(s_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(s_eglDisplay, handle->context);
        eglDestroySurface(s_eglDisplay, handle->surface);

        handle->surface = NULL;
        handle->context = NULL;

        FREE(handle);
        handle = NULL;

        --s_gfxCount;
    }

    *pHandle = NULL;

    if (s_gfxCount == 0)
    {
        eglTerminate(s_eglDisplay);
        s_eglDisplay    = NULL;
        s_isInitialized = false;
    }
}

bool Graphics_SwapBuffers(DROP_GfxHandle handle)
{
    return eglSwapBuffers(s_eglDisplay, handle->surface);
}

bool Graphics_MakeCurrent(DROP_GfxHandle handle)
{
    return eglMakeCurrent(s_eglDisplay, handle->surface, handle->surface, handle->context);
}

void Graphics_ClearColor(f32 r, f32 g, f32 b, f32 a)
{
    glClearColor(r, g, b, a);
}

void Graphics_ClearDepth(f32 depth)
{
    glClearDepth(depth);
}

void Graphics_Clear(DROP_GFX_CLEAR flags)
{
    u32 clearFlags = DROP_GFX_CLEAR_NONE;

    if (flags & DROP_GFX_CLEAR_COLOR)
        clearFlags |= GL_COLOR_BUFFER_BIT;
    if (flags & DROP_GFX_CLEAR_DEPTH)
        clearFlags |= GL_DEPTH_BUFFER_BIT;

    glClear(clearFlags);
}
