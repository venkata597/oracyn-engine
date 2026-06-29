#include "../../include/resources/mesh.hpp"
#include "../../include/gltf/cgltf/cgltf.h"
#include <cstdint>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


Mesh MeshLoader::_construct_mesh(cgltf_mesh* mesh){
    Mesh m;
    if(mesh == nullptr){
        m.hasMesh = false;
        return m;
    }
    m.name = mesh->name;
    m.hasMesh = true;
    m.primitives.reserve(mesh->primitives_count);
    for(cgltf_size i = 0;i<mesh->primitives_count;++i){
        Primitive p;
        cgltf_primitive* primitive = &mesh->primitives[i];

        cgltf_accessor* positionaccessor = nullptr;
        cgltf_accessor* uvaccessor = nullptr;
        cgltf_accessor* normalaccessor = nullptr;
        cgltf_accessor* tangentaccessor = nullptr;

        cgltf_accessor* indexaccessor = primitive->indices;
        if(!indexaccessor){
            for(uint32_t k = 0;k<(uint32_t)positionaccessor->count;++k){
                p.indices.push_back(k);
            }
        }
        else{
            p.indices.reserve(indexaccessor->count);
            for(cgltf_size k = 0 ;k<indexaccessor->count;++k){
                cgltf_uint index = cgltf_accessor_read_index(
                indexaccessor,
                k
                );
                p.indices.push_back((uint32_t)index);
            }
        }

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
            else if(attr->type == cgltf_attribute_type_tangent){
                tangentaccessor = attr->data;
            }
        }

        if(!positionaccessor){
            continue;
        }

        p.vertices.reserve(positionaccessor->count);

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
            if(tangentaccessor){
                float tangents[4];
                cgltf_accessor_read_float(
                    tangentaccessor,
                    j,
                    tangents,
                    4
                );
                v.tangents = glm::vec4(tangents[0],tangents[1],tangents[2],tangents[3]);
            }
            p.vertices.push_back(v);
        }
        p.material_index = primitive->material ? static_cast<uint32_t>(primitive->material - _current_model_data->materials) : UINT32_MAX;

        m.primitives.push_back(p);
    }
    return m;
}

NodeData MeshLoader::_construct_node(cgltf_node* node){
    NodeData nde;
    nde.mesh = _construct_mesh(node->mesh);

    if(node->has_matrix){
        nde.localTransform.transform = glm::make_mat4(node->matrix);
        nde.localTransform.hasMatrix = true;
    }else{
        nde.localTransform.hasMatrix = false;
    }

    node->has_translation ? nde.localTransform.translate = glm::make_vec3(node->translation)
        : nde.localTransform.translate = glm::vec3(0.0f);

    node->has_rotation ? nde.localTransform.rotation = glm::make_quat(node->rotation)
        : nde.localTransform.rotation = glm::quat(1.0f,0.0f,0.0f,0.0f);

    node->has_scale ? nde.localTransform.scale = glm::make_vec3(node->scale)
        : nde.localTransform.scale = glm::vec3(1.0f);

    if(node->children_count==0){
        return nde;
    }
    else{
        for(int i = 0;i<node->children_count;i++){
            nde.children.push_back(_construct_node(node->children[i]));
        }
    }
    return nde;
}

std::vector<NodeData> MeshLoader::constructModel(cgltf_data* data){
    _current_model_data = data;
    std::vector<NodeData> model;
    cgltf_scene* scene = _current_model_data->scene;
    for(int c = 0;c<scene->nodes_count;c++){
        model.push_back(_construct_node(scene->nodes[c]));
    }
    return model;
}
