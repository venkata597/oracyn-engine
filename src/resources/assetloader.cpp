#include "../../include/resources/assetloader.hpp"
#include <filesystem>
#include <iostream>
#include <vector>

std::unordered_map<std::string,unsigned int> AssetMap::map;
std::unordered_map<unsigned int,AssetData> AssetLoader::asset_map;

unsigned int AssetLoader::_id = 100;

unsigned int AssetMap::getAssetID(std::string aname){
    return AssetMap::map[aname];
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
    AssetMap::map[name] = _id++;
}

void AssetLoader::loadAsset(std::string aname){
    if(AssetMap::map.find(aname) != AssetMap::map.end()){
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
    _register_asset(aname);

    cgltf_data* data;
    modelLoader.loadModel(path+modelfile);
    data = modelLoader.getData(path+modelfile);

    AssetData ad;
    meshloader.constructMesh(data);
    ad.meshes = meshloader.getMeshes();
    
    materialloader.constructMaterial(data,path);
    ad.materials = std::move(materialloader.getMaterials());
    
    asset_map[AssetMap::getAssetID(modelfile)] = std::move(ad);
}