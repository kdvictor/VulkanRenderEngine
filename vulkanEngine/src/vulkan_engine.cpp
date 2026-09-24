/**
 * @file vulkan_engine.cpp
 * @brief Implementation of the VulkanEngine class (C++ API version)
 * @details This file contains all the implementation details for Vulkan initialization,
 *          resource creation, rendering, and cleanup operations using modern C++ vulkan.hpp.
 */

#include "vulkan_engine.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

VulkanEngine::VulkanEngine() {}

VulkanEngine::~VulkanEngine()
{
    cleanup();
}

void VulkanEngine::init(void* nativeWinHandle, uint32_t width, uint32_t height)
{
    createInstance();
    createSurface(nativeWinHandle);
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapchain(width, height);
    createRenderPass();
    createFramebuffers();
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
    std::cout << "Vulkan init ok\n";
}

void VulkanEngine::createInstance()
{
    // Application info
    vk::ApplicationInfo appInfo{
        "SimpleCAD",                    // pApplicationName
        VK_MAKE_VERSION(1, 0, 0),       // applicationVersion
        "No Engine",                    // pEngineName
        VK_MAKE_VERSION(1, 0, 0),       // engineVersion
        VK_API_VERSION_1_0              // apiVersion
    };

    // Required Win32 surface extensions
    std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    // Enable validation layers for debugging
    std::vector<const char*> layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    // Instance create info
    vk::InstanceCreateInfo createInfo{
        {},                                             // flags
        &appInfo,                                       // pApplicationInfo
        static_cast<uint32_t>(layers.size()),          // enabledLayerCount
        layers.data(),                                  // ppEnabledLayerNames
        static_cast<uint32_t>(extensions.size()),      // enabledExtensionCount
        extensions.data()                               // ppEnabledExtensionNames
    };

    // Create instance (throws on error)
    m_instance = vk::createInstanceUnique(createInfo);
}

void VulkanEngine::createSurface(void* nativeWinHandle)
{
#ifdef _WIN32
    vk::Win32SurfaceCreateInfoKHR createInfo{
        {},                                 // flags
        GetModuleHandle(nullptr),           // hinstance
        static_cast<HWND>(nativeWinHandle)  // hwnd
    };

    m_surface = m_instance->createWin32SurfaceKHRUnique(createInfo);
#endif
}

void VulkanEngine::pickPhysicalDevice()
{
    // Enumerate physical devices
    std::vector<vk::PhysicalDevice> devices = m_instance->enumeratePhysicalDevices();

    if (devices.empty()) {
        throw std::runtime_error("No physical devices found");
    }

    // Find a device with graphics queue that supports presentation
    for (const auto& device : devices) {
        auto queueFamilies = device.getQueueFamilyProperties();

        for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
            // Check for graphics support
            if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
                // Check for present support
                if (device.getSurfaceSupportKHR(i, m_surface.get())) {
                    m_physDev = device;
                    m_graphicsQueueFamilyIndex = i;

                    vk::PhysicalDeviceProperties props = device.getProperties();
                    std::cout << "Selected GPU: " << props.deviceName << "\n";
                    return;
                }
            }
        }
    }

    throw std::runtime_error("No suitable physical device found");
}

void VulkanEngine::createLogicalDevice()
{
    // Queue create info
    float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo{
        {},                             // flags
        m_graphicsQueueFamilyIndex,     // queueFamilyIndex
        1,                              // queueCount
        &queuePriority                  // pQueuePriorities
    };

    // Enable swapchain extension
    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Device create info
    vk::DeviceCreateInfo createInfo{
        {},                                             // flags
        1,                                              // queueCreateInfoCount
        &queueCreateInfo,                               // pQueueCreateInfos
        0,                                              // enabledLayerCount (deprecated)
        nullptr,                                        // ppEnabledLayerNames (deprecated)
        static_cast<uint32_t>(deviceExtensions.size()), // enabledExtensionCount
        deviceExtensions.data()                         // ppEnabledExtensionNames
    };

    // Create device (throws on error)
    m_device = m_physDev.createDeviceUnique(createInfo);

    // Get queue
    m_graphicsQueue = m_device->getQueue(m_graphicsQueueFamilyIndex, 0);
}

