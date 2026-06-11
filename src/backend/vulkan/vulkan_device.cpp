#include "../../../include/backend/vulkan/vulkan_device.hpp"
#include <cstdint>
#include <cstring>
#include <map>
#include <set>
#include <sys/types.h>
#include <utility>
#include <vulkan/vulkan_core.h>

void VulkanDevice::_create_instance(std::vector<const char*> extensions){
    VkApplicationInfo info{};
    info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    info.pApplicationName = "Vulkan Engine";
    info.applicationVersion = VK_MAKE_VERSION(1,0,0);
    info.pEngineName = "None";
    info.engineVersion = VK_MAKE_VERSION(1,0,0);
    info.apiVersion = VK_API_VERSION_1_0;

    if(enableValidationLayers){
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }


    VkInstanceCreateInfo icinfo{};
    icinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    icinfo.pApplicationInfo = &info;    
    icinfo.enabledExtensionCount = extensions.size();
    icinfo.ppEnabledExtensionNames = extensions.data();
    icinfo.enabledLayerCount = 0;

    if(enableValidationLayers && !_check_validation_layer_support()){
        std::cout << "[ORACYN (Backend): Validation layers requested but are not supported" << '\n';
        return;
    }
    VkDebugUtilsMessengerCreateInfoEXT dinfo{};
    if(enableValidationLayers){
        icinfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        icinfo.ppEnabledLayerNames = validation_layers.data();
        _create_debug_create_info(dinfo);
        icinfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &dinfo;
    }
    else{
        icinfo.enabledLayerCount = 0;
        icinfo.pNext = nullptr;
    }

    VkResult result = vkCreateInstance(&icinfo,nullptr,&instance);
    if(result!=VK_SUCCESS){
        std::cout << "[ORACYN (Backend)]: Failed to create Vulkan Instance" << '\n';
        return;
    }
}

bool VulkanDevice::_check_validation_layer_support(){
    uint32_t lc;
    vkEnumerateInstanceLayerProperties(&lc,nullptr);
    std::vector<VkLayerProperties> availableLayers(lc);
    vkEnumerateInstanceLayerProperties(&lc,availableLayers.data());

    for(const char* layer: validation_layers){
        bool layer_found = false;
        for(auto& avail_layer: availableLayers){
            if(strcmp(layer,avail_layer.layerName)==0){
                layer_found = true;
                break;
            }
        }
        if(!layer_found){
                return false;
        }
    }
    return true;
}

VkDebugUtilsMessengerCreateInfoEXT VulkanDevice::_create_debug_create_info(VkDebugUtilsMessengerCreateInfoEXT& dinfo){
    dinfo = {};
    dinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    dinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    
    dinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    
    dinfo.pfnUserCallback = debugCallBack;
    dinfo.pUserData = nullptr;
    return dinfo;
}

VkResult VulkanDevice::_create_debug_messenger(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger
){

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,"vkCreateDebugUtilsMessengerEXT");
    if(func!=nullptr){
        return func(instance,pCreateInfo,pAllocator,pDebugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanDevice::_destroy_debug_messenger(VkInstance instance,VkDebugUtilsMessengerEXT debugMessenger,const VkAllocationCallbacks* pAllocator){
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT");
    if(func!=nullptr){
        func(instance,debugMessenger,pAllocator);
    }
}

int VulkanDevice::_score_device(VkPhysicalDevice device){
    int score = 0;
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceProperties(device,&device_properties);
    vkGetPhysicalDeviceFeatures(device,&device_features);

    if(device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU){
        score += 100;
    }
    else if(device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
        score+=1000;
    }
    else{
        score+=5;
    }
    
    if(!device_features.geometryShader){
        return 0;
    }
    return score;
}

void VulkanDevice::_pick_physical_device(){
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance,&device_count,nullptr);
    if(device_count == 0){
        std::cout << "[ORACYN (Backend)]: No Physical Devices for Vulkan to select from" << '\n';
        return;
    }
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count,devices.data());

    std::multimap<int,VkPhysicalDevice> device_map;

    for(const auto& device: devices){
        int score = _score_device(device);
        device_map.insert(std::make_pair(score,device));
    }

    if(device_map.rbegin()->first>0){
        physicalDevice = device_map.rbegin()->second;
    }else{
        std::cout << "[ORACYN (Backend)]: Unable to select a Physical Device" << '\n';
        return;
    }
}

VulkanDevice::_QueueFamilyIndices VulkanDevice::_find_queue_families(VkPhysicalDevice device,VkSurfaceKHR surface){
    _QueueFamilyIndices indices;
    uint32_t fc = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device,&fc,nullptr);

    std::vector<VkQueueFamilyProperties> queue_f_props(fc);
    vkGetPhysicalDeviceQueueFamilyProperties(device,&fc,queue_f_props.data());

    int i = 0;
    for(const auto& prop: queue_f_props){
        if(indices.isComplete()){
            break;
        }
        if(prop.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphics_family = i;
            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device,i,surface,&present_support);
            if(present_support){
                indices.present_family = i;
            }
        }

        i++;
    }
    return indices;
}

void VulkanDevice::_create_logical_device(VkSurfaceKHR& surface){
    qf = _find_queue_families(physicalDevice,surface);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {qf.graphics_family.value(),qf.present_family.value()};

    float queue_priority = 1.0f;
    for(uint32_t queuefam: unique_queue_families){
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queuefam;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures features{};
    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    device_create_info.pQueueCreateInfos = queue_create_infos.data();
    device_create_info.pEnabledFeatures = &features;
    device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    device_create_info.ppEnabledExtensionNames = device_extensions.data();

    if(enableValidationLayers){
        device_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        device_create_info.ppEnabledLayerNames = validation_layers.data();
    }else{
        device_create_info.enabledLayerCount = 0;
    }

    if(vkCreateDevice(physicalDevice,&device_create_info,nullptr,&ldevice) != VK_SUCCESS){
        std::cout << "[ORACYN (Backend)]: Unable to create a logical device" << '\n';
        return;
    }
    vkGetDeviceQueue(ldevice,qf.graphics_family.value(),0,&graphicsQueue);
    vkGetDeviceQueue(ldevice,qf.present_family.value(),0,&presentQueue);
}

VulkanDevice::VulkanDevice(std::vector<const char*> extensions){
    _create_instance(extensions);
}

void VulkanDevice::initVulkanDevice(VkSurfaceKHR& surface){

    if(enableValidationLayers){
        VkDebugUtilsMessengerCreateInfoEXT info{};
        _create_debug_create_info(info);
        if(_create_debug_messenger(instance,&info,nullptr,&debugMessenger) != VK_SUCCESS){
            std::cout << "[ORACYN (Backend)]: Failed to create debug messenger for Validation Layers" << '\n';
            return;
        }
    }

    _pick_physical_device();
    _create_logical_device(surface);
}

const VkInstance& VulkanDevice::getInstance(){
    return this->instance;
}

const VkPhysicalDevice& VulkanDevice::getPhysicalDevice(){
    return this->physicalDevice;
}

const std::pair<uint32_t,uint32_t> VulkanDevice::getQueueFamilies(){
    return std::make_pair(qf.graphics_family.value(),qf.present_family.value());
}

const VkDevice& VulkanDevice::getLogicalDevice(){
    return this->ldevice;
}