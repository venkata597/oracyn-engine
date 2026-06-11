#include "../../../include/backend/vulkan/vulkan_swapchain.hpp"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vulkan/vulkan_core.h>


void VulkanSwapChain::_create_image_views(const VkDevice& ldevice){
    swapChainImageViews.resize(swapChainImages.size());
    for(size_t i = 0;i<swapChainImages.size();i++){
        VkImageViewCreateInfo img_info{};
        img_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        img_info.image = swapChainImages[i];
        img_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        img_info.format = swapChainImageFormat;
        
        img_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        img_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        img_info.subresourceRange.baseMipLevel = 0;
        img_info.subresourceRange.levelCount = 1;
        img_info.subresourceRange.baseArrayLayer = 0;
        img_info.subresourceRange.layerCount = 1;

        if(vkCreateImageView(ldevice,&img_info,nullptr,&swapChainImageViews[i])!=VK_SUCCESS){
            std::cout << "[ORACYN (BACKEND)]: Failed to create image view no " <<  i << '\n';
            return;
        }
    }
}

VulkanSwapChain::_SwapChainSupportDetails VulkanSwapChain::_query_swapchain_support_details(const VkPhysicalDevice& device,const VkSurfaceKHR& surface){
    _SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,surface,&details.capabilites);
    uint32_t formatc;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&formatc,nullptr);

    if(formatc!=0){
        details.formats.resize(formatc);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&formatc,details.formats.data());
    }

    uint32_t present_modec;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&present_modec,nullptr);

    if(present_modec!=0){
        details.present_modes.resize(present_modec);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&present_modec,details.present_modes.data());
    }

    return details;
}

VkSurfaceFormatKHR VulkanSwapChain::_choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> surface_formats){
    VkSurfaceFormatKHR format{};
    for(const auto& aformat: surface_formats){
        if(aformat.format == VK_FORMAT_B8G8R8A8_SRGB && aformat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
            format = aformat;
            break;
        }
    }
    return format;
}

VkPresentModeKHR VulkanSwapChain::_choose_swap_present_mode(const std::vector<VkPresentModeKHR> present_modes){
    VkPresentModeKHR present_mode;
    for(const auto& pm: present_modes){
        if(pm == VK_PRESENT_MODE_MAILBOX_KHR){
            present_mode = pm;
        }
    }
    return present_mode;
}

VkExtent2D VulkanSwapChain::_choose_swap_extent(const VkSurfaceCapabilitiesKHR capabilites,AppWindow& window){
    if(capabilites.currentExtent.width != std::numeric_limits<uint32_t>::max()){
        return capabilites.currentExtent;
    }else{
        auto [width,height] = window.getFrameBufferSize();
        VkExtent2D actual_extent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        actual_extent.width = std::clamp(actual_extent.width,capabilites.minImageExtent.width,capabilites.maxImageExtent.width);
        actual_extent.height = std::clamp(actual_extent.height,capabilites.minImageExtent.height,capabilites.maxImageExtent.height);
        return actual_extent;
    }
}

void VulkanSwapChain::_create_swapchain(AppWindow& window,const VkPhysicalDevice& physicalDevice,const VkSurfaceKHR& surface,const std::pair<uint32_t,uint32_t>& qf,const VkDevice& ldevice){
    _SwapChainSupportDetails details = _query_swapchain_support_details(physicalDevice,surface);

    VkSurfaceFormatKHR surface_format = _choose_swap_surface_format(details.formats);
    VkPresentModeKHR present_mode = _choose_swap_present_mode(details.present_modes);
    VkExtent2D extent = _choose_swap_extent(details.capabilites,window);

    uint32_t image_count = details.capabilites.minImageCount + 1;
    if(details.capabilites.maxImageCount>0 && image_count>details.capabilites.maxImageCount){
        image_count = details.capabilites.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queue_family_indices[] = {
        qf.first,
        qf.second
    };
    if(qf.first != qf.second){
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }else{
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }
    create_info.preTransform = details.capabilites.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;
    if(vkCreateSwapchainKHR(ldevice,&create_info,nullptr,&swapChain)){
        std::cout << "[ORACYN (Backend)]: Failed to create Swapchain" << '\n';
        return;
    }
    uint32_t image_c;
    vkGetSwapchainImagesKHR(ldevice,swapChain,&image_c, nullptr);
    swapChainImages.resize(image_c);
    vkGetSwapchainImagesKHR(ldevice,swapChain,&image_c,swapChainImages.data());
    swapChainImageFormat = surface_format.format;
    swapChainExtent = extent;
}


void VulkanSwapChain::_create_frame_buffers(const VkDevice& ldevice,const VkRenderPass& renderPass){
    swapChainFrameBuffers.resize(swapChainImages.size());
    for(size_t i = 0;i<swapChainImages.size();i++){
        VkImageView attachments[] = {
            swapChainImageViews[i]
        };
        VkFramebufferCreateInfo fb_create_info{};
        fb_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_create_info.renderPass = renderPass;
        fb_create_info.attachmentCount = 1;
        fb_create_info.pAttachments = attachments;
        fb_create_info.width = swapChainExtent.width;
        fb_create_info.height = swapChainExtent.height;
        fb_create_info.layers = 1;
        
        if(vkCreateFramebuffer(ldevice,&fb_create_info,nullptr,&swapChainFrameBuffers[i]) != VK_SUCCESS){
            std::cout << "[ORACYN (Backend)]: Failed to create Swap chain Frame buffer no:" << i << "." << '\n';
            return;
        } 
    }
}

void VulkanSwapChain::initSwapChain(AppWindow& window,const VkPhysicalDevice& physicalDevice,const VkSurfaceKHR& surface,const std::pair<uint32_t,uint32_t>& qf,const VkDevice& ldevice){
    _create_swapchain(window,physicalDevice,surface,qf,ldevice);
}

void VulkanSwapChain::initFrameBuffers(const VkDevice& ldevice,const VkRenderPass& render_pass){
    _create_frame_buffers(ldevice,render_pass);
}

void VulkanSwapChain::cleanupSwapChain(const VkDevice& ldevice){
    for(auto framebuffer: swapChainFrameBuffers){
        vkDestroyFramebuffer(ldevice,framebuffer,nullptr);
    }
    for(auto imageview: swapChainImageViews){
        vkDestroyImageView(ldevice,imageview,nullptr);
    }
    vkDestroySwapchainKHR(ldevice,swapChain,nullptr);
}

const VkSwapchainKHR& VulkanSwapChain::getswapChain(){
    return this->swapChain;
}
    
const std::vector<VkImage>& VulkanSwapChain::getswapChainImages(){
    return this->swapChainImages;
}
    
const VkFormat& VulkanSwapChain::getswapChainImageFormat(){
    return this->swapChainImageFormat;
}
    
const VkExtent2D& VulkanSwapChain::getswapChainExtent(){
    return this->swapChainExtent;
}

const std::vector<VkImageView>& VulkanSwapChain::getswapChainImageViews(){
    return this->swapChainImageViews;
}

const std::vector<VkFramebuffer>& VulkanSwapChain::getswapChainFrameBuffers(){
    return this->swapChainFrameBuffers;
}