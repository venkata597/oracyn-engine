#pragma once

#include "mesh.hpp"
#include "material.hpp"
#include "../gltf/gltfloader.hpp"
#include <string>
#include <unordered_map>
#include <vector>

struct cgltf_data;

class AssetLoader;
class RenderContext;

struct AssetData{
    std::vector<NodeData> model;
    std::vector<MaterialData> materials;
};

struct SkyBoxData{
    SkyBoxMaterials data;
};

class AssetLoader{
    friend class RenderContext;
private:

    static std::unordered_map<std::string,unsigned int> map;

    std::string assetpath = "assets/";
    std::string path;

    MeshLoader meshloader;
    MaterialLoader materialloader;

    gltfLoader modelLoader;

    static unsigned int _id;

    std::vector<std::string> _get_file_contents(std::string path);

    void _register_asset(std::string name);

    std::unordered_map<unsigned int,AssetData> asset_map;


public:
    void loadSkyBox(std::string skybox_path);
    void loadAsset(std::string assetname);
    unsigned int getAssetID(std::string aname);
    const AssetData& getAssetDataByID(unsigned int id){ return asset_map.at(id);}


    AssetLoader() = default;
    AssetLoader(const AssetLoader&) = delete;
    AssetLoader operator=(const AssetLoader&) = delete;
};
