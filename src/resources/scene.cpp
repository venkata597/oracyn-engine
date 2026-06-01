#include "../../include/resources/scene.hpp"
#include <fstream>
#include <iostream>


void Scene::_load_entity(std::string n,glm::vec3 trans,glm::vec3 rot,glm::vec3 scl,float angle){
    Entity e;
    unsigned int assetID;
    assetID = AssetMap::getAssetID(n);
    e.makeEntity(n,assetID,trans,rot,scl,angle);
    entities.push_back(e);
}

void Scene::_map_entities(){
    for(int i = 0;i<entities.size();i++){
        instances[entities[i]].push_back(entities[i].transform);
    }
}

void Scene::loadScene(const std::string& filepath){
    std::ifstream file(filepath);
    if(!file.is_open()){
        std::cerr << "[ORACYN (Scene)]: Failed to load scene file: "  << filepath << "." << '\n';
        return;
    }    
    json root;
    file >> root;
    file.clear();


    for(const auto& entity: root["entities"]){
        loader.loadAsset(entity.value("modelpath",""));
        glm::vec3 trans,rot,scale;
        float angle;
        if(entity.contains("instances")){
            for(const auto& instance: entity["instances"]){
                for(int i = 0;i<3;i++){
                    trans[i] = instance["translate"][i].get<float>();
                    rot[i] = instance["rotate"][i].get<float>();
                    scale[i] = instance["scale"][i].get<float>();
                }
                angle = instance["rotate"][3].get<float>();
                _load_entity(
                    entity.value("name",""),
                    trans,
                    rot,
                    scale,
                    angle
                );
            }
        }
    }
}

