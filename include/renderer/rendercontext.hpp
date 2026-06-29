#pragma once

#include <unordered_map>
#include <vector>

#include "../resources/assetloader.hpp"
#include "../resources/scene.hpp"

#include "../backend/resources/gl_mesh.hpp"
#include "../backend/resources/gl_material.hpp"

namespace Backend {
    class GLRenderDevice;
}

struct GPUAssetData{
    Backend::GPUScene scene;
    std::vector<Backend::GPUMaterial> gMaterials;
};

class RenderContext{
    friend class Backend::GLRenderDevice;
private:
    static std::unordered_map<unsigned int,GPUAssetData> gpuResourceMap;
public:
    void uploadToGPU(Scene& scene,Backend::GLRenderDevice& render_device);
};
