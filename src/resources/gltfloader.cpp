#include "../../include/gltf/gltfloader.hpp"
#include <stdexcept>

void gltfLoader::loadModel(const std::string& modelname){

    cgltf_options options{};
    cgltf_data* data = nullptr;

    auto res = cgltf_parse_file(
        &options,
        modelname.c_str(),
        &data
    );

    if(res==cgltf_result_success){
        cgltf_load_buffers(
            &options,
            data,
            modelname.c_str()
        );

        model_map[modelname] = CGLTFDataPtr(data);
    }else{
        throw std::runtime_error("[ORACYN (GLTF LOADER)]: Failed to load the model");
    }
}

cgltf_data* gltfLoader::getData(const std::string& modelname){
    return model_map.at(modelname).get();
}
