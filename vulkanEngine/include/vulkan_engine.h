#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H

// 在 Windows 平台上，必须先包含 windows.h，然后才能包含 vulkan_win32.h
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#endif

#include <vulkan/vulkan.h>
#include <cstdint>

class VulkanEngine
{
public:
    VulkanEngine();
    ~VulkanEngine();

    // ����Qt����ԭ����� HWND�����ڿ���
    bool init(void* nativeWinHandle, uint32_t width, uint32_t height);
    void renderLoop();
    void cleanup();

private:
    // Vulkan ����
    VkInstance       m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physDev = VK_NULL_HANDLE;
    VkDevice         m_device = VK_NULL_HANDLE;
    VkQueue          m_graphicsQueue = VK_NULL_HANDLE;
    uint32_t         m_graphicsQueueFamilyIndex = 0;

    VkSurfaceKHR     m_surface = VK_NULL_HANDLE;
    VkSwapchainKHR   m_swapchain = VK_NULL_HANDLE;
    VkFormat         m_swapchainImageFormat;
    VkExtent2D       m_swapchainExtent;

    VkRenderPass     m_renderPass = VK_NULL_HANDLE;
    VkCommandPool    m_cmdPool = VK_NULL_HANDLE;
    VkCommandBuffer  m_cmdBuffer = VK_NULL_HANDLE;

    VkImage* m_swapchainImages = nullptr;
    VkImageView* m_swapchainImageViews = nullptr;
    VkFramebuffer* m_framebuffers = nullptr;
    uint32_t         m_swapchainImageCount = 0;

    VkSemaphore      m_imageAvailableSem = VK_NULL_HANDLE;
    VkSemaphore      m_renderFinishedSem = VK_NULL_HANDLE;
    VkFence          m_inFlightFence = VK_NULL_HANDLE;

    bool createInstance();
    bool pickPhysicalDevice();
    bool createLogicalDevice();
    bool createSurface(void* nativeWinHandle);
    bool createSwapchain(uint32_t w, uint32_t h);
    bool createRenderPass();
    bool createFramebuffers();
    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();
    void recordCommandBuffer(VkCommandBuffer cmd, uint32_t imageIdx);
};

#endif
