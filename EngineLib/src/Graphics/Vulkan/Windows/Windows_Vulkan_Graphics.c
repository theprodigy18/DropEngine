#include "pch.h"
#include "Graphics/Vulkan/Windows/Windows_Vulkan_Graphics.h"

#define QUEUE_LIMIT 32

#define VK_SAFE_DESTROY(fn, device, handle) \
    if ((handle) != VK_NULL_HANDLE)         \
    {                                       \
        fn(device, handle, NULL);           \
        handle = VK_NULL_HANDLE;            \
    }

#pragma region INTERNAL
PFN_vkCreateWin32SurfaceKHR pfn_vkCreateWin32SurfaceKHR = NULL;

static bool           s_isInitialized = false;
static HINSTANCE      s_hInstance     = NULL;
static DROP_GfxHandle s_currentHandle = NULL;
#pragma endregion

bool Graphics_CreateGraphics(DROP_GfxHandle* pHandle, const DROP_GfxInitProps* pProps)
{
    *pHandle = NULL;

    ASSERT_MSG(pProps && pProps->wndHandle, "Invalid window handle");
    HWND hwnd = pProps->wndHandle->hwnd;

    ASSERT_MSG(hwnd, "Invalid window handle");

    VkApplicationInfo appInfo = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = "DropEngine",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName        = "DropEngine",
        .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion         = VK_API_VERSION_1_2};

    const char* extensions[] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
    };

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo        = &appInfo,
        .enabledExtensionCount   = 2,
        .ppEnabledExtensionNames = extensions,
        .enabledLayerCount       = 0,
        .ppEnabledLayerNames     = NULL};

#ifdef DEBUG
    const char* layers[]                   = {"VK_LAYER_KHRONOS_validation"};
    instanceCreateInfo.enabledLayerCount   = 1;
    instanceCreateInfo.ppEnabledLayerNames = layers;
