#pragma once

#include "Graphics/Graphics.h"
#include "Platform/Windows/Windows_Window.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

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

    // Win32.
    HWND hwnd;
    HDC  hdc;

    // Cache for clearing.
    VkClearColorValue clearColor;
    f32               clearDepth;
    u32               clearFlags;

    // Depth buffer.
    VkImage        depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView    depthImageView;
    VkFormat       depthFormat;

    // Current state.
    u32 currentImageIndex;
} _GfxHandle;