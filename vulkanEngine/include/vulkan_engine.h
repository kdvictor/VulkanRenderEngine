/**
 * @file vulkan_engine.h
 * @brief Vulkan rendering engine class declaration (C++ API version)
 * @details This header defines the VulkanEngine class which encapsulates
 *          all Vulkan initialization, resource management, and rendering logic.
 *          Uses modern C++ vulkan.hpp bindings with RAII and exception handling.
 */

#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H

// On Windows platform, must include windows.h before vulkan_win32.h
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#endif

#include <vulkan/vulkan.hpp>
#include <cstdint>
#include <vector>

/**
 * @class VulkanEngine
 * @brief Main Vulkan rendering engine class (C++ API)
 *
 * @details This class manages the entire Vulkan rendering pipeline including:
 *          - Instance and device creation
 *          - Surface and swapchain setup
 *          - Render pass configuration
 *          - Command buffer management
 *          - Synchronization primitives
 *
 * Uses C++ vulkan.hpp bindings with:
 *          - RAII (Unique handles) for automatic resource management
 *          - Exception handling instead of manual error checking
 *          - Modern C++ idioms
 */
class VulkanEngine
{
public:
    /**
     * @brief Constructor
     */
    VulkanEngine();

    /**
     * @brief Destructor - automatically cleans up all resources via RAII
     */
    ~VulkanEngine();

    /**
     * @brief Initialize the Vulkan engine with a native window handle
     * @param nativeWinHandle Native window handle (HWND on Windows)
     * @param width Initial window width in pixels
     * @param height Initial window height in pixels
     * @throws vk::SystemError if initialization fails
     *
     * @details This function performs complete Vulkan initialization including:
     *          instance creation, device selection, swapchain setup, and
     *          synchronization object creation.
     */
    void init(void* nativeWinHandle, uint32_t width, uint32_t height);

    /**
     * @brief Execute one frame of the render loop
     *
     * @details Acquires next swapchain image, records and submits command buffer,
     *          and presents the rendered image to the screen.
     */
    void renderLoop();

    /**
     * @brief Clean up all Vulkan resources
     *
     * @details Waits for device to be idle. RAII handles automatically destroy
     *          all resources in proper order.
     */
    void cleanup();

private:
    // ============================================================================
    // Vulkan Core Objects (RAII managed)
    // ============================================================================

    vk::UniqueInstance       m_instance;                       ///< Vulkan instance
    vk::PhysicalDevice       m_physDev;                        ///< Physical device (GPU)
    vk::UniqueDevice         m_device;                         ///< Logical device
    vk::Queue                m_graphicsQueue;                  ///< Graphics queue
    uint32_t                 m_graphicsQueueFamilyIndex = 0;   ///< Graphics queue family index

    // ============================================================================
    // Surface and Swapchain (RAII managed)
    // ============================================================================

    vk::UniqueSurfaceKHR     m_surface;                        ///< Window surface
    vk::UniqueSwapchainKHR   m_swapchain;                      ///< Swapchain
    vk::Format               m_swapchainImageFormat;           ///< Swapchain image format
    vk::Extent2D             m_swapchainExtent;                ///< Swapchain extent (width, height)

    // ============================================================================
    // Rendering Resources (RAII managed)
    // ============================================================================

    vk::UniqueRenderPass     m_renderPass;                     ///< Render pass
    vk::UniqueCommandPool    m_cmdPool;                        ///< Command pool
    vk::CommandBuffer        m_cmdBuffer;                      ///< Command buffer

    std::vector<vk::Image>           m_swapchainImages;        ///< Array of swapchain images
    std::vector<vk::UniqueImageView> m_swapchainImageViews;    ///< Array of image views (RAII)
    std::vector<vk::UniqueFramebuffer> m_framebuffers;         ///< Array of framebuffers (RAII)

    // ============================================================================
    // Synchronization Objects (RAII managed)
    // ============================================================================

    vk::UniqueSemaphore      m_imageAvailableSem;              ///< Semaphore for image acquisition
    vk::UniqueSemaphore      m_renderFinishedSem;              ///< Semaphore for render completion
    vk::UniqueFence          m_inFlightFence;                  ///< Fence for frame synchronization

    // ============================================================================
    // Private Initialization Methods
    // ============================================================================

    /**
     * @brief Create Vulkan instance
     * @throws vk::SystemError on failure
     */
    void createInstance();

    /**
     * @brief Select suitable physical device (GPU)
     * @throws std::runtime_error if no suitable device found
     */
    void pickPhysicalDevice();

    /**
     * @brief Create logical device and retrieve graphics queue
     * @throws vk::SystemError on failure
     */
    void createLogicalDevice();

    /**
     * @brief Create window surface for rendering
     * @param nativeWinHandle Native window handle
     * @throws vk::SystemError on failure
     */
    void createSurface(void* nativeWinHandle);

    /**
     * @brief Create swapchain with specified dimensions
     * @param w Width in pixels
     * @param h Height in pixels
     * @throws vk::SystemError on failure
     */
    void createSwapchain(uint32_t w, uint32_t h);

    /**
     * @brief Create render pass with basic color attachment
     * @throws vk::SystemError on failure
     */
    void createRenderPass();

    /**
     * @brief Create framebuffers for all swapchain images
     * @throws vk::SystemError on failure
     */
    void createFramebuffers();

    /**
     * @brief Create command pool for allocating command buffers
     * @throws vk::SystemError on failure
     */
    void createCommandPool();

    /**
     * @brief Allocate command buffer from command pool
     * @throws vk::SystemError on failure
     */
    void createCommandBuffer();

    /**
     * @brief Create synchronization objects (semaphores and fences)
     * @throws vk::SystemError on failure
     */
    void createSyncObjects();

    /**
     * @brief Record rendering commands into command buffer
     * @param cmd Command buffer to record into
     * @param imageIdx Index of swapchain image to render to
     */
    void recordCommandBuffer(vk::CommandBuffer cmd, uint32_t imageIdx);
};

#endif // VULKAN_ENGINE_H
