#include "pch.h"
#include "Graphics/DirectX11/Windows/Windows_DirectX11_Graphics.h"

#pragma region INTERNAL
static GfxHandle s_currentHandle = NULL;
#pragma endregion

bool Graphics_CreateGraphics(GfxHandle* pHandle, const GfxInitProps* pProps)
{
    *pHandle = NULL;

    WndHandle wndHandle = pProps->wndHandle;
    ASSERT_MSG(wndHandle && wndHandle->hwnd, "Invalid window handle.");

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapChainDesc.BufferCount                        = 1;
    swapChainDesc.BufferDesc.Width                   = 0;
    swapChainDesc.BufferDesc.Height                  = 0;
    swapChainDesc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow                       = wndHandle->hwnd;
    swapChainDesc.SampleDesc.Count                   = 1;
    swapChainDesc.SampleDesc.Quality                 = 0;
    swapChainDesc.Windowed                           = TRUE;
    swapChainDesc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags                              = 0;

    ID3D11Device*        pDevice        = NULL;
    ID3D11DeviceContext* pDeviceContext = NULL;
    IDXGISwapChain*      pSwapChain     = NULL;

    D3D_FEATURE_LEVEL featureLevel;

    UINT deviceFlags = 0;
#ifdef DEBUG
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // DEBUG

    if (FAILED(D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags,
            NULL, 0, D3D11_SDK_VERSION,
            &swapChainDesc, &pSwapChain, &pDevice, &featureLevel, &pDeviceContext)))
    {
        ASSERT_MSG(false, "Failed to create DirectX11 Device and SwapChain.");
        return false;
    }

