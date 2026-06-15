#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

inline void hash_combine(std::size_t& seed,const std::string& v){
    std::hash<std::string> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
inline void hash_combine(std::size_t& seed,unsigned int v){
    std::hash<unsigned int> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct Transform{

    enum Mode{
        TRANSLATE, 
        ROTATE,
        SCALE
    };

    glm::mat4 matrix = glm::mat4(1.0f);
    
    void apply_transform(Transform::Mode mode,glm::vec3 param,float angle = NULL);
};

class Entity{
private:
    static unsigned int _id;
public:
    unsigned int ID;
    unsigned int assetID;
    std::string name;

    Transform transform;

    void makeEntity(std::string n,unsigned int assetid,glm::vec3 trans,glm::vec3 rotate,glm::vec3 scale,float angle);

    bool operator==(const Entity& other) const {
        return name==other.name && assetID==other.assetID;
    }
};

struct EntityHasher{
    std::size_t operator()(const Entity& e) const noexcept{
        std::size_t seed = 0;
        hash_combine(seed,e.name);
        hash_combine(seed,e.assetID);
        return seed;
    }
};