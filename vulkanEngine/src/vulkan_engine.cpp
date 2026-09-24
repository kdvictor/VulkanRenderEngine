#include "vulkan_engine.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <vulkan/vulkan_win32.h>
#include <windows.h>

VulkanEngine::VulkanEngine() {}
VulkanEngine::~VulkanEngine()
{
    cleanup();
}

bool VulkanEngine::init(void* nativeWinHandle, uint32_t width, uint32_t height)
{
    if (!createInstance()) return false;
    if (!createSurface(nativeWinHandle)) return false;
    if (!pickPhysicalDevice()) return false;
    if (!createLogicalDevice()) return false;
    if (!createSwapchain(width, height)) return false;
    if (!createRenderPass()) return false;
    if (!createFramebuffers()) return false;
    if (!createCommandPool()) return false;
    if (!createCommandBuffer()) return false;
    if (!createSyncObjects()) return false;
    std::cout << "Vulkan init ok\n";
    return true;
}

bool VulkanEngine::createInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "SimpleCAD";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // 需要Win32 surface扩展
    std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    // 开发打开验证层
    std::vector<const char*> layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    createInfo.ppEnabledLayerNames = layers.data();

    VkResult res = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (res != VK_SUCCESS)
    {
        std::cerr << "vkCreateInstance failed\n";
        return false;
    }
    return true;
}

bool VulkanEngine::createSurface(void* nativeWinHandle)
{
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = (HWND)nativeWinHandle;
    createInfo.hinstance = GetModuleHandle(nullptr);
    VkResult res = vkCreateWin32SurfaceKHR(m_instance, &createInfo, nullptr, &m_surface);
    if (res != VK_SUCCESS)
    {
        std::cerr << "create surface failed\n";
        return false;
    }
#endif
    return true;
}

bool VulkanEngine::pickPhysicalDevice()
{
    uint32_t devCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &devCount, nullptr);
    if (devCount == 0) return false;
    std::vector<VkPhysicalDevice> devices(devCount);
    vkEnumeratePhysicalDevices(m_instance, &devCount, devices.data());

    for (auto dev : devices)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, queueFamilies.data());

        bool found = false;
        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, m_surface, &presentSupport);
                if (presentSupport)
                {
                    m_physDev = dev;
                    m_graphicsQueueFamilyIndex = i;
                    found = true;
                    break;
                }
            }
        }
        if (found) break;
    }
    return m_physDev != VK_NULL_HANDLE;
}

bool VulkanEngine::createLogicalDevice()
{
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriority;

    std::vector<const char*> devExts = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(devExts.size());
    createInfo.ppEnabledExtensionNames = devExts.data();

    VkResult res = vkCreateDevice(m_physDev, &createInfo, nullptr, &m_device);
    if (res != VK_SUCCESS) return false;
    vkGetDeviceQueue(m_device, m_graphicsQueueFamilyIndex, 0, &m_graphicsQueue);
    return true;
}

bool VulkanEngine::createSwapchain(uint32_t w, uint32_t h)
{
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physDev, m_surface, &caps);

    std::vector<VkSurfaceFormatKHR> formats;
    uint32_t fmtCnt = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physDev, m_surface, &fmtCnt, nullptr);
    formats.resize(fmtCnt);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physDev, m_surface, &fmtCnt, formats.data());
    m_swapchainImageFormat = formats[0].format;

    m_swapchainExtent.width = std::clamp(w, caps.minImageExtent.width, caps.maxImageExtent.width);
    m_swapchainExtent.height = std::clamp(h, caps.minImageExtent.height, caps.maxImageExtent.height);

    uint32_t imageCount = caps.minImageCount + 1;
    if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
        imageCount = caps.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = m_swapchainImageFormat;
    createInfo.imageColorSpace = formats[0].colorSpace;
    createInfo.imageExtent = m_swapchainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.preTransform = caps.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.clipped = VK_TRUE;

    VkResult res = vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain);
    if (res != VK_SUCCESS) return false;

    vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_swapchainImageCount, nullptr);
    m_swapchainImages = new VkImage[m_swapchainImageCount];
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_swapchainImageCount, m_swapchainImages);

    m_swapchainImageViews = new VkImageView[m_swapchainImageCount];
    for (uint32_t i = 0; i < m_swapchainImageCount; i++)
    {
        VkImageViewCreateInfo ivInfo{};
        ivInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ivInfo.image = m_swapchainImages[i];
        ivInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ivInfo.format = m_swapchainImageFormat;
        ivInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ivInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ivInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ivInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ivInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ivInfo.subresourceRange.baseMipLevel = 0;
        ivInfo.subresourceRange.levelCount = 1;
        ivInfo.subresourceRange.baseArrayLayer = 0;
        ivInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(m_device, &ivInfo, nullptr, &m_swapchainImageViews[i]) != VK_SUCCESS)
            return false;
    }
    return true;
}

bool VulkanEngine::createRenderPass()
{
    VkAttachmentDescription colorAtt{};
    colorAtt.format = m_swapchainImageFormat;
    colorAtt.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAtt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAtt.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAtt.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAtt.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAtt.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAtt.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    VkRenderPassCreateInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.attachmentCount = 1;
    rpInfo.pAttachments = &colorAtt;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;

    return vkCreateRenderPass(m_device, &rpInfo, nullptr, &m_renderPass) == VK_SUCCESS;
}

bool VulkanEngine::createFramebuffers()
{
    m_framebuffers = new VkFramebuffer[m_swapchainImageCount];
    for (uint32_t i = 0; i < m_swapchainImageCount; i++)
    {
        VkFramebufferCreateInfo fbInfo{};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = m_renderPass;
        fbInfo.attachmentCount = 1;
        fbInfo.pAttachments = &m_swapchainImageViews[i];
        fbInfo.width = m_swapchainExtent.width;
        fbInfo.height = m_swapchainExtent.height;
        fbInfo.layers = 1;
        if (vkCreateFramebuffer(m_device, &fbInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
            return false;
    }
    return true;
}

bool VulkanEngine::createCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    return vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_cmdPool) == VK_SUCCESS;
}

bool VulkanEngine::createCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_cmdPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    return vkAllocateCommandBuffers(m_device, &allocInfo, &m_cmdBuffer) == VK_SUCCESS;
}

bool VulkanEngine::createSyncObjects()
{
    VkSemaphoreCreateInfo semInfo{};
    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(m_device, &semInfo, nullptr, &m_imageAvailableSem) != VK_SUCCESS) return false;
    if (vkCreateSemaphore(m_device, &semInfo, nullptr, &m_renderFinishedSem) != VK_SUCCESS) return false;
    if (vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFence) != VK_SUCCESS) return false;
    return true;
}

void VulkanEngine::recordCommandBuffer(VkCommandBuffer cmd, uint32_t imageIdx)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(cmd, &beginInfo);

    VkRenderPassBeginInfo rpBegin{};
    rpBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBegin.renderPass = m_renderPass;
    rpBegin.framebuffer = m_framebuffers[imageIdx];
    rpBegin.renderArea.offset = { 0,0 };
    rpBegin.renderArea.extent = m_swapchainExtent;
    // ========== 纯色背景：浅蓝色 ==========
    VkClearValue clearColor = { {{0.1f,0.2f,0.4f,1.0f}} };
    rpBegin.clearValueCount = 1;
    rpBegin.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass(cmd);
    vkEndCommandBuffer(cmd);
}

void VulkanEngine::renderLoop()
{
    vkWaitForFences(m_device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device, 1, &m_inFlightFence);

    uint32_t imageIdx;
    vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_imageAvailableSem, VK_NULL_HANDLE, &imageIdx);

    vkResetCommandBuffer(m_cmdBuffer, 0);
    recordCommandBuffer(m_cmdBuffer, imageIdx);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSem[] = { m_imageAvailableSem };
    VkPipelineStageFlags waitStage[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSem;
    submitInfo.pWaitDstStageMask = waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_cmdBuffer;
    VkSemaphore signalSem[] = { m_renderFinishedSem };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSem;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFence);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSem;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.pImageIndices = &imageIdx;
    vkQueuePresentKHR(m_graphicsQueue, &presentInfo);
}

void VulkanEngine::cleanup()
{
    if (m_device != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(m_device, m_imageAvailableSem, nullptr);
        vkDestroySemaphore(m_device, m_renderFinishedSem, nullptr);
        vkDestroyFence(m_device, m_inFlightFence, nullptr);

        vkFreeCommandBuffers(m_device, m_cmdPool, 1, &m_cmdBuffer);
        vkDestroyCommandPool(m_device, m_cmdPool, nullptr);

        for (uint32_t i = 0; i < m_swapchainImageCount; i++)
        {
            vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
            vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
        }
        delete[] m_framebuffers;
        delete[] m_swapchainImageViews;
        delete[] m_swapchainImages;

        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
        vkDestroyDevice(m_device, nullptr);
    }
    if (m_surface != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    if (m_instance != VK_NULL_HANDLE)
        vkDestroyInstance(m_instance, nullptr);
}
