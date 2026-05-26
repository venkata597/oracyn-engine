#pragma once

#include "mesh.hpp"
#include "material.hpp"
#include "../gltf/gltfloader.hpp"
#include <string>
#include <unordered_map>
#include <vector>

struct cgltf_data;

class AssetLoader;

class AssetMap{
private:
    static std::unordered_map<std::string,unsigned int> map;
public:
    static unsigned int getAssetID(std::string aname);
    friend class AssetLoader;
};

struct AssetData{
    std::vector<Mesh> meshes;
    std::vector<MaterialData> materials;
};

class AssetLoader{
private:

    std::string assetpath = "assets/";
    std::string path;

    MeshLoader meshloader;
    MaterialLoader materialloader;

    gltfLoader modelLoader;
    
    static unsigned int _id;
    
    std::vector<std::string> _get_file_contents(std::string path);

    void _register_asset(std::string name);

    static std::unordered_map<unsigned int,AssetData> asset_map;

public:
    void loadAsset(std::string assetname);
    AssetData getAssetDataByID(unsigned int id);

    AssetLoader() = default;
    AssetLoader(const AssetLoader&) = delete;
    AssetLoader operator=(const AssetLoader&) = delete;
};