#include "../../include/backend/vulkanbe.hpp"
#include "../../include/backend/be_shader.hpp"
#include "../../include/backend/be_vertex.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>

void VulkanBackend::_create_instance(std::vector<const char*> extensions){
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

bool VulkanBackend::_check_validation_layer_support(){
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

VkDebugUtilsMessengerCreateInfoEXT VulkanBackend::_create_debug_create_info(VkDebugUtilsMessengerCreateInfoEXT& dinfo){
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

VkResult VulkanBackend::_create_debug_messenger(VkInstance instance,
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

void VulkanBackend::_destroy_debug_messenger(VkInstance instance,VkDebugUtilsMessengerEXT debugMessenger,const VkAllocationCallbacks* pAllocator){
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT");
    if(func!=nullptr){
        func(instance,debugMessenger,pAllocator);
    }
}

int VulkanBackend::_score_device(VkPhysicalDevice device){

    bool extensions_supported = _check_device_extension_support(device);
    bool swap_chain_supported = false;
    if(extensions_supported){
        _SwapChainSupportDetails support = _query_swapchain_support_details(device);
        swap_chain_supported = !support.formats.empty() && !support.present_modes.empty();
    }
    if(!swap_chain_supported){
        return 0;
    }

    _QueueFamilyIndices qf = _find_queue_families(device);
    if(!qf.isComplete()){
        return 0;
    }
    int score = 0;
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceProperties(device,&device_properties);
    vkGetPhysicalDeviceFeatures(device,&device_features);

    if(device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU){
        score += 100;
    }
    else if(device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
        score+=150;
    }
    else{
        score+=5;
    }
    
    if(!device_features.geometryShader){
        return 0;
    }
    return score;
}

void VulkanBackend::_pick_physical_device(){
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

VulkanBackend::_QueueFamilyIndices VulkanBackend::_find_queue_families(VkPhysicalDevice device){
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

void VulkanBackend::_create_logical_device(){
    _QueueFamilyIndices qf = _find_queue_families(physicalDevice);

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

bool VulkanBackend::_check_device_extension_support(VkPhysicalDevice device){
    uint32_t ec;
    vkEnumerateDeviceExtensionProperties(device,nullptr,&ec,nullptr);

    std::vector<VkExtensionProperties> available_extensions(ec);
    vkEnumerateDeviceExtensionProperties(device,nullptr,&ec,available_extensions.data());

    std::set<std::string> required_extension(device_extensions.begin(),device_extensions.end());

    for(const auto& extension: available_extensions){
        required_extension.erase(extension.extensionName);
    }
    return required_extension.empty();
}

VulkanBackend::_SwapChainSupportDetails VulkanBackend::_query_swapchain_support_details(VkPhysicalDevice device){
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

VkSurfaceFormatKHR VulkanBackend::_choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> surface_formats){
    VkSurfaceFormatKHR format{};
    for(const auto& aformat: surface_formats){
        if(aformat.format == VK_FORMAT_B8G8R8A8_SRGB && aformat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
            format = aformat;
            break;
        }
    }
    return format;
}

VkPresentModeKHR VulkanBackend::_choose_swap_present_mode(const std::vector<VkPresentModeKHR> present_modes){
    VkPresentModeKHR present_mode;
    for(const auto& pm: present_modes){
        if(pm == VK_PRESENT_MODE_MAILBOX_KHR){
            present_mode = pm;
        }
    }
    return present_mode;
}

VkExtent2D VulkanBackend::_choose_swap_extent(const VkSurfaceCapabilitiesKHR capabilites,AppWindow& window){
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

void VulkanBackend::_create_swapchain(AppWindow& window){
    _SwapChainSupportDetails details = _query_swapchain_support_details(physicalDevice);

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

    _QueueFamilyIndices qf = _find_queue_families(physicalDevice);
    uint32_t queue_family_indices[] = {
        qf.graphics_family.value(),
        qf.present_family.value()
    };
    if(qf.graphics_family != qf.present_family){
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

void VulkanBackend::_create_image_views(){
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

VkShaderModule VulkanBackend::_create_shader_module(const std::vector<char>& code){
    VkShaderModuleCreateInfo s_create_info{};
    s_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    s_create_info.codeSize = code.size();
    s_create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shad_mod;
    if(vkCreateShaderModule(ldevice,&s_create_info,nullptr,&shad_mod)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to create Shader Module" << '\n';
    }
    return shad_mod;
}


void VulkanBackend::_create_render_pass(){
    VkAttachmentDescription ca{};
    ca.format = swapChainImageFormat;
    ca.samples = VK_SAMPLE_COUNT_1_BIT;
    ca.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ca.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ca.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ca.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    ca.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ca.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference ca_attach_ref{};
    ca_attach_ref.attachment = 0;
    ca_attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription sub_pass{};
    sub_pass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    sub_pass.colorAttachmentCount = 1;
    sub_pass.pColorAttachments = &ca_attach_ref;

    VkSubpassDependency subpass_depend{};
    subpass_depend.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_depend.dstSubpass = 0;
    subpass_depend.srcStageMask =  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_depend.srcAccessMask = 0;
    subpass_depend.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_depend.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo rp_create_info{};
    rp_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_create_info.attachmentCount = 1;
    rp_create_info.pAttachments = &ca;
    rp_create_info.subpassCount = 1;
    rp_create_info.pSubpasses = &sub_pass;
    rp_create_info.dependencyCount = 1;
    rp_create_info.pDependencies = &subpass_depend;

    if(vkCreateRenderPass(ldevice,&rp_create_info,nullptr,&renderPass)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to create Render Pass" << '\n';
        return;
    }
}

void VulkanBackend::_create_graphics_pipeline(){
    Shader vertex_code;
    Shader fragment_code;
    vertex_code.readFromFile("shaders/vert.spv");
    fragment_code.readFromFile("shaders/frag.spv");

    std::vector<char> vert_raw_code = vertex_code.getShaderCode();
    std::vector<char> frag_raw_code = fragment_code.getShaderCode();
    VkShaderModule vertex_shader_mod = _create_shader_module(vert_raw_code);
    VkShaderModule frag_shader_mod = _create_shader_module(frag_raw_code);

    VkPipelineShaderStageCreateInfo vert_stage_create_info{};
    vert_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage_create_info.module = vertex_shader_mod;
    vert_stage_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_stage_create_info{};
    frag_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_stage_create_info.module = frag_shader_mod;
    frag_stage_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {
        vert_stage_create_info,
        frag_stage_create_info
    };

    std::vector<VkDynamicState> dyn_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dyn_create_info{};
    dyn_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dyn_create_info.dynamicStateCount = static_cast<uint32_t>(dyn_states.size());
    dyn_create_info.pDynamicStates = dyn_states.data();

    VkPipelineVertexInputStateCreateInfo vinp_create_info{};
    auto bnd_dsc = gpuVertex::getBindingDesc();
    auto attr_dsc = gpuVertex::getAttributeDesc();
    vinp_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vinp_create_info.vertexBindingDescriptionCount = 1;
    vinp_create_info.pVertexBindingDescriptions = &bnd_dsc;
    vinp_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attr_dsc.size());
    vinp_create_info.pVertexAttributeDescriptions = attr_dsc.data();
    
    VkPipelineInputAssemblyStateCreateInfo ias_create_info{};
    ias_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ias_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ias_create_info.primitiveRestartEnable = VK_FALSE;

    
    viewPort.x = 0.0f;
    viewPort.y = 0.0f;
    viewPort.width = (float)swapChainExtent.width;
    viewPort.height = (float)swapChainExtent.height;
    viewPort.minDepth = 0.0f;
    viewPort.maxDepth = 1.0f;

    scissor.offset = {0,0};
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo vp_create_info{};
    vp_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp_create_info.viewportCount = 1;
    vp_create_info.pViewports = &viewPort;
    vp_create_info.scissorCount = 1;
    vp_create_info.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rs_create_info{};
    rs_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs_create_info.depthClampEnable = VK_FALSE;
    rs_create_info.rasterizerDiscardEnable = VK_FALSE;
    rs_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rs_create_info.lineWidth = 1.0f;
    rs_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rs_create_info.frontFace =VK_FRONT_FACE_CLOCKWISE;
    rs_create_info.depthBiasEnable = VK_FALSE;
    rs_create_info.depthBiasConstantFactor = 0.0f;
    rs_create_info.depthBiasClamp = 0.0f;
    rs_create_info.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo ms_create_info{};
    ms_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms_create_info.sampleShadingEnable = VK_FALSE;
    ms_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    ms_create_info.minSampleShading = 1.0f;
    ms_create_info.pSampleMask = nullptr;
    ms_create_info.alphaToCoverageEnable = VK_FALSE;
    ms_create_info.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState cba_create_info{};
    cba_create_info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    cba_create_info.blendEnable = VK_FALSE;
    cba_create_info.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    cba_create_info.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    cba_create_info.colorBlendOp = VK_BLEND_OP_ADD;
    cba_create_info.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    cba_create_info.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    cba_create_info.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo cbs_create_info{};
    cbs_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cbs_create_info.logicOpEnable = VK_FALSE;
    cbs_create_info.logicOp = VK_LOGIC_OP_COPY;
    cbs_create_info.attachmentCount = 1;
    cbs_create_info.pAttachments = &cba_create_info;

    VkPipelineLayoutCreateInfo pp_create_info{};
    pp_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pp_create_info.setLayoutCount = 0;
    pp_create_info.pSetLayouts = nullptr;
    pp_create_info.pushConstantRangeCount = 0;
    pp_create_info.pPushConstantRanges = nullptr;

    if(vkCreatePipelineLayout(ldevice,&pp_create_info,nullptr,&pipelineLayout)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to create graphics pipeline layout" << '\n';
    }
    
    VkGraphicsPipelineCreateInfo gp_create_info{};
    gp_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    gp_create_info.stageCount = 2;
    gp_create_info.pStages = shader_stages;
    gp_create_info.pVertexInputState = &vinp_create_info;
    gp_create_info.pInputAssemblyState = &ias_create_info;
    gp_create_info.pViewportState = &vp_create_info;
    gp_create_info.pRasterizationState = &rs_create_info;
    gp_create_info.pMultisampleState = &ms_create_info;
    gp_create_info.pDepthStencilState = nullptr;
    gp_create_info.pColorBlendState = &cbs_create_info;
    gp_create_info.pDynamicState = &dyn_create_info;
    gp_create_info.layout = pipelineLayout;
    gp_create_info.renderPass = renderPass;
    gp_create_info.subpass = 0;
    gp_create_info.basePipelineHandle = VK_NULL_HANDLE;
    gp_create_info.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(ldevice,VK_NULL_HANDLE,1,&gp_create_info, nullptr,&graphicsPipeline)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to create Graphics Pipeline" << '\n';
        return;
    }

    vkDestroyShaderModule(ldevice,vertex_shader_mod,nullptr);
    vkDestroyShaderModule(ldevice,frag_shader_mod,nullptr);
}

void VulkanBackend::_create_frame_buffers(){
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

void VulkanBackend::_create_command_pool(){
    _QueueFamilyIndices qfi = _find_queue_families(physicalDevice);

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = qfi.graphics_family.value();

    if(vkCreateCommandPool(ldevice,&pool_info,nullptr,&commandPool)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to create Command Pool" << '\n';
        return;
    }
}

void VulkanBackend::_create_command_buffers(){
    uint32_t image_c;
    vkGetSwapchainImagesKHR(ldevice,swapChain,&image_c,nullptr);
    commandBuffers.resize(image_c);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = commandPool;
    alloc_info.level  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    
    if(vkAllocateCommandBuffers(ldevice,&alloc_info,commandBuffers.data())!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to allocate command buffers" << '\n';
        return;
    }
}

void VulkanBackend::_record_command_buffer(VkCommandBuffer commandBuffer,uint32_t imageIndex){
    VkCommandBufferBeginInfo beg_info{};
    beg_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beg_info.flags = 0;
    beg_info.pInheritanceInfo = nullptr;

    if(vkBeginCommandBuffer(commandBuffer,&beg_info)!=VK_SUCCESS){
        std::cout << "[ORACYN (Backend)]: Failed to begin recording command buffer" << '\n';
        return;
    }

    VkRenderPassBeginInfo rp_beg_info{};
    rp_beg_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_beg_info.renderPass = renderPass;
    rp_beg_info.framebuffer = swapChainFrameBuffers[imageIndex];
    rp_beg_info.renderArea.offset = {0,0};
    rp_beg_info.renderArea.extent = swapChainExtent;

    VkClearValue clearcolor = {{{0.0f,0.0f,0.0f,1.0f}}};
    rp_beg_info.clearValueCount = 1;
    rp_beg_info.pClearValues = &clearcolor;

    vkCmdBeginRenderPass(commandBuffer,&rp_beg_info,VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,graphicsPipeline);
        vkCmdSetViewport(commandBuffer,0,1,&viewPort);
        vkCmdSetScissor(commandBuffer,0,1,&scissor);
        
        VkBuffer vertexBuffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer,0,1,vertexBuffers,offsets);
        vkCmdDraw(commandBuffer,static_cast<uint32_t>(vertices.size()),1,0,0);

    vkCmdEndRenderPass(commandBuffer);

    if(vkEndCommandBuffer(commandBuffer)!=VK_SUCCESS){
        std::cout << "[ORACYN (Backend)]: Failed to record command buffer" << '\n';
    }
}

void VulkanBackend::_create_sync_objects(){
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(swapChainImages.size());
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);    
    uint32_t image_c;
    vkGetSwapchainImagesKHR(ldevice,swapChain,&image_c,nullptr);

    imagesInFlight.resize(image_c,VK_NULL_HANDLE);
    
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(size_t i = 0;i<MAX_FRAMES_IN_FLIGHT;i++){
        if(vkCreateSemaphore(ldevice,&semaphore_info,nullptr,&imageAvailableSemaphores[i]) ||
            vkCreateFence(ldevice,&fence_info,nullptr,&inFlightFences[i])){
                std::cout << "[ORACYN (BACKEND)]: Failed to create Semaphores or Fence" << '\n';
                return;
        }
    }

    for(size_t i = 0;i<swapChainImages.size();i++){
        if(vkCreateSemaphore(ldevice,&semaphore_info,nullptr,&renderFinishedSemaphores[i]) != VK_SUCCESS){
            std::cout << "[ORACYN (BACKEND)]: Failed to create Semaphores or Fence" << '\n';
            return;
        }
    }
}

void VulkanBackend::_cleanup_swap_chain(){
    for(auto framebuffer: swapChainFrameBuffers){
        vkDestroyFramebuffer(ldevice,framebuffer,nullptr);
    }
    for(auto imageview: swapChainImageViews){
        vkDestroyImageView(ldevice,imageview,nullptr);
    }
    vkDestroySwapchainKHR(ldevice,swapChain,nullptr);
    vkDestroyPipeline(ldevice,graphicsPipeline,nullptr);
    vkDestroyPipelineLayout(ldevice,pipelineLayout,nullptr);
    vkDestroyRenderPass(ldevice,renderPass, nullptr);
}

void VulkanBackend::_recreate_swap_chain(){
    vkDeviceWaitIdle(ldevice);

    _cleanup_swap_chain();

    _create_swapchain(*this->window);
    _create_image_views();
    _create_render_pass();
    _create_graphics_pipeline();
    _create_frame_buffers();
}

uint32_t VulkanBackend::_find_memory_type(uint32_t type_filter,VkMemoryPropertyFlags flags){
    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &mem_props);
    for(uint32_t i = 0;i<mem_props.memoryTypeCount;i++){
        if(type_filter & (1<<i) && (mem_props.memoryTypes[i].propertyFlags & flags) == flags){
            return i;
        }
    }
    std::cout << "[ORACYN (BACKEND)]: Failed to find suitable memory type" << '\n';
    return -1;
}

void VulkanBackend::_create_vertex_buffer(){
    VkBufferCreateInfo bfc_inf{};
    bfc_inf.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bfc_inf.size = sizeof(vertices[0]) * vertices.size();
    bfc_inf.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bfc_inf.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(ldevice,&bfc_inf,nullptr,&vertexBuffer)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to create vertex buffer" << '\n';
        return;
    }

    VkMemoryRequirements mem_rqrmnts;
    vkGetBufferMemoryRequirements(ldevice,vertexBuffer,&mem_rqrmnts);
    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_rqrmnts.size;
    alloc_info.memoryTypeIndex = _find_memory_type(mem_rqrmnts.memoryTypeBits,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if(vkAllocateMemory(ldevice,&alloc_info,nullptr,&vertexBufferMemory)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to allocate memory for vertex buffer" << '\n';
        return;
    }

    if(vkBindBufferMemory(ldevice,vertexBuffer,vertexBufferMemory,0)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to bind vertex buffer" << '\n';
        return;
    }

    void* data;
    vkMapMemory(ldevice,vertexBufferMemory,0,bfc_inf.size,0,&data);
        memcpy(data,vertices.data(),(size_t)bfc_inf.size);
    vkUnmapMemory(ldevice,vertexBufferMemory);
}

void VulkanBackend::initBackend(std::vector<const char*> extensions,AppWindow& window){
    this->window = &window;
    _create_instance(extensions);
    surface = window.createVulkanSurface(this->instance);
    if(enableValidationLayers){
        VkDebugUtilsMessengerCreateInfoEXT info{};
        _create_debug_create_info(info);
        if(_create_debug_messenger(instance,&info,nullptr,&debugMessenger) != VK_SUCCESS){
            std::cout << "[ORACYN (Backend)]: Failed to create debug messenger for Validation Layers" << '\n';
            return;
        }
    }
    _pick_physical_device();
    _create_logical_device();
    _create_swapchain(window);
    _create_image_views();
    _create_render_pass();
    _create_graphics_pipeline();
    _create_frame_buffers();
    _create_command_pool();
    _create_vertex_buffer();
    _create_command_buffers();
    _create_sync_objects();
}

void VulkanBackend::drawFrame(){
    vkWaitForFences(ldevice,1,&inFlightFences[currentFrame],VK_TRUE,UINT64_MAX);
    
    if(windowResized){
        windowResized = false;
        _recreate_swap_chain();
        return;
    }

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(ldevice,swapChain,UINT64_MAX,imageAvailableSemaphores[currentFrame],VK_NULL_HANDLE,&imageIndex);

    if(result==VK_ERROR_OUT_OF_DATE_KHR || result==VK_SUBOPTIMAL_KHR || windowResized){
        windowResized = false;
        _recreate_swap_chain();
        return;
    }else if(result!= VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to acquire swapchain image" << '\n';
        return;
    }

    if(imagesInFlight[imageIndex]!=VK_NULL_HANDLE){
        vkWaitForFences(ldevice,1,&imagesInFlight[imageIndex],VK_TRUE,UINT64_MAX);
    }

    imagesInFlight[imageIndex] =  inFlightFences[currentFrame];

    vkResetFences(ldevice,1,&inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[imageIndex],0);
    _record_command_buffer(commandBuffers[imageIndex],imageIndex);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {
        imageAvailableSemaphores[currentFrame]
    };

    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signal_semaphores[] = {
        renderFinishedSemaphores[imageIndex]
    };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if(vkQueueSubmit(graphicsQueue,1,&submit_info,inFlightFences[currentFrame])!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to submit to the graphics queue" << '\n';
        return;
    }   

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    
    VkSwapchainKHR swap_chains[] = {swapChain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &imageIndex;
    present_info.pResults = nullptr;

    result = vkQueuePresentKHR(presentQueue,&present_info);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowResized){
        windowResized = false;
        _recreate_swap_chain();
        
    }else if(result != VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to present swap chain image" << '\n';
        return;
    }

    currentFrame = (currentFrame+1)%MAX_FRAMES_IN_FLIGHT;
}

void VulkanBackend::deviceWait(){
    vkDeviceWaitIdle(ldevice);
}

void VulkanBackend::resize(){
    windowResized = true;
}

VulkanBackend::~VulkanBackend(){

    _cleanup_swap_chain();

    if(vertexBuffer!=VK_NULL_HANDLE){
        vkDestroyBuffer(ldevice,vertexBuffer,nullptr);
    }
    if(vertexBufferMemory!=VK_NULL_HANDLE){
        vkFreeMemory(ldevice,vertexBufferMemory,nullptr);
    }

    for(size_t i = 0;i<swapChainImages.size();i++){
        if(renderFinishedSemaphores[i]!=VK_NULL_HANDLE){
            vkDestroySemaphore(ldevice,renderFinishedSemaphores[i],nullptr);
        }
    }
    for(size_t i = 0;i<MAX_FRAMES_IN_FLIGHT;i++){
        if(inFlightFences[i]!=VK_NULL_HANDLE){
            vkDestroyFence(ldevice,inFlightFences[i],nullptr);
        }
        if(imageAvailableSemaphores[i]!=VK_NULL_HANDLE){
            vkDestroySemaphore(ldevice,imageAvailableSemaphores[i],nullptr);
        }
    }
    if(commandPool!=VK_NULL_HANDLE){
        vkDestroyCommandPool(ldevice,commandPool,nullptr);
    }
    
    if(ldevice!=VK_NULL_HANDLE){
        vkDestroyDevice(ldevice,nullptr);
    }
    if(surface!=VK_NULL_HANDLE){
        vkDestroySurfaceKHR(instance,surface,nullptr);
    }
    if(enableValidationLayers){
        _destroy_debug_messenger(instance,debugMessenger,nullptr);
    }
    if(instance!=VK_NULL_HANDLE){
        vkDestroyInstance(instance,nullptr);
    }
}