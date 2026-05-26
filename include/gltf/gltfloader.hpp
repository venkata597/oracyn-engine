#pragma once

#include "cgltf/cgltf.h"
#include <unordered_map>

#include <string>
#include <memory>

struct CGLTFdeleter{
    void operator()(cgltf_data* data)const noexcept{
        if(data){
            cgltf_free(data);
        }
    }
};

using CGLTFDataPtr = std::unique_ptr<cgltf_data,CGLTFdeleter>;

class gltfLoader{
private:

    std::unordered_map<std::string,CGLTFDataPtr> model_map; 

public:
    void loadModel(const std::string& modelname);
    cgltf_data* getData(const std::string& modelname);

    gltfLoader() = default;
    gltfLoader(const gltfLoader&) = delete;
    gltfLoader operator=(const gltfLoader&) = delete;
};