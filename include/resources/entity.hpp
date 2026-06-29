#pragma once

#include <cstddef>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "transform.hpp"

inline void hash_combine(std::size_t& seed,const std::string& v){
    std::hash<std::string> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
inline void hash_combine(std::size_t& seed,unsigned int v){
    std::hash<unsigned int> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

class Entity{
private:
    static unsigned int _id;
public:
    unsigned int ID;
    unsigned int assetID;
    std::string name;

    Transform transform;

    void makeEntity(std::string n,unsigned int assetid,glm::vec3 trans,glm::quat rotate,glm::vec3 scale);

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
