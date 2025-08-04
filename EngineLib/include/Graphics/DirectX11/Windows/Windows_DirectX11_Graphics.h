#pragma once

#include "Graphics/Graphics.h"
#include "Platform/Windows/Windows_Window.h"

#include <d3d11.h>
#include <dxgi.h>

typedef struct _GfxHandle
{
    ID3D11Device*           pDevice;
    ID3D11DeviceContext*    pDeviceContext;
    IDXGISwapChain*         pSwapChain;
    ID3D11RenderTargetView* pRenderTargetView;
	ID3D11DepthStencilView* pDepthStencilView;

	f32 clearColor[4];
	f32 clearDepth;
} _GfxHandle;
