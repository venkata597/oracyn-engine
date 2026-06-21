#include "../../include/renderer/rendercontext.hpp"

std::unordered_map<unsigned int,GPUAssetData> RenderContext::gpuResourceMap;

void RenderContext::uploadToGPU(AssetLoader& asset_loader){
    for(auto& [id,asset_data]: asset_loader.asset_map){
        GPUAssetData gpu_asset;
        for(const auto& primitves: asset_data.meshes){
            for(const auto& primitive: primitves.primitives){
                Backend::GPUMesh mesh;
                mesh.makeMesh(primitive.vertices,primitive.indices,primitive.material_index);
                gpu_asset.gMeshes.push_back(mesh);
            }
        }
        for(auto& material_data: asset_data.materials){
            Backend::GPUMaterial material;
            material.makeGPUMaterial(std::move(material_data));
            gpu_asset.gMaterials.push_back(material);
        }
        gpuResourceMap[id] = gpu_asset;
    }
}
