#include "pch.h"
#include "Graphics/OpenGL/Windows/Windows_OpenGL_Graphics.h"

#include <string.h>

#pragma region INTERNAL
static HMODULE g_openglDLL = NULL;

#define LOAD_GL_FUNCTION(type, name)                      \
    name = (type) wglGetProcAddress(#name);               \
    if (!name)                                            \
    {                                                     \
        name = (type) GetProcAddress(g_openglDLL, #name); \
    }                                                     \
    ASSERT(name);

#pragma region OpenGL Functions Pointers
static PFNGLCREATEPROGRAMPROC           glCreateProgram;
static PFNGLCREATESHADERPROC            glCreateShader;
static PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
static PFNGLUNIFORM1FPROC               glUniform1f;
static PFNGLUNIFORM2FVPROC              glUniform2fv;
static PFNGLUNIFORM3FVPROC              glUniform3fv;
static PFNGLUNIFORM1IPROC               glUniform1i;
static PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;
static PFNGLVERTEXATTRIBDIVISORPROC     glVertexAttribDivisor;
static PFNGLACTIVETEXTUREPROC           glActiveTexture;
static PFNGLBUFFERSUBDATAPROC           glBufferSubData;
static PFNGLDRAWARRAYSINSTANCEDPROC     glDrawArraysInstanced;
static PFNGLBINDFRAMEBUFFERPROC         glBindFramebuffer;
static PFNGLCHECKFRAMEBUFFERSTATUSPROC  glCheckFramebufferStatus;
static PFNGLGENFRAMEBUFFERSPROC         glGenFramebuffers;
static PFNGLFRAMEBUFFERTEXTURE2DPROC    glFramebufferTexture2D;
static PFNGLDRAWBUFFERSPROC             glDrawBuffers;
static PFNGLDELETEFRAMEBUFFERSPROC      glDeleteFramebuffers;
static PFNGLBLENDFUNCIPROC              glBlendFunci;
static PFNGLBLENDEQUATIONPROC           glBlendEquation;
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
#pragma endregion

void LoadOpenGLFunctions()
{
    g_openglDLL = LoadLibraryA("opengl32.dll");
    ASSERT(g_openglDLL);

    LOAD_GL_FUNCTION(PFNGLCREATEPROGRAMPROC, glCreateProgram);
    LOAD_GL_FUNCTION(PFNGLCREATESHADERPROC, glCreateShader);
    LOAD_GL_FUNCTION(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
    LOAD_GL_FUNCTION(PFNGLUNIFORM1FPROC, glUniform1f);
    LOAD_GL_FUNCTION(PFNGLUNIFORM2FVPROC, glUniform2fv);
    LOAD_GL_FUNCTION(PFNGLUNIFORM3FVPROC, glUniform3fv);
    LOAD_GL_FUNCTION(PFNGLUNIFORM1IPROC, glUniform1i);
    LOAD_GL_FUNCTION(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
    LOAD_GL_FUNCTION(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor);
    LOAD_GL_FUNCTION(PFNGLACTIVETEXTUREPROC, glActiveTexture);
    LOAD_GL_FUNCTION(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
    LOAD_GL_FUNCTION(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced);
    LOAD_GL_FUNCTION(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
    LOAD_GL_FUNCTION(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
    LOAD_GL_FUNCTION(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
    LOAD_GL_FUNCTION(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
    LOAD_GL_FUNCTION(PFNGLDRAWBUFFERSPROC, glDrawBuffers);
    LOAD_GL_FUNCTION(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers);
    LOAD_GL_FUNCTION(PFNGLBLENDFUNCIPROC, glBlendFunci);
    LOAD_GL_FUNCTION(PFNGLBLENDEQUATIONPROC, glBlendEquation);
    LOAD_GL_FUNCTION(PFNGLCLEARBUFFERFVPROC, glClearBufferfv);
    LOAD_GL_FUNCTION(PFNGLSHADERSOURCEPROC, glShaderSource);
    LOAD_GL_FUNCTION(PFNGLCOMPILESHADERPROC, glCompileShader);
    LOAD_GL_FUNCTION(PFNGLGETSHADERIVPROC, glGetShaderiv);
    LOAD_GL_FUNCTION(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
    LOAD_GL_FUNCTION(PFNGLATTACHSHADERPROC, glAttachShader);
    LOAD_GL_FUNCTION(PFNGLLINKPROGRAMPROC, glLinkProgram);
    LOAD_GL_FUNCTION(PFNGLVALIDATEPROGRAMPROC, glValidateProgram);
    LOAD_GL_FUNCTION(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
    LOAD_GL_FUNCTION(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
    LOAD_GL_FUNCTION(PFNGLGENBUFFERSPROC, glGenBuffers);
    LOAD_GL_FUNCTION(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
    LOAD_GL_FUNCTION(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation);
    LOAD_GL_FUNCTION(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
    LOAD_GL_FUNCTION(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
    LOAD_GL_FUNCTION(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
    LOAD_GL_FUNCTION(PFNGLBINDBUFFERPROC, glBindBuffer);
    LOAD_GL_FUNCTION(PFNGLBINDBUFFERBASEPROC, glBindBufferBase);
    LOAD_GL_FUNCTION(PFNGLBUFFERDATAPROC, glBufferData);
    LOAD_GL_FUNCTION(PFNGLGETVERTEXATTRIBPOINTERVPROC, glGetVertexAttribPointerv);
    LOAD_GL_FUNCTION(PFNGLUSEPROGRAMPROC, glUseProgram);
    LOAD_GL_FUNCTION(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
    LOAD_GL_FUNCTION(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
    LOAD_GL_FUNCTION(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
    LOAD_GL_FUNCTION(PFNGLDETACHSHADERPROC, glDetachShader);
    LOAD_GL_FUNCTION(PFNGLDELETESHADERPROC, glDeleteShader);
    LOAD_GL_FUNCTION(PFNGLDRAWELEMENTSINSTANCEDPROC, glDrawElementsInstanced);
    LOAD_GL_FUNCTION(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap);
    LOAD_GL_FUNCTION(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback);
}

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB    = NULL;
PFNWGLGETEXTENSIONSSTRINGARBPROC  wglGetExtensionsStringARB  = NULL;

void CALLBACK GLDebugCallback(GLenum source, GLenum type, GLuint id,
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

static bool s_isInitialized = false;
static i32  s_gfxCount      = 0;
#pragma endregion

bool Graphics_CreateGraphics(GfxHandle* pHandle, const GfxInitProps* pProps)
{
	*pHandle = NULL;

	if (!s_isInitialized)
	{
        WndInitProps dummyProps;
        dummyProps.title = "Dummy";
        dummyProps.width = 1280;
		dummyProps.height = 720;
	
		WndHandle dummyWndHandle = NULL;
		if (!Platform_CreateWindow(&dummyWndHandle, &dummyProps))
		{
			ASSERT(false);
			return false;
		}

		PIXELFORMATDESCRIPTOR pfd;
        pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion     = 1;
        pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType   = PFD_TYPE_RGBA;
        pfd.cColorBits   = 32;
        pfd.cDepthBits   = 24;
        pfd.cStencilBits = 8;

		i32 pixelFormat = ChoosePixelFormat(dummyWndHandle->hdc, &pfd);
		if (!pixelFormat)
		{
			ASSERT(false);
			Platform_DestroyWindow(&dummyWndHandle);
			return false;
		}

		if (!SetPixelFormat(dummyWndHandle->hdc, pixelFormat, &pfd))
		{
			ASSERT(false);
            Platform_DestroyWindow(&dummyWndHandle);
			return false;
		}

		HGLRC dummyContext = wglCreateContext(dummyWndHandle->hdc);
		if (!dummyContext)
		{
			ASSERT(false);
            Platform_DestroyWindow(&dummyWndHandle);
			return false;
		}

		if (!wglMakeCurrent(dummyWndHandle->hdc, dummyContext))
        {
            ASSERT(false);
			wglDeleteContext(dummyContext);
            Platform_DestroyWindow(&dummyWndHandle);
            return false;
        }

		LoadOpenGLFunctions();

		LOAD_GL_FUNCTION(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);
		LOAD_GL_FUNCTION(PFNWGLCHOOSEPIXELFORMATARBPROC,    wglChoosePixelFormatARB);
		LOAD_GL_FUNCTION(PFNWGLGETEXTENSIONSSTRINGARBPROC,  wglGetExtensionsStringARB);
		if (!wglCreateContextAttribsARB || !wglChoosePixelFormatARB || !wglGetExtensionsStringARB)
		{
			ASSERT(false);
			wglDeleteContext(dummyContext);
			Platform_DestroyWindow(&dummyWndHandle);
			return false;
		}

		const char* wglExt = wglGetExtensionsStringARB(dummyWndHandle->hdc);
		if (!wglExt || !strstr(wglExt, "WGL_ARB_framebuffer_sRGB"))
		{
			ASSERT_MSG(false, "WGL_ARB_framebuffer_sRGB is not supported.");
			wglDeleteContext(dummyContext);
			Platform_DestroyWindow(&dummyWndHandle);
			return false;
		}

		glDebugMessageCallback(GLDebugCallback, NULL);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glEnable(GL_DEBUG_OUTPUT);

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(dummyContext);

		Platform_DestroyWindow(&dummyWndHandle);

		s_isInitialized = true;
	}

	WndHandle wndHandle = pProps->wndHandle;
    ASSERT_MSG(wndHandle, "Window handle is NULL.");

	const i32 pixelAttribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
		WGL_RED_BITS_ARB, 8,
		WGL_GREEN_BITS_ARB, 8,
		WGL_BLUE_BITS_ARB, 8,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
        0}; // Last entry must be 0 to terminate the list.

    u32 numPixelFormats = 0;
    i32 pixelformat     = 0;
    if (!wglChoosePixelFormatARB(wndHandle->hdc, pixelAttribs, NULL, 1, &pixelformat, &numPixelFormats))
    {
        ASSERT_MSG(false, "Failed to choose pixel format.");
        return false;
    }

	PIXELFORMATDESCRIPTOR pfd;
    if (!DescribePixelFormat(wndHandle->hdc, pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd))
    {
        ASSERT_MSG(false, "Failed to describe pixel format.");
        return false;
    }

    if (!SetPixelFormat(wndHandle->hdc, pixelformat, &pfd))
    {
        ASSERT_MSG(false, "Failed to set pixel format.");
        return false;
    }

	const i32 contextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
        0}; // Last entry must be 0 to terminate the list.

    HGLRC context = wglCreateContextAttribsARB(wndHandle->hdc, NULL, contextAttribs);
    if (!context)
    {
        ASSERT_MSG(false, "Failed to create OpenGL context.");
        return false;
    }

    if (!wglMakeCurrent(wndHandle->hdc, context))
    {
        ASSERT_MSG(false, "Failed to make OpenGL context current.");
		wglDeleteContext(context);
        return false;
    }


	GfxHandle handle = ALLOC_SINGLE(_GfxHandle);
    ASSERT(handle);
    handle->hdc     = wndHandle->hdc;
    handle->context = context;
    *pHandle = handle;

    RECT rc;
    GetClientRect(wndHandle->hwnd, &rc);

    glViewport(0, 0, rc.right - rc.left, rc.bottom - rc.top);
	glEnable(GL_FRAMEBUFFER_SRGB);

	++s_gfxCount;

	return true;
}

void Graphics_DestroyGraphics(GfxHandle* pHandle)
{
    ASSERT_MSG(pHandle && *pHandle, "Handle is NULL.");
	GfxHandle handle = *pHandle;

	if (handle)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(handle->context);

		handle->context = NULL;
		handle->hdc     = NULL;
		
		FREE(handle);
		handle = NULL;

		--s_gfxCount;
	}

	*pHandle = NULL;

	if (s_gfxCount == 0)
	{
		FreeLibrary(g_openglDLL);
		g_openglDLL = NULL;
		s_isInitialized = false;
	}
}

bool Graphics_SwapBuffers(GfxHandle handle)
{
	return SwapBuffers(handle->hdc);
}

bool Graphics_MakeCurrent(GfxHandle handle)
{
	return wglMakeCurrent(handle->hdc, handle->context);
}

void Graphics_ClearColor(f32 r, f32 g, f32 b, f32 a)
{
	glClearColor(r, g, b, a);
}

void Graphics_ClearDepth(f32 depth)
{
	glClearDepth(depth);
}

void Graphics_Clear(u32 flags)
{
    u32 clearFlags = 0;
    if (flags & GFX_CLEAR_COLOR)
		clearFlags |= GL_COLOR_BUFFER_BIT;
	if (flags & GFX_CLEAR_DEPTH)
		clearFlags |= GL_DEPTH_BUFFER_BIT;

	glClear(clearFlags);
}