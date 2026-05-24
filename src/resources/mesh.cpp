#include "../../include/resources/mesh.hpp"
#include "../../include/gltf/cgltf/cgltf.h"

void MeshLoader::constructMesh(cgltf_data* data){
    for(int c = 0;c<data->meshes_count;c++){
        cgltf_mesh* mesh = &(data->meshes[c]);
        Mesh m;
        m.name = mesh->name;
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
            for(cgltf_size k = 0 ;k<indexaccessor->count;++k){
                cgltf_uint index = cgltf_accessor_read_index(
                indexaccessor,
                k
                );
                pdata.indices.push_back((uint32_t)index);
            }
            m.primitives.push_back(pdata);
        }
        meshes.push_back(m);
    }
}

const std::vector<Mesh>& MeshLoader::getMeshes() const{
    return this->meshes;
}
