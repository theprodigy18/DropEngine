#pragma once

#include "Graphics/Graphics.h"
#include "Platform/Linux/Linux_Window.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_xlib.h>

typedef struct _GfxHandle
{
    // Vulkan core objects.
    VkInstance       instance;
    VkDevice         device;
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR     surface;

    // Swapchain.
    VkSwapchainKHR swapchain;
    VkImage*       swapchainImages;
    VkImageView*   swapchainImageViews;
    u32            swapchainImageCount;
    VkFormat       swapchainFormat;
    VkExtent2D     swapchainExtent;

    // Command objects.
    VkCommandPool   commandPool;
    VkCommandBuffer commandBuffer;

    // Synchronization.
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence     inFlightFence;

    // Queue info.
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    u32     graphicsFamilyIndex;
    u32     presentFamilyIndex;

    // X11.
    Display* pDisplay;
    Window   window;

    // Cache for clearing.
    VkClearColorValue clearColor;
    f32               clearDepth;
    DROP_GFX_CLEAR    clearFlags;

    // Depth buffer.
    VkImage        depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView    depthImageView;
    VkFormat       depthFormat;

    // Current state.
    u32 currentImageIndex;

} _GfxHandle;
