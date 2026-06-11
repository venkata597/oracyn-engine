#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <iostream>
#include <vulkan/vulkan_core.h>

#ifndef NDEBUG
    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const bool enableValidationLayers = true;
#else 
    const bool enablevenableValidationLayers = false;
#endif

const std::vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messasgeType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData,
    void* pUserData
){
    std::cerr << "Validation Layer: " << pCallBackData->pMessage << '\n';

    return VK_FALSE;
}

class VulkanDevice{
private:
    struct _QueueFamilyIndices{
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool isComplete(){
            return graphics_family.has_value() && present_family.has_value();
        }
    };
private:
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    _QueueFamilyIndices qf;
    VkDevice ldevice = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;

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
    _QueueFamilyIndices _find_queue_families(VkPhysicalDevice device,VkSurfaceKHR surface);

    // Logical Device Creation
    void _create_logical_device(VkSurfaceKHR& surface);

public:
    VulkanDevice(std::vector<const char*> extensions);
    const VkInstance& getInstance();
    void initVulkanDevice(VkSurfaceKHR& surface);
    const VkPhysicalDevice& getPhysicalDevice();
    const std::pair<uint32_t,uint32_t> getQueueFamilies();
    const VkDevice& getLogicalDevice();
};