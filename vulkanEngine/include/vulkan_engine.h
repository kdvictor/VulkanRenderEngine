/**
 * @file vulkan_engine.h
 * @brief Vulkan rendering engine class declaration
 * @details This header defines the VulkanEngine class which encapsulates
 *          all Vulkan initialization, resource management, and rendering logic.
 */

#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H

// On Windows platform, must include windows.h before vulkan_win32.h
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#endif

#include <vulkan/vulkan.h>
#include <cstdint>

/**
 * @class VulkanEngine
 * @brief Main Vulkan rendering engine class
 *
 * @details This class manages the entire Vulkan rendering pipeline including:
 *          - Instance and device creation
 *          - Surface and swapchain setup
 *          - Render pass configuration
 *          - Command buffer management
 *          - Synchronization primitives
 */
class VulkanEngine
{
public:
    /**
     * @brief Constructor
     */
    VulkanEngine();

    /**
     * @brief Destructor - calls cleanup()
     */
    ~VulkanEngine();

    /**
     * @brief Initialize the Vulkan engine with a native window handle
     * @param nativeWinHandle Native window handle (HWND on Windows)
     * @param width Initial window width in pixels
     * @param height Initial window height in pixels
     * @return true if initialization succeeds, false otherwise
     *
     * @details This function performs complete Vulkan initialization including:
     *          instance creation, device selection, swapchain setup, and
     *          synchronization object creation.
     */
    bool init(void* nativeWinHandle, uint32_t width, uint32_t height);

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
     * @details Destroys all Vulkan objects in proper order and frees allocated memory.
     */
    void cleanup();

private:
    // ============================================================================
    // Vulkan Core Objects
    // ============================================================================

    VkInstance       m_instance = VK_NULL_HANDLE;              ///< Vulkan instance
    VkPhysicalDevice m_physDev = VK_NULL_HANDLE;               ///< Physical device (GPU)
    VkDevice         m_device = VK_NULL_HANDLE;                ///< Logical device
    VkQueue          m_graphicsQueue = VK_NULL_HANDLE;         ///< Graphics queue
    uint32_t         m_graphicsQueueFamilyIndex = 0;           ///< Graphics queue family index

    // ============================================================================
    // Surface and Swapchain
    // ============================================================================

    VkSurfaceKHR     m_surface = VK_NULL_HANDLE;               ///< Window surface
    VkSwapchainKHR   m_swapchain = VK_NULL_HANDLE;             ///< Swapchain
    VkFormat         m_swapchainImageFormat;                   ///< Swapchain image format
    VkExtent2D       m_swapchainExtent;                        ///< Swapchain extent (width, height)

    // ============================================================================
    // Rendering Resources
    // ============================================================================

    VkRenderPass     m_renderPass = VK_NULL_HANDLE;            ///< Render pass
    VkCommandPool    m_cmdPool = VK_NULL_HANDLE;               ///< Command pool
    VkCommandBuffer  m_cmdBuffer = VK_NULL_HANDLE;             ///< Command buffer

    VkImage*         m_swapchainImages = nullptr;              ///< Array of swapchain images
    VkImageView*     m_swapchainImageViews = nullptr;          ///< Array of image views
    VkFramebuffer*   m_framebuffers = nullptr;                 ///< Array of framebuffers
    uint32_t         m_swapchainImageCount = 0;                ///< Number of swapchain images

    // ============================================================================
    // Synchronization Objects
    // ============================================================================

    VkSemaphore      m_imageAvailableSem = VK_NULL_HANDLE;     ///< Semaphore for image acquisition
    VkSemaphore      m_renderFinishedSem = VK_NULL_HANDLE;     ///< Semaphore for render completion
    VkFence          m_inFlightFence = VK_NULL_HANDLE;         ///< Fence for frame synchronization

    // ============================================================================
    // Private Initialization Methods
    // ============================================================================

    /**
     * @brief Create Vulkan instance
     * @return true on success, false on failure
     */
    bool createInstance();

    /**
     * @brief Select suitable physical device (GPU)
     * @return true on success, false on failure
     */
    bool pickPhysicalDevice();

    /**
     * @brief Create logical device and retrieve graphics queue
     * @return true on success, false on failure
     */
    bool createLogicalDevice();

    /**
     * @brief Create window surface for rendering
     * @param nativeWinHandle Native window handle
     * @return true on success, false on failure
     */
    bool createSurface(void* nativeWinHandle);

    /**
     * @brief Create swapchain with specified dimensions
     * @param w Width in pixels
     * @param h Height in pixels
     * @return true on success, false on failure
     */
    bool createSwapchain(uint32_t w, uint32_t h);

    /**
     * @brief Create render pass with basic color attachment
     * @return true on success, false on failure
     */
    bool createRenderPass();

    /**
     * @brief Create framebuffers for all swapchain images
     * @return true on success, false on failure
     */
    bool createFramebuffers();

    /**
     * @brief Create command pool for allocating command buffers
     * @return true on success, false on failure
     */
    bool createCommandPool();

    /**
     * @brief Allocate command buffer from command pool
     * @return true on success, false on failure
     */
    bool createCommandBuffer();

    /**
     * @brief Create synchronization objects (semaphores and fences)
     * @return true on success, false on failure
     */
    bool createSyncObjects();

    /**
     * @brief Record rendering commands into command buffer
     * @param cmd Command buffer to record into
     * @param imageIdx Index of swapchain image to render to
     */
    void recordCommandBuffer(VkCommandBuffer cmd, uint32_t imageIdx);
};

#endif // VULKAN_ENGINE_H