#endif // DEBUG

    VkInstance instance;
    VkResult   result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to create vulkan instance.");
        return false;
    }

    if (!s_isInitialized)
    {
        pfn_vkCreateWin32SurfaceKHR =
            (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");

        if (!pfn_vkCreateWin32SurfaceKHR)
        {
            ASSERT(false);
            vkDestroyInstance(instance, NULL);
            return false;
        }

        s_hInstance = GetModuleHandleW(NULL);
        ASSERT(s_hInstance);

        s_isInitialized = true;
    }

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
        .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = s_hInstance,
        .hwnd      = hwnd};

    VkSurfaceKHR surface;
    result = pfn_vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to create vulkan surface.");
        vkDestroyInstance(instance, NULL);
        return false;
    }

    u32 deviceCount = 0;
    result          = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if (result != VK_SUCCESS || deviceCount == 0)
    {
        ASSERT_MSG(false, "Failed to enumerate physical devices");
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    VkPhysicalDevice* devices = ALLOC_ARRAY(VkPhysicalDevice, deviceCount);
    result                    = vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to enumerate physical devices");
        FREE(devices);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    VkPhysicalDevice choosenPhysicalDevice      = VK_NULL_HANDLE;
    i32              choosenGraphicsFamilyIndex = -1;
    i32              choosenPresentFamilyIndex  = -1;

    for (u32 i = 0; i < deviceCount; ++i)
    {
        VkPhysicalDevice device = devices[i];

        // 1. Query queue families.
        u32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

        VkQueueFamilyProperties queueProps[QUEUE_LIMIT];
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueProps);

        // 2. Check all queue one by one and check for graphics and presentation support.
        i32 graphicsFamilyIndex = -1;
        int presentFamilyIndex  = -1;

        for (u32 j = 0; j < queueFamilyCount; ++j)
        {
            if (queueProps[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                graphicsFamilyIndex = j;

            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, j, surface, &presentSupport);

            if (presentSupport)
                presentFamilyIndex = j;

            if (graphicsFamilyIndex != -1 && presentFamilyIndex != -1)
                break;
        }

        // 3. If graphics and presentation support is valid then save the index.
        if (graphicsFamilyIndex != -1 && presentFamilyIndex != -1)
        {
            choosenPhysicalDevice      = device;
            choosenGraphicsFamilyIndex = graphicsFamilyIndex;
            choosenPresentFamilyIndex  = presentFamilyIndex;
            break;
        }
    }

    // Free devices array as we no longer need it.
    FREE(devices);

    if (choosenGraphicsFamilyIndex == -1 || choosenPresentFamilyIndex == -1)
    {
        ASSERT_MSG(false, "Failed to find a graphics and presentation queue family index.");
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    f32 queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfos[2];
    u32                     queueCreateInfoCount = 0;

    if (choosenGraphicsFamilyIndex == choosenPresentFamilyIndex)
    {
        queueCreateInfos[0] = (VkDeviceQueueCreateInfo) {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = choosenGraphicsFamilyIndex,
            .queueCount       = 1,
            .pQueuePriorities = &queuePriority};

        queueCreateInfoCount = 1;
    }
    else
    {
        queueCreateInfos[0] = (VkDeviceQueueCreateInfo) {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = choosenGraphicsFamilyIndex,
            .queueCount       = 1,
            .pQueuePriorities = &queuePriority};

        queueCreateInfos[1] = (VkDeviceQueueCreateInfo) {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = choosenPresentFamilyIndex,
            .queueCount       = 1,
            .pQueuePriorities = &queuePriority};

        queueCreateInfoCount = 2;
    }

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos       = queueCreateInfos,
        .queueCreateInfoCount    = queueCreateInfoCount,
        .enabledExtensionCount   = 1,
        .ppEnabledExtensionNames = (const char*[]) {
            "VK_KHR_swapchain"},
        .pEnabledFeatures = NULL};

    VkDevice device;
    result = vkCreateDevice(choosenPhysicalDevice, &deviceCreateInfo, NULL, &device);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to create logical device");
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    // Queue handle.
    VkQueue graphicsQueue, presentQueue;
    vkGetDeviceQueue(device, choosenGraphicsFamilyIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(device, choosenPresentFamilyIndex, 0, &presentQueue);

    // CREATING SWAPCHAIN.
    // 1. Query surface capabilities.
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(choosenPhysicalDevice, surface, &surfaceCapabilities);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to query surface capabilities");
        vkDestroyDevice(device, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    // 2. Query surface formats (RGBA 8*4).
    u32 formatCount = 0;

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(choosenPhysicalDevice, surface, &formatCount, NULL);
    if (result != VK_SUCCESS || formatCount == 0)
    {
        ASSERT_MSG(false, "Failed to query surface formats");
        vkDestroyDevice(device, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    VkSurfaceFormatKHR* surfaceFormats = ALLOC_ARRAY(VkSurfaceFormatKHR, formatCount);
    result                             = vkGetPhysicalDeviceSurfaceFormatsKHR(choosenPhysicalDevice, surface, &formatCount, surfaceFormats);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to query surface formats");
        FREE(surfaceFormats);
        vkDestroyDevice(device, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    VkSurfaceFormatKHR choosenFormat = surfaceFormats[0];
    for (u32 i = 0; i < formatCount; ++i)
    {
        if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB)
        {
            choosenFormat = surfaceFormats[i];
            break;
        }
    }

    // Free surface formats array as we no longer need it.
    FREE(surfaceFormats);

    // 3. Get window extend.
    RECT rc;
    GetClientRect(hwnd, &rc);

    VkExtent2D extent = {rc.right - rc.left, rc.bottom - rc.top};

    // 4. Create swapchain (double buffer).
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface          = surface,
        .minImageCount    = surfaceCapabilities.minImageCount,
        .imageFormat      = choosenFormat.format,
        .imageExtent      = extent,
        .imageArrayLayers = 1,
        .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .presentMode      = VK_PRESENT_MODE_FIFO_KHR, // VSync.
        .clipped          = VK_TRUE,
        .preTransform     = surfaceCapabilities.currentTransform,
        .compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .oldSwapchain     = VK_NULL_HANDLE,
        .imageColorSpace  = choosenFormat.colorSpace};

    VkSwapchainKHR swapchain;
    result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to create swapchain");
        vkDestroyDevice(device, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    // 5. Get swapchain images.
    u32 swapchainImageCount = 0;
    result                  = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, NULL);
    if (result != VK_SUCCESS || swapchainImageCount == 0)
    {
        ASSERT_MSG(false, "Failed to get swapchain images");
        vkDestroySwapchainKHR(device, swapchain, NULL);
        vkDestroyDevice(device, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    VkImage* swapchainImages = ALLOC_ARRAY(VkImage, swapchainImageCount);
    result                   = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to get swapchain images");
        FREE(swapchainImages);
        vkDestroySwapchainKHR(device, swapchain, NULL);
        vkDestroyDevice(device, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    // 6. Create image views.
    VkImageView* swapchainImageViews = ALLOC_ARRAY(VkImageView, swapchainImageCount);
    for (u32 i = 0; i < swapchainImageCount; ++i)
    {
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image            = swapchainImages[i],
            .viewType         = VK_IMAGE_VIEW_TYPE_2D,
            .format           = choosenFormat.format,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = 1,
                .layerCount = 1}};

        result = vkCreateImageView(device, &imageViewCreateInfo, NULL, &swapchainImageViews[i]);
        if (result != VK_SUCCESS)
        {
            ASSERT_MSG(false, "Failed to create image view");
            for (u32 j = 0; j < i; ++j)
                vkDestroyImageView(device, swapchainImageViews[j], NULL);
            FREE(swapchainImageViews);
            FREE(swapchainImages);
            vkDestroySwapchainKHR(device, swapchain, NULL);
            vkDestroyDevice(device, NULL);
            vkDestroySurfaceKHR(instance, surface, NULL);
            vkDestroyInstance(instance, NULL);
            return false;
        }
    }

    // 6.5 CREATE DEPTH BUFFER.
    VkFormat depthFormats[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM};

    VkFormat choosenDepthFormat = VK_FORMAT_UNDEFINED;
    for (u32 i = 0; i < sizeof(depthFormats) / sizeof(depthFormats[0]); ++i)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(choosenPhysicalDevice, depthFormats[i], &props);

        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            choosenDepthFormat = depthFormats[i];
            break;
        }
    }

    VkImage        depthImage       = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
    VkImageView    depthImageView   = VK_NULL_HANDLE;

    if (choosenDepthFormat != VK_FORMAT_UNDEFINED)
    {
        // Creating depth image.
        VkImageCreateInfo depthImageInfo = {
            .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType     = VK_IMAGE_TYPE_2D,
            .format        = choosenDepthFormat,
            .extent        = {extent.width, extent.height, 1},
            .mipLevels     = 1,
            .arrayLayers   = 1,
            .samples       = VK_SAMPLE_COUNT_1_BIT,
            .tiling        = VK_IMAGE_TILING_OPTIMAL,
            .usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

        result = vkCreateImage(device, &depthImageInfo, NULL, &depthImage);
        if (result != VK_SUCCESS)
        {
            ASSERT_MSG(false, "Failed to create depth image.");
            for (u32 i = 0; i < swapchainImageCount; ++i)
                vkDestroyImageView(device, swapchainImageViews[i], NULL);
            FREE(swapchainImageViews);
            FREE(swapchainImages);
            vkDestroySwapchainKHR(device, swapchain, NULL);
            vkDestroyDevice(device, NULL);
            vkDestroySurfaceKHR(instance, surface, NULL);
            vkDestroyInstance(instance, NULL);
            return false;
        }

        // Get memory requirements.
        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements(device, depthImage, &memReqs);

        // Get memory properties.
        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(choosenPhysicalDevice, &memProps);

        // Find suitable memory type.
        u32 memoryTypeIndex = UINT32_MAX;
        for (u32 i = 0; i < memProps.memoryTypeCount; ++i)
        {
            if ((memReqs.memoryTypeBits & (1 << i)) &&
                (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
            {
                memoryTypeIndex = i;
                break;
            }
        }

        if (memoryTypeIndex == UINT32_MAX)
        {
            ASSERT_MSG(false, "Failed to find suitable memory type for depth buffer.");
            vkDestroyImage(device, depthImage, NULL);
            for (u32 i = 0; i < swapchainImageCount; ++i)
                vkDestroyImageView(device, swapchainImageViews[i], NULL);
            FREE(swapchainImageViews);
            FREE(swapchainImages);
            vkDestroySwapchainKHR(device, swapchain, NULL);
            vkDestroyDevice(device, NULL);
            vkDestroySurfaceKHR(instance, surface, NULL);
            vkDestroyInstance(instance, NULL);
            return false;
        }

        // Allocate memory.
        VkMemoryAllocateInfo allocInfo = {
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize  = memReqs.size,
            .memoryTypeIndex = memoryTypeIndex};

        result = vkAllocateMemory(device, &allocInfo, NULL, &depthImageMemory);
        if (result != VK_SUCCESS)
        {
            ASSERT_MSG(false, "Failed to allocate depth image memory.");
            vkDestroyImage(device, depthImage, NULL);
            for (u32 i = 0; i < swapchainImageCount; ++i)
                vkDestroyImageView(device, swapchainImageViews[i], NULL);
            FREE(swapchainImageViews);
            FREE(swapchainImages);
            vkDestroySwapchainKHR(device, swapchain, NULL);
            vkDestroyDevice(device, NULL);
            vkDestroySurfaceKHR(instance, surface, NULL);
            vkDestroyInstance(instance, NULL);
            return false;
        }

        // Bind memory to image.
        result = vkBindImageMemory(device, depthImage, depthImageMemory, 0);
        if (result != VK_SUCCESS)
        {
            ASSERT_MSG(false, "Failed to bind depth image memory.");
            vkFreeMemory(device, depthImageMemory, NULL);
            vkDestroyImage(device, depthImage, NULL);
            for (u32 i = 0; i < swapchainImageCount; ++i)
                vkDestroyImageView(device, swapchainImageViews[i], NULL);
            FREE(swapchainImageViews);
            FREE(swapchainImages);
            vkDestroySwapchainKHR(device, swapchain, NULL);
            vkDestroyDevice(device, NULL);
            vkDestroySurfaceKHR(instance, surface, NULL);
            vkDestroyInstance(instance, NULL);
            return false;
        }

        // Create depth image view.
        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (choosenDepthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT ||
            choosenDepthFormat == VK_FORMAT_D24_UNORM_S8_UINT)
        {
            aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        VkImageViewCreateInfo depthViewInfo = {
            .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image            = depthImage,
            .viewType         = VK_IMAGE_VIEW_TYPE_2D,
            .format           = choosenDepthFormat,
            .subresourceRange = {
                .aspectMask     = aspectMask,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1}};

        result = vkCreateImageView(device, &depthViewInfo, NULL, &depthImageView);
        if (result != VK_SUCCESS)
        {
            ASSERT_MSG(false, "Failed to create depth image view.");
            vkFreeMemory(device, depthImageMemory, NULL);
            vkDestroyImage(device, depthImage, NULL);
            for (u32 i = 0; i < swapchainImageCount; ++i)
                vkDestroyImageView(device, swapchainImageViews[i], NULL);
            FREE(swapchainImageViews);
            FREE(swapchainImages);
            vkDestroySwapchainKHR(device, swapchain, NULL);
            vkDestroyDevice(device, NULL);
            vkDestroySurfaceKHR(instance, surface, NULL);
            vkDestroyInstance(instance, NULL);
            return false;
        }
    }

    // 7. Create command pool.
    VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = choosenGraphicsFamilyIndex};

    VkCommandPool commandPool;
    result = vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &commandPool);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to create command pool");
        vkDestroyImageView(device, depthImageView, NULL);
        vkFreeMemory(device, depthImageMemory, NULL);
        vkDestroyImage(device, depthImage, NULL);
        for (u32 i = 0; i < swapchainImageCount; ++i)
            vkDestroyImageView(device, swapchainImageViews[i], NULL);
        FREE(swapchainImageViews);
        FREE(swapchainImages);
        vkDestroySwapchainKHR(device, swapchain, NULL);
        vkDestroyDevice(device, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    // 8. Allocate command buffer.
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool        = commandPool,
        .commandBufferCount = 1,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY};

    VkCommandBuffer commandBuffer;
    result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to allocate command buffer");
        vkDestroyCommandPool(device, commandPool, NULL);
        vkDestroyImageView(device, depthImageView, NULL);
        vkFreeMemory(device, depthImageMemory, NULL);
        vkDestroyImage(device, depthImage, NULL);
        for (u32 i = 0; i < swapchainImageCount; ++i)
            vkDestroyImageView(device, swapchainImageViews[i], NULL);
        FREE(swapchainImageViews);
        FREE(swapchainImages);
        vkDestroySwapchainKHR(device, swapchain, NULL);
        vkDestroyDevice(device, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    // 9. Create synchronization objects.
    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence     inFlightFence;

    result = vkCreateSemaphore(device, &semaphoreCreateInfo, NULL, &imageAvailableSemaphore);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to create image available semaphore");
        vkDestroyCommandPool(device, commandPool, NULL);
        vkDestroyImageView(device, depthImageView, NULL);
        vkFreeMemory(device, depthImageMemory, NULL);
        vkDestroyImage(device, depthImage, NULL);
        for (u32 i = 0; i < swapchainImageCount; ++i)
            vkDestroyImageView(device, swapchainImageViews[i], NULL);
        FREE(swapchainImageViews);
        FREE(swapchainImages);
        vkDestroySwapchainKHR(device, swapchain, NULL);
        vkDestroyDevice(device, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    result = vkCreateSemaphore(device, &semaphoreCreateInfo, NULL, &renderFinishedSemaphore);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to create render finished semaphore");
        vkDestroySemaphore(device, imageAvailableSemaphore, NULL);
        vkDestroyCommandPool(device, commandPool, NULL);
        vkDestroyImageView(device, depthImageView, NULL);
        vkFreeMemory(device, depthImageMemory, NULL);
        vkDestroyImage(device, depthImage, NULL);
        for (u32 i = 0; i < swapchainImageCount; ++i)
            vkDestroyImageView(device, swapchainImageViews[i], NULL);
        FREE(swapchainImageViews);
        FREE(swapchainImages);
        vkDestroySwapchainKHR(device, swapchain, NULL);
        vkDestroyDevice(device, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    result = vkCreateFence(device, &fenceCreateInfo, NULL, &inFlightFence);
    if (result != VK_SUCCESS)
    {
        ASSERT_MSG(false, "Failed to create in flight fence");
        vkDestroySemaphore(device, renderFinishedSemaphore, NULL);
        vkDestroySemaphore(device, imageAvailableSemaphore, NULL);
        vkDestroyCommandPool(device, commandPool, NULL);
        vkDestroyImageView(device, depthImageView, NULL);
        vkFreeMemory(device, depthImageMemory, NULL);
        vkDestroyImage(device, depthImage, NULL);
        for (u32 i = 0; i < swapchainImageCount; ++i)
            vkDestroyImageView(device, swapchainImageViews[i], NULL);
        FREE(swapchainImageViews);
        FREE(swapchainImages);
        vkDestroySwapchainKHR(device, swapchain, NULL);
        vkDestroyDevice(device, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        return false;
    }

    DROP_GfxHandle handle = ALLOC_SINGLE(_GfxHandle);
    ASSERT(handle);
    // Core.
    handle->instance       = instance;
    handle->device         = device;
    handle->physicalDevice = choosenPhysicalDevice;
    handle->surface        = surface;
    // Swapchain.
    handle->swapchain           = swapchain;
    handle->swapchainImages     = ALLOC_ARRAY(VkImage, swapchainImageCount);
    handle->swapchainImageViews = ALLOC_ARRAY(VkImageView, swapchainImageCount);
    ASSERT(handle->swapchainImages && handle->swapchainImageViews);
    for (u32 i = 0; i < swapchainImageCount; ++i)
    {
        handle->swapchainImages[i]     = swapchainImages[i];
        handle->swapchainImageViews[i] = swapchainImageViews[i];
    }
    FREE(swapchainImageViews);
    FREE(swapchainImages);
    handle->swapchainImageCount = swapchainImageCount;
    handle->swapchainFormat     = choosenFormat.format;
    handle->swapchainExtent     = extent;
    // Command objects.
    handle->commandPool   = commandPool;
    handle->commandBuffer = commandBuffer;
    // Synchronization.
    handle->imageAvailableSemaphore = imageAvailableSemaphore;
    handle->renderFinishedSemaphore = renderFinishedSemaphore;
    handle->inFlightFence           = inFlightFence;
    // Queue info.
    handle->graphicsQueue       = graphicsQueue;
    handle->presentQueue        = presentQueue;
    handle->graphicsFamilyIndex = choosenGraphicsFamilyIndex;
    handle->presentFamilyIndex  = choosenPresentFamilyIndex;
    // Win32.
    handle->hwnd = hwnd;
    // Cache clearing state.
    handle->clearColor.float32[0] = 0.0f;
    handle->clearColor.float32[1] = 0.0f;
    handle->clearColor.float32[2] = 0.0f;
    handle->clearColor.float32[3] = 1.0f;
    handle->clearDepth            = 1.0f;
    handle->clearFlags            = DROP_GFX_CLEAR_NONE;
    // Depth buffer.
    handle->depthImage       = depthImage;
    handle->depthImageMemory = depthImageMemory;
    handle->depthImageView   = depthImageView;
    handle->depthFormat      = choosenDepthFormat;
    // Current state.
    handle->currentImageIndex = 0;

    *pHandle        = handle;
    s_currentHandle = handle;

    return true;
}

void Graphics_DestroyGraphics(DROP_GfxHandle* pHandle)
{
    ASSERT_MSG(pHandle && *pHandle, "Handle is NULL.");
    DROP_GfxHandle handle = *pHandle;

    if (handle)
    {
        // 1. Wait for all ops.
        vkDeviceWaitIdle(handle->device);

        // 2. Destroy sync objects.
        VK_SAFE_DESTROY(vkDestroySemaphore, handle->device, handle->renderFinishedSemaphore);
        VK_SAFE_DESTROY(vkDestroySemaphore, handle->device, handle->imageAvailableSemaphore);
        VK_SAFE_DESTROY(vkDestroyFence, handle->device, handle->inFlightFence);

        // 3. Destroy command objects.
        vkFreeCommandBuffers(handle->device, handle->commandPool, 1, &handle->commandBuffer);
        VK_SAFE_DESTROY(vkDestroyCommandPool, handle->device, handle->commandPool);

        // 4. Destroy image views.
        for (u32 i = 0; i < handle->swapchainImageCount; ++i)
        {
            VK_SAFE_DESTROY(vkDestroyImageView, handle->device, handle->swapchainImageViews[i]);
        }

        // 5. Free arrays.
        FREE(handle->swapchainImageViews);
        FREE(handle->swapchainImages);
        handle->swapchainImageViews = NULL;
        handle->swapchainImages     = NULL;

        // 5.5 Destroy depth buffer.
        VK_SAFE_DESTROY(vkDestroyImageView, handle->device, handle->depthImageView);
        VK_SAFE_DESTROY(vkDestroyImage, handle->device, handle->depthImage);
        VK_SAFE_DESTROY(vkFreeMemory, handle->device, handle->depthImageMemory);

        // 6. Destroy swapchain.
        VK_SAFE_DESTROY(vkDestroySwapchainKHR, handle->device, handle->swapchain);

        // 7. Destroy device.
        if (handle->device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(handle->device, NULL);
            handle->device = VK_NULL_HANDLE;
        }

        // 8. Destroy surface.
        VK_SAFE_DESTROY(vkDestroySurfaceKHR, handle->instance, handle->surface);

        // 9. Destroy instance.
        if (handle->instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(handle->instance, NULL);
            handle->instance = VK_NULL_HANDLE;
        }

        // 10. Free handle.
        if (s_currentHandle == handle)
            s_currentHandle = NULL;
        FREE(handle);
        handle = NULL;
    }

    *pHandle = NULL;
}

bool Graphics_SwapBuffers(DROP_GfxHandle handle)
{
    // 1. Wait for the previous frame to finish.
    vkWaitForFences(handle->device, 1, &handle->inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(handle->device, 1, &handle->inFlightFence);

    // 2. Acquire next swapchain image.
    u32      imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        handle->device, handle->swapchain, UINT64_MAX,
        handle->imageAvailableSemaphore,
        VK_NULL_HANDLE, &imageIndex);

    handle->currentImageIndex = imageIndex;
    if (result != VK_SUCCESS) return false;

    // 3. Reset and record command buffer.
    vkResetCommandBuffer(handle->commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(handle->commandBuffer, &beginInfo);

    // 4. Transition image layout.
    if (handle->clearFlags & DROP_GFX_CLEAR_COLOR)
    {
        VkImageMemoryBarrier barrier = {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_GENERAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = handle->swapchainImages[imageIndex],
            .subresourceRange    = {
                   .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                   .baseMipLevel   = 0,
                   .levelCount     = 1,
                   .baseArrayLayer = 0,
                   .layerCount     = 1},
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT};

        vkCmdPipelineBarrier(handle->commandBuffer,
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             0, 0, NULL, 0, NULL, 1, &barrier);

        // Clear operation
        VkImageSubresourceRange colorRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1};

        vkCmdClearColorImage(handle->commandBuffer,
                             handle->swapchainImages[imageIndex],
                             VK_IMAGE_LAYOUT_GENERAL,
                             &handle->clearColor, 1, &colorRange);

        // Transition untuk present
        barrier.oldLayout     = VK_IMAGE_LAYOUT_GENERAL;
        barrier.newLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

        vkCmdPipelineBarrier(handle->commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0, 0, NULL, 0, NULL, 1, &barrier);
    }
    else
    {
        // Transition for present.
        VkImageMemoryBarrier barrier = {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = handle->swapchainImages[imageIndex],
            .subresourceRange    = {
                   .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                   .baseMipLevel   = 0,
                   .levelCount     = 1,
                   .baseArrayLayer = 0,
                   .layerCount     = 1},
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT};

        vkCmdPipelineBarrier(handle->commandBuffer,
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0, 0, NULL, 0, NULL, 1, &barrier);
    }

    // 5. Handle depth clear operations
    if (handle->clearFlags & DROP_GFX_CLEAR_DEPTH)
    {
        // Pastikan depth buffer tersedia
        if (handle->depthImage != VK_NULL_HANDLE)
        {
            // Transition depth image untuk clear
            VkImageMemoryBarrier depthBarrier = {
                .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout           = VK_IMAGE_LAYOUT_GENERAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image               = handle->depthImage,
                .subresourceRange    = {
                       .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                       .baseMipLevel   = 0,
                       .levelCount     = 1,
                       .baseArrayLayer = 0,
                       .layerCount     = 1},
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT};

            vkCmdPipelineBarrier(handle->commandBuffer,
                                 VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 0, 0, NULL, 0, NULL, 1, &depthBarrier);

            // Clear depth
            VkClearDepthStencilValue depthClearValue = {
                .depth   = handle->clearDepth,
                .stencil = 0 // Atau handle->clearStencil jika Anda support stencil
            };

            VkImageSubresourceRange depthRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .levelCount = 1,
                .layerCount = 1};

            vkCmdClearDepthStencilImage(handle->commandBuffer,
                                        handle->depthImage,
                                        VK_IMAGE_LAYOUT_GENERAL,
                                        &depthClearValue, 1, &depthRange);

            // Transition depth image untuk rendering
            depthBarrier.oldLayout     = VK_IMAGE_LAYOUT_GENERAL;
            depthBarrier.newLayout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                         VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            vkCmdPipelineBarrier(handle->commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                                 0, 0, NULL, 0, NULL, 1, &depthBarrier);
        }
    }
    else if (handle->depthImage != VK_NULL_HANDLE)
    {
        // Jika tidak clear depth, tetap transition ke layout yang benar
        VkImageMemoryBarrier depthBarrier = {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = handle->depthImage,
            .subresourceRange    = {
                   .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                   .baseMipLevel   = 0,
                   .levelCount     = 1,
                   .baseArrayLayer = 0,
                   .layerCount     = 1},
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT};

        vkCmdPipelineBarrier(handle->commandBuffer,
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                             0, 0, NULL, 0, NULL, 1, &depthBarrier);
    }

    // Clear pending flags.
    handle->clearFlags = DROP_GFX_CLEAR_NONE;

    vkEndCommandBuffer(handle->commandBuffer);

    // 6. Submit command buffer.
    VkSemaphore          waitSemaphores[]   = {handle->imageAvailableSemaphore};
    VkSemaphore          signalSemaphores[] = {handle->renderFinishedSemaphore};
    VkPipelineStageFlags waitStages[]       = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = waitSemaphores,
        .pWaitDstStageMask    = waitStages,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &handle->commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = signalSemaphores};

    result = vkQueueSubmit(handle->graphicsQueue, 1, &submitInfo, handle->inFlightFence);
    if (result != VK_SUCCESS) return false;

    // 6. Present to screen.
    VkSwapchainKHR   swapchains[] = {handle->swapchain};
    VkPresentInfoKHR presentInfo  = {
         .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
         .waitSemaphoreCount = 1,
         .pWaitSemaphores    = signalSemaphores,
         .swapchainCount     = 1,
         .pSwapchains        = swapchains,
         .pImageIndices      = &imageIndex};

    result = vkQueuePresentKHR(handle->presentQueue, &presentInfo);
    return result == VK_SUCCESS;
}

bool Graphics_MakeCurrent(DROP_GfxHandle handle)
{
    DEBUG_OP(if (!handle) return false);

    s_currentHandle = handle;
    return true;
}

void Graphics_ClearColor(f32 r, f32 g, f32 b, f32 a)
{
    ASSERT(s_currentHandle);
    s_currentHandle->clearColor.float32[0] = r;
    s_currentHandle->clearColor.float32[1] = g;
    s_currentHandle->clearColor.float32[2] = b;
    s_currentHandle->clearColor.float32[3] = a;
}

void Graphics_ClearDepth(f32 depth)
{
    ASSERT(s_currentHandle);
    s_currentHandle->clearDepth = depth;
}

void Graphics_Clear(DROP_GFX_CLEAR flags)
{
    ASSERT(s_currentHandle);
    s_currentHandle->clearFlags = flags;
}
