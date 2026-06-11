#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "../../windowing/window.hpp"

class VulkanSwapChain{
private:
    struct _SwapChainSupportDetails{
        VkSurfaceCapabilitiesKHR capabilites;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };
private:
    AppWindow* window = nullptr;

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;


    std::vector<VkFramebuffer> swapChainFrameBuffers;
private:
    
    _SwapChainSupportDetails _query_swapchain_support_details(const VkPhysicalDevice& device,const VkSurfaceKHR& surface);
    VkSurfaceFormatKHR _choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> surface_formats);
    VkPresentModeKHR _choose_swap_present_mode(const std::vector<VkPresentModeKHR> present_modes);
    VkExtent2D _choose_swap_extent(const VkSurfaceCapabilitiesKHR capabilites,AppWindow& window);

    // Swapchain Creation
    void _create_swapchain(AppWindow& window,const VkPhysicalDevice& physicalDevice,const VkSurfaceKHR& surface,const std::pair<uint32_t,uint32_t>& qf,const VkDevice& ldevice);

    // Swapchain Creation Helpers
    bool _check_device_extension_support(VkPhysicalDevice device);
   
    // Image View Creation
    void _create_image_views(const VkDevice& ldevice);

    void _create_frame_buffers(const VkDevice& ldevice,const VkRenderPass& renderPass);

public:
    void initSwapChain(AppWindow& window,const VkPhysicalDevice& physicalDevice,const VkSurfaceKHR& surface,const std::pair<uint32_t,uint32_t>& qf,const VkDevice& ldevice);
    void initFrameBuffers(const VkDevice& ldevice,const VkRenderPass& render_pass);
    void cleanupSwapChain(const VkDevice& ldevice);

    const VkSwapchainKHR& getswapChain();
    const std::vector<VkImage>& getswapChainImages();
    const VkFormat& getswapChainImageFormat();
    const VkExtent2D& getswapChainExtent();
    const std::vector<VkImageView>& getswapChainImageViews();


    const std::vector<VkFramebuffer>& getswapChainFrameBuffers();
};
