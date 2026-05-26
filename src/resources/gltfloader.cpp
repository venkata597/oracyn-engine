#include "../../include/gltf/gltfloader.hpp"

void gltfLoader::loadModel(const std::string& modelname){
    
    cgltf_options options{};
    cgltf_data* data = nullptr;

    cgltf_parse_file(
        &options,
        modelname.c_str(),
        &data
    );

    cgltf_load_buffers(
        &options,
        data,
        modelname.c_str()
    );

    model_map[modelname] = CGLTFDataPtr(data);
}

cgltf_data* gltfLoader::getData(const std::string& modelname){
    return model_map.at(modelname).get();
}
