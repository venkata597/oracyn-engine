#include "../../include/renderer/rendercontext.hpp"
#include "../../include/backend/opengl/renderdevice.hpp"
#include <iostream>

std::unordered_map<unsigned int,GPUAssetData> RenderContext::gpuResourceMap;

void RenderContext::uploadToGPU(Scene& scene,Backend::GLRenderDevice& render_device){
    for(auto& [id,asset_data]: scene.loader.asset_map){
        GPUAssetData gpu_asset;
        Backend::GPUScene scene;
        scene.constructScene(asset_data.model,glm::mat4(1.0f),render_device.getInstanceBufferID());
        gpu_asset.scene = std::move(scene);

        for(auto& material: asset_data.materials){
            Backend::GPUMaterial gpu_material;
            gpu_material.makeGPUMaterial(std::move(material));
            gpu_asset.gMaterials.push_back(std::move(gpu_material));
        }
        gpuResourceMap[id] = std::move(gpu_asset);
    }
}