void VulkanEngine::createSwapchain(uint32_t w, uint32_t h)
{
    // Query surface capabilities
    vk::SurfaceCapabilitiesKHR caps = m_physDev.getSurfaceCapabilitiesKHR(m_surface.get());

    // Query surface formats
    std::vector<vk::SurfaceFormatKHR> formats = m_physDev.getSurfaceFormatsKHR(m_surface.get());

    // Query present modes
    std::vector<vk::PresentModeKHR> presentModes = m_physDev.getSurfacePresentModesKHR(m_surface.get());

    // Choose format (prefer BGRA8 SRGB)
    vk::SurfaceFormatKHR chosenFormat = formats[0];
    for (const auto& fmt : formats) {
        if (fmt.format == vk::Format::eB8G8R8A8Srgb &&
            fmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            chosenFormat = fmt;
            break;
        }
    }
    m_swapchainImageFormat = chosenFormat.format;

    // Choose present mode (prefer FIFO for VSync)
    vk::PresentModeKHR chosenMode = vk::PresentModeKHR::eFifo;
    for (const auto& mode : presentModes) {
        if (mode == vk::PresentModeKHR::eMailbox) {
            chosenMode = mode;
            break;
        }
    }

    // Choose extent
    if (caps.currentExtent.width != UINT32_MAX) {
        m_swapchainExtent = caps.currentExtent;
    } else {
        m_swapchainExtent = vk::Extent2D{
            std::clamp(w, caps.minImageExtent.width, caps.maxImageExtent.width),
            std::clamp(h, caps.minImageExtent.height, caps.maxImageExtent.height)
        };
    }

    // Choose image count
    uint32_t imageCount = caps.minImageCount + 1;
    if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount) {
        imageCount = caps.maxImageCount;
    }

    // Create swapchain
    vk::SwapchainCreateInfoKHR createInfo{
        {},                                     // flags
        m_surface.get(),                        // surface
        imageCount,                             // minImageCount
        m_swapchainImageFormat,                 // imageFormat
        chosenFormat.colorSpace,                // imageColorSpace
        m_swapchainExtent,                      // imageExtent
        1,                                      // imageArrayLayers
        vk::ImageUsageFlagBits::eColorAttachment, // imageUsage
        vk::SharingMode::eExclusive,            // imageSharingMode
        0,                                      // queueFamilyIndexCount
        nullptr,                                // pQueueFamilyIndices
        caps.currentTransform,                  // preTransform
        vk::CompositeAlphaFlagBitsKHR::eOpaque, // compositeAlpha
        chosenMode,                             // presentMode
        VK_TRUE,                                // clipped
        nullptr                                 // oldSwapchain
    };

    m_swapchain = m_device->createSwapchainKHRUnique(createInfo);

    // Get swapchain images
    m_swapchainImages = m_device->getSwapchainImagesKHR(m_swapchain.get());
}

void VulkanEngine::createRenderPass()
{
    // Color attachment
    vk::AttachmentDescription colorAttachment{
        {},                                     // flags
        m_swapchainImageFormat,                 // format
        vk::SampleCountFlagBits::e1,            // samples
        vk::AttachmentLoadOp::eClear,           // loadOp
        vk::AttachmentStoreOp::eStore,          // storeOp
        vk::AttachmentLoadOp::eDontCare,        // stencilLoadOp
        vk::AttachmentStoreOp::eDontCare,       // stencilStoreOp
        vk::ImageLayout::eUndefined,            // initialLayout
        vk::ImageLayout::ePresentSrcKHR         // finalLayout
    };

    vk::AttachmentReference colorRef{
        0,                                      // attachment
        vk::ImageLayout::eColorAttachmentOptimal // layout
    };

    // Subpass
    vk::SubpassDescription subpass{
        {},                                     // flags
        vk::PipelineBindPoint::eGraphics,       // pipelineBindPoint
        0,                                      // inputAttachmentCount
        nullptr,                                // pInputAttachments
        1,                                      // colorAttachmentCount
        &colorRef,                              // pColorAttachments
        nullptr,                                // pResolveAttachments
        nullptr,                                // pDepthStencilAttachment
        0,                                      // preserveAttachmentCount
        nullptr                                 // pPreserveAttachments
    };

    // Subpass dependency
    vk::SubpassDependency dependency{
        VK_SUBPASS_EXTERNAL,                    // srcSubpass
        0,                                      // dstSubpass
        vk::PipelineStageFlagBits::eColorAttachmentOutput, // srcStageMask
        vk::PipelineStageFlagBits::eColorAttachmentOutput, // dstStageMask
        {},                                     // srcAccessMask
        vk::AccessFlagBits::eColorAttachmentWrite, // dstAccessMask
        {}                                      // dependencyFlags
    };

    // Render pass create info
    vk::RenderPassCreateInfo createInfo{
        {},                                     // flags
        1,                                      // attachmentCount
        &colorAttachment,                       // pAttachments
        1,                                      // subpassCount
        &subpass,                               // pSubpasses
        1,                                      // dependencyCount
        &dependency                             // pDependencies
    };

    m_renderPass = m_device->createRenderPassUnique(createInfo);
}

void VulkanEngine::createFramebuffers()
{
    // Create image views
    m_swapchainImageViews.reserve(m_swapchainImages.size());
    for (const auto& image : m_swapchainImages) {
        vk::ImageViewCreateInfo createInfo{
            {},                                 // flags
            image,                              // image
            vk::ImageViewType::e2D,             // viewType
            m_swapchainImageFormat,             // format
            {},                                 // components (identity)
            {                                   // subresourceRange
                vk::ImageAspectFlagBits::eColor,
                0, 1,                           // baseMipLevel, levelCount
                0, 1                            // baseArrayLayer, layerCount
            }
        };

        m_swapchainImageViews.push_back(m_device->createImageViewUnique(createInfo));
    }

    // Create framebuffers
    m_framebuffers.reserve(m_swapchainImageViews.size());
    for (const auto& imageView : m_swapchainImageViews) {
        vk::ImageView attachments[] = { imageView.get() };

        vk::FramebufferCreateInfo createInfo{
            {},                                 // flags
            m_renderPass.get(),                 // renderPass
            1,                                  // attachmentCount
            attachments,                        // pAttachments
            m_swapchainExtent.width,            // width
            m_swapchainExtent.height,           // height
            1                                   // layers
        };

        m_framebuffers.push_back(m_device->createFramebufferUnique(createInfo));
    }
}

void VulkanEngine::createCommandPool()
{
    vk::CommandPoolCreateInfo createInfo{
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer, // flags
        m_graphicsQueueFamilyIndex                          // queueFamilyIndex
    };

    m_cmdPool = m_device->createCommandPoolUnique(createInfo);
}

void VulkanEngine::createCommandBuffer()
{
    vk::CommandBufferAllocateInfo allocInfo{
        m_cmdPool.get(),                        // commandPool
        vk::CommandBufferLevel::ePrimary,       // level
        1                                       // commandBufferCount
    };

    std::vector<vk::CommandBuffer> cmdBuffers = m_device->allocateCommandBuffers(allocInfo);
    m_cmdBuffer = cmdBuffers[0];
}

void VulkanEngine::createSyncObjects()
{
    vk::SemaphoreCreateInfo semInfo{};
    vk::FenceCreateInfo fenceInfo{
        vk::FenceCreateFlagBits::eSignaled      // Start signaled
    };

    m_imageAvailableSem = m_device->createSemaphoreUnique(semInfo);
    m_renderFinishedSem = m_device->createSemaphoreUnique(semInfo);
    m_inFlightFence = m_device->createFenceUnique(fenceInfo);
}

void VulkanEngine::recordCommandBuffer(vk::CommandBuffer cmd, uint32_t imageIdx)
{
    // Begin command buffer
    vk::CommandBufferBeginInfo beginInfo{};
    cmd.begin(beginInfo);

    // Clear color: dark blue background
    vk::ClearValue clearColor{
        vk::ClearColorValue{std::array<float, 4>{0.1f, 0.2f, 0.4f, 1.0f}}
    };

    // Render pass begin info
    vk::RenderPassBeginInfo rpBegin{
        m_renderPass.get(),                     // renderPass
        m_framebuffers[imageIdx].get(),         // framebuffer
        vk::Rect2D{{0, 0}, m_swapchainExtent},  // renderArea
        1,                                      // clearValueCount
        &clearColor                             // pClearValues
    };

    cmd.beginRenderPass(rpBegin, vk::SubpassContents::eInline);
    cmd.endRenderPass();
    cmd.end();
}

void VulkanEngine::renderLoop()
{
    // Wait for previous frame
    vk::Result result = m_device->waitForFences(
        1, &m_inFlightFence.get(),
        VK_TRUE,
        UINT64_MAX
    );
    m_device->resetFences(1, &m_inFlightFence.get());

    // Acquire next image
    uint32_t imageIdx = m_device->acquireNextImageKHR(
        m_swapchain.get(),
        UINT64_MAX,
        m_imageAvailableSem.get(),
        nullptr
    ).value;

    // Reset and record command buffer
    m_cmdBuffer.reset();
    recordCommandBuffer(m_cmdBuffer, imageIdx);

    // Submit commands
    vk::Semaphore waitSemaphores[] = { m_imageAvailableSem.get() };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    vk::Semaphore signalSemaphores[] = { m_renderFinishedSem.get() };

    vk::SubmitInfo submitInfo{
        1,                                      // waitSemaphoreCount
        waitSemaphores,                         // pWaitSemaphores
        waitStages,                             // pWaitDstStageMask
        1,                                      // commandBufferCount
        &m_cmdBuffer,                           // pCommandBuffers
        1,                                      // signalSemaphoreCount
        signalSemaphores                        // pSignalSemaphores
    };

    m_graphicsQueue.submit(1, &submitInfo, m_inFlightFence.get());

    // Present
    vk::SwapchainKHR swapchains[] = { m_swapchain.get() };
    vk::PresentInfoKHR presentInfo{
        1,                                      // waitSemaphoreCount
        signalSemaphores,                       // pWaitSemaphores
        1,                                      // swapchainCount
        swapchains,                             // pSwapchains
        &imageIdx,                              // pImageIndices
        nullptr                                 // pResults
    };

    result = m_graphicsQueue.presentKHR(presentInfo);
}

void VulkanEngine::cleanup()
{
    if (m_device) {
        m_device->waitIdle();
    }
    // All Unique handles automatically destroyed in reverse order of creation
}