#ifdef DEBUG
    ID3D11InfoQueue* pInfoQueue = NULL;
    if (SUCCEEDED(pDevice->lpVtbl->QueryInterface(pDevice, &IID_ID3D11InfoQueue, (void**) &pInfoQueue)))
    {
        pInfoQueue->lpVtbl->SetBreakOnSeverity(pInfoQueue, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->lpVtbl->SetBreakOnSeverity(pInfoQueue, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        pInfoQueue->lpVtbl->SetBreakOnSeverity(pInfoQueue, D3D11_MESSAGE_SEVERITY_WARNING, TRUE);
        pInfoQueue->lpVtbl->Release(pInfoQueue);
    }
#endif // DEBUG

    ID3D11Texture2D* pBackBuffer = NULL;
    if (FAILED(pSwapChain->lpVtbl->GetBuffer(pSwapChain, 0, &IID_ID3D11Texture2D, (void**) &pBackBuffer)))
    {
        ASSERT_MSG(false, "Failed to get backbuffer.");
        pDeviceContext->lpVtbl->Release(pDeviceContext);
        pSwapChain->lpVtbl->Release(pSwapChain);
        pDevice->lpVtbl->Release(pDevice);
        return false;
    }

    ID3D11RenderTargetView* pRenderTargetView = NULL;
    HRESULT                 hr                = pDevice->lpVtbl->CreateRenderTargetView(pDevice, (ID3D11Resource*) pBackBuffer, NULL, &pRenderTargetView);
    pBackBuffer->lpVtbl->Release(pBackBuffer);
    if (FAILED(hr))
    {
        ASSERT_MSG(false, "Failed to create render target view.");
        pDeviceContext->lpVtbl->Release(pDeviceContext);
        pSwapChain->lpVtbl->Release(pSwapChain);
        pDevice->lpVtbl->Release(pDevice);
        return false;
    }

    RECT rc;
    GetClientRect(wndHandle->hwnd, &rc);
    UINT width  = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
    textureDesc.Width              = width;
    textureDesc.Height             = height;
    textureDesc.MipLevels          = 1;
    textureDesc.ArraySize          = 1;
    textureDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    textureDesc.SampleDesc.Count   = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage              = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* pDepthStencilBuffer = NULL;
    hr                                   = pDevice->lpVtbl->CreateTexture2D(pDevice, &textureDesc, NULL, &pDepthStencilBuffer);
    if (FAILED(hr))
    {
        ASSERT_MSG(false, "Failed to create depth stencil buffer.");
        pRenderTargetView->lpVtbl->Release(pRenderTargetView);
        pDeviceContext->lpVtbl->Release(pDeviceContext);
        pSwapChain->lpVtbl->Release(pSwapChain);
        pDevice->lpVtbl->Release(pDevice);
        return false;
    }

    ID3D11DepthStencilView* pDepthStencilView = NULL;

    hr = pDevice->lpVtbl->CreateDepthStencilView(pDevice, (ID3D11Resource*) pDepthStencilBuffer, NULL, &pDepthStencilView);
    pDepthStencilBuffer->lpVtbl->Release(pDepthStencilBuffer);
    if (FAILED(hr))
    {
        ASSERT_MSG(false, "Failed to create depth stencil view.");
        pRenderTargetView->lpVtbl->Release(pRenderTargetView);
        pDeviceContext->lpVtbl->Release(pDeviceContext);
        pSwapChain->lpVtbl->Release(pSwapChain);
        pDevice->lpVtbl->Release(pDevice);
        return false;
    }

    pDeviceContext->lpVtbl->OMSetRenderTargets(pDeviceContext, 1, &pRenderTargetView, pDepthStencilView);

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width    = (FLOAT) width;
    viewport.Height   = (FLOAT) height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    pDeviceContext->lpVtbl->RSSetViewports(pDeviceContext, 1, &viewport);

    GfxHandle handle = ALLOC_SINGLE(_GfxHandle);
    ASSERT(handle);
    handle->pDeviceContext    = pDeviceContext;
    handle->pDevice           = pDevice;
    handle->pSwapChain        = pSwapChain;
    handle->pRenderTargetView = pRenderTargetView;
    handle->pDepthStencilView = pDepthStencilView;
    handle->clearColor[0]     = 0.0f;
    handle->clearColor[1]     = 0.0f;
    handle->clearColor[2]     = 0.0f;
    handle->clearColor[3]     = 1.0f;
    handle->clearDepth        = 1.0f;

    *pHandle = handle;

    s_currentHandle = handle;

    return true;
}

void Graphics_DestroyGraphics(GfxHandle* pHandle)
{
    ASSERT_MSG(pHandle && *pHandle, "Invalid graphics handle.");
    GfxHandle handle = *pHandle;

    if (handle)
    {
        handle->pDeviceContext->lpVtbl->ClearState(handle->pDeviceContext);
        handle->pDeviceContext->lpVtbl->Flush(handle->pDeviceContext);

        handle->pDepthStencilView->lpVtbl->Release(handle->pDepthStencilView);
        handle->pRenderTargetView->lpVtbl->Release(handle->pRenderTargetView);
        handle->pDeviceContext->lpVtbl->Release(handle->pDeviceContext);
        handle->pSwapChain->lpVtbl->Release(handle->pSwapChain);
        handle->pDevice->lpVtbl->Release(handle->pDevice);

        FREE(handle);

        *pHandle = NULL;
    }
}

bool Graphics_SwapBuffers(GfxHandle handle)
{
    return SUCCEEDED(handle->pSwapChain->lpVtbl->Present(handle->pSwapChain, 1, 0));
}

bool Graphics_MakeCurrent(GfxHandle handle)
{
    DEBUG_OP(if (!handle) return false);
    s_currentHandle = handle;
    return true;
}

void Graphics_ClearColor(f32 r, f32 g, f32 b, f32 a)
{
    ASSERT(s_currentHandle);
    s_currentHandle->clearColor[0] = r;
    s_currentHandle->clearColor[1] = g;
    s_currentHandle->clearColor[2] = b;
    s_currentHandle->clearColor[3] = a;
}

void Graphics_ClearDepth(f32 depth)
{
    ASSERT(s_currentHandle);
    s_currentHandle->clearDepth = depth;
}

void Graphics_Clear(u32 flags)
{
    ASSERT(s_currentHandle);

    if (flags & GFX_CLEAR_COLOR)
    {
        s_currentHandle->pDeviceContext->lpVtbl->ClearRenderTargetView(
            s_currentHandle->pDeviceContext,
            s_currentHandle->pRenderTargetView,
            s_currentHandle->clearColor);
    }

    if (flags & GFX_CLEAR_DEPTH)
    {
        s_currentHandle->pDeviceContext->lpVtbl->ClearDepthStencilView(
            s_currentHandle->pDeviceContext,
            s_currentHandle->pDepthStencilView,
            D3D11_CLEAR_DEPTH,
            s_currentHandle->clearDepth,
            0);
    }
}