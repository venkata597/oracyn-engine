#pragma once

#include <unordered_map>
#include <vector>

#include "../resources/assetloader.hpp"

#include "../backend/resources/gl_mesh.hpp"
#include "../backend/resources/gl_material.hpp"

namespace Backend {
    class GLRenderDevice;
}

struct GPUAssetData{
    std::vector<Backend::GPUMesh> gMeshes;
    std::vector<Backend::GPUMaterial> gMaterials;
};

class RenderContext{
    friend class Backend::GLRenderDevice;
private:
    static std::unordered_map<unsigned int,GPUAssetData> gpuResourceMap;
public:
    void uploadToGPU(AssetLoader& asset_loader);
};
