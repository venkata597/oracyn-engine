#pragma once

#include "cgltf/cgltf.h"
#include <unordered_map>

#include <string>
#include <memory>

using CGLTFDataPtr = std::unique_ptr<cgltf_data,decltype(&cgltf_free)>;

class gltfLoader{
private:

    std::unordered_map<std::string,std::unique_ptr<cgltf_data,decltype(&cgltf_free)>> model_map; 

public:
    void loadModel(const std::string& modelname);
    cgltf_data* getData(const std::string& modelname);

    gltfLoader(const gltfLoader&) = delete;
    gltfLoader operator=(const gltfLoader&) = delete;
};