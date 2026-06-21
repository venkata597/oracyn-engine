#include "../../include/resources/mesh.hpp"
#include "../../include/gltf/cgltf/cgltf.h"
#include <cstdint>

std::vector<Mesh> MeshLoader::constructMesh(cgltf_data* data){
    std::vector<Mesh> meshes;
    meshes.reserve(data->meshes_count);
    for(int c = 0;c<data->meshes_count;c++){
        cgltf_mesh* mesh = &(data->meshes[c]);
        Mesh m;
        m.name = mesh->name;
        m.primitives.reserve(mesh->primitives_count);
        for(cgltf_size i = 0;i<mesh->primitives_count;++i){
            Primitive pdata;
            cgltf_primitive* primitive = &mesh->primitives[i];

            cgltf_accessor* positionaccessor = nullptr;
            cgltf_accessor* uvaccessor = nullptr;
            cgltf_accessor* normalaccessor = nullptr;

            for(cgltf_size j = 0;j<primitive->attributes_count;++j){

                cgltf_attribute* attr = &primitive->attributes[j];
                if(attr->type == cgltf_attribute_type_position){
                    positionaccessor = attr->data;
                }
                else if(attr->type == cgltf_attribute_type_texcoord){
                    uvaccessor = attr->data;
                }
                else if(attr->type == cgltf_attribute_type_normal){
                    normalaccessor = attr->data;
                }
            }

            if(!positionaccessor){
                continue;
            }

            pdata.vertices.reserve(positionaccessor->count);

            cgltf_size vc = positionaccessor->count;

            for(cgltf_size j = 0;j<vc;++j){

                Vertex v{};

                {
                    float pos[3];

                    cgltf_accessor_read_float(
                    positionaccessor,
                    j,
                    pos,
                    3
                    );

                    v.position = glm::vec3(pos[0],pos[1],pos[2]);
                }

                if(uvaccessor){

                    float uv[2];
                    cgltf_accessor_read_float(
                    uvaccessor,
                    j,
                    uv,
                    2
                    );

                    v.uv = glm::vec2(uv[0],uv[1]);
                }

                if(normalaccessor){
                    float normals[3];
                    cgltf_accessor_read_float(
                    normalaccessor,
                    j,
                    normals,
                    3
                    );
                    v.normals = glm::vec3(normals[0],normals[1],normals[2]);
                }
                pdata.vertices.push_back(v);
            }


            cgltf_accessor* indexaccessor = primitive->indices;
            if(!indexaccessor){
                for(uint32_t k = 0;k<(uint32_t)positionaccessor->count;++k){
                    pdata.indices.push_back(k);
                }
            }
            else{
                pdata.indices.reserve(indexaccessor->count);
                for(cgltf_size k = 0 ;k<indexaccessor->count;++k){
                    cgltf_uint index = cgltf_accessor_read_index(
                    indexaccessor,
                    k
                    );
                    pdata.indices.push_back((uint32_t)index);
                }
            }
            pdata.material_index = primitive->material ? static_cast<uint32_t>(primitive->material - data->materials) : UINT32_MAX;
            m.primitives.push_back(pdata);
        }
        meshes.push_back(m);
    }
    return meshes;
}
