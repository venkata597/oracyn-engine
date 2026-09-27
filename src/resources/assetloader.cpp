#include "../../include/resources/assetloader.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

std::unordered_map<std::string,unsigned int> AssetLoader::map;

unsigned int AssetLoader::_id = 100;

unsigned int AssetLoader::getAssetID(std::string aname){
    auto it = AssetLoader::map.find(aname);
    if(it == map.end()){
        throw std::runtime_error("[ORACYN (ASSET)]: Asset not found");
    }
    return it->second;
}


std::vector<std::string> AssetLoader::_get_file_contents(std::string path){
    std::vector<std::string> contents;
    if(std::filesystem::exists(path) && std::filesystem::is_directory(path)){
        for(const auto& fe: std::filesystem::directory_iterator(path)){
            contents.push_back(fe.path().filename().string());
        }
    }
    return contents;
}

void AssetLoader::_register_asset(std::string name){
    AssetLoader::map[name] = _id++;
}

void AssetLoader::loadAsset(std::string aname){
    if(AssetLoader::map.find(aname) != AssetLoader::map.end()){
        return;
    }
    path = assetpath + aname + "/";
    std::vector<std::string> assetcontents = _get_file_contents(path);
    std::string modelfile;

    for(const std::string& fe: assetcontents){
        if(fe.find(".gltf")!=std::string::npos){
            modelfile = fe;
            break;
        }
        else{
            continue;
        }
    }

    if(modelfile.empty()){
        std::cout << "[ORACYN (AssetLoader)]: Failed to find the .gltf file for " << aname << " model" << '\n';
        return;
    }

    cgltf_data* data;
    modelLoader.loadModel(path+modelfile);
    data = modelLoader.getData(path+modelfile);

    AssetData ad;
    ad.model = meshloader.constructModel(data);
    ad.materials = std::move(materialloader.constructMaterial(data,path));


    _register_asset(aname);
    asset_map[getAssetID(aname)] = std::move(ad);
}

void AssetLoader::loadSkyBox(std::string skybox_path){
    SkyBoxData skybox;
    skybox.data = std::move(materialloader.constructSkyBox(assetpath+skybox_path));
}
