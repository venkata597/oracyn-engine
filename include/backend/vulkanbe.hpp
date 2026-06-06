#pragma once

#include <cstdint>
#include <sys/types.h>
#include <vector>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <iostream>
#include <optional>
#include "../windowing/window.hpp"

#ifndef NDEBUG
    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const bool enableValidationLayers = true;
#else 
    const bool enablevenableValidationLayers = false;
#endif


const int MAX_FRAMES_IN_FLIGHT = 2; 

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messasgeType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData,
    void* pUserData
){
    std::cerr << "Validation Layer: " << pCallBackData->pMessage << '\n';

    return VK_FALSE;
}

class VulkanBackend{
private:
    struct _QueueFamilyIndices{
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool isComplete(){
            return graphics_family.has_value();
        }
    };

    const std::vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    struct _SwapChainSupportDetails{
        VkSurfaceCapabilitiesKHR capabilites;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

private:

    AppWindow* window = nullptr;
    bool windowResized = false;
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice ldevice = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkImageView> swapChainImageViews;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    VkViewport viewPort{};
    VkRect2D scissor{};

    VkPipeline graphicsPipeline = VK_NULL_HANDLE;

    std::vector<VkFramebuffer> swapChainFrameBuffers;

    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;

    uint32_t currentFrame = 0;
private:
    // Create Instance
    void _create_instance(std::vector<const char*> extensions);
    // Check for validation layers
    bool _check_validation_layer_support();

    // Debug Messenger Setup and Destruction
    VkDebugUtilsMessengerCreateInfoEXT _create_debug_create_info(VkDebugUtilsMessengerCreateInfoEXT& dinfo);
    VkResult _create_debug_messenger(VkInstance instance,const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,const VkAllocationCallbacks* pAllocator,VkDebugUtilsMessengerEXT* pDebugMessenger);
    void _destroy_debug_messenger(VkInstance instance,VkDebugUtilsMessengerEXT debugMessenger,const VkAllocationCallbacks* pAllocator);

    // Physical Device picking
    bool _is_device_suitable(VkPhysicalDevice device);
    int _score_device(VkPhysicalDevice device);
    void _pick_physical_device();

    // Queue Families
    _QueueFamilyIndices _find_queue_families(VkPhysicalDevice device);

    // Logical Device Creation
    void _create_logical_device();

    // Swapchain Creation Helpers
    bool _check_device_extension_support(VkPhysicalDevice device);
    _SwapChainSupportDetails _query_swapchain_support_details(VkPhysicalDevice device);
    VkSurfaceFormatKHR _choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> surface_formats);
    VkPresentModeKHR _choose_swap_present_mode(const std::vector<VkPresentModeKHR> present_modes);
    VkExtent2D _choose_swap_extent(const VkSurfaceCapabilitiesKHR capabilites,AppWindow& window);

    // Swapchain Creation
    void _create_swapchain(AppWindow& window);

    // Image View Creation
    void _create_image_views();

    // Render Pass Creation
    void _create_render_pass();

    // Graphics Pipeline Creation Helpers
    VkShaderModule _create_shader_module(const std::vector<char>& code);

    // Graphics Pipeline Creation
    void _create_graphics_pipeline();

    // Frame Buffer Creation
    void _create_frame_buffers();

    // Command Pool Creation
    void _create_command_pool();

    // Command Buffer Creation
    void _create_command_buffers();

    // Command Buffer Recording
    void _record_command_buffer(VkCommandBuffer commandBuffer,uint32_t imageIndex);

    // Create Synchronization Objects
    void _create_sync_objects();

    // Swapchain Recreation
    void _cleanup_swap_chain();
    void _recreate_swap_chain();

public:
    void initBackend(std::vector<const char*> extensions,AppWindow& window);
    void resize();
    void drawFrame();
    void deviceWait();
    ~VulkanBackend();
};