#include "../../../include/backend/resources/gl_mesh.hpp"
#include <cstdint>
#include <cstring>
#include <iostream>

void Backend::GPUMesh::makeMesh(Mesh m,unsigned int ibid){
    for(int pc = 0;pc<m.primitives.size();pc++){
        Backend::GPUPrimitive prim;

        prim.vao.createVAO();
        prim.vao.bind();

        prim.vb.create();
        prim.vb.bind();
        prim.vb.setData(m.primitives.at(pc).vertices);

        prim.ib.create();
        prim.ib.bind();
        prim.ib.setData(m.primitives.at(pc).indices);

        prim.vao.setAttribPointers();
        glBindBuffer(GL_ARRAY_BUFFER,ibid);
        prim.vao.setInstanceAttribPointers();
        prim.indices_count = m.primitives.at(pc).indices.size();
        prim.materialIndex = m.primitives.at(pc).material_index;

        this->mesh.push_back(std::move(prim));
    }
}

void Backend::NodeUBO::bindUniformBufferData(){
    glBufferData(
        GL_UNIFORM_BUFFER,
        sizeof(GPUNodeUBOData),
        nullptr,
        GL_DYNAMIC_DRAW
    );
}

void Backend::NodeUBO::updateUniformBufferData(){
    this->bindUniformBufferObject();
    glBindBufferBase(GL_UNIFORM_BUFFER,2,id);
    void* ptr = glMapBuffer(GL_UNIFORM_BUFFER,GL_WRITE_ONLY);
    std::memcpy(ptr,&data,sizeof(GPUNodeUBOData));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

Backend::GPUNode::GPUNode(){
    nodeUBO.createUniformBufferObject();
    nodeUBO.bindUniformBufferObject();
    nodeUBO.bindUniformBufferData();
}

std::vector<Backend::GPUNode> Backend::GPUScene::_construct_scene_recur(const std::vector<NodeData>& scene,glm::mat4 globalTransform,unsigned int instance_buffer_id){
    std::vector<GPUNode> res;
    for(auto& scene_comp: scene){
        GPUNode node;
        auto local = scene_comp.localTransform.getTransform();
        auto global = globalTransform * local;
        if(scene_comp.mesh.hasMesh){
            node.hasGPUMesh = true;
            if(mesh_cache.find(scene_comp.mesh.name)!=mesh_cache.end()){
                node.meshIndex = mesh_cache.at(scene_comp.mesh.name);
            }
            else{
                GPUMesh mesh;
                mesh.makeMesh(scene_comp.mesh,instance_buffer_id);
                gMeshes.push_back(std::move(mesh));
                mesh_cache[scene_comp.mesh.name] = gMeshes.size() - 1;
                node.meshIndex = mesh_cache.at(scene_comp.mesh.name);
            }
        }
        else{
            node.hasGPUMesh = false;
        }
        node.localTransform = local;
        node.globalTransform = global;
        node.nodeUBO.data.globalTransform = global;

        if(scene_comp.children.size()>0){
            node.child_nodes =
                _construct_scene_recur(scene_comp.children,global,instance_buffer_id);
        }
        res.push_back(std::move(node));
    }
    return res;
}

void Backend::GPUScene::constructScene(std::vector<NodeData> scene,glm::mat4 globalTrasform,unsigned int instance_buffer_id){
    gScene = _construct_scene_recur(scene,globalTrasform,instance_buffer_id);
}

std::vector<Backend::GPUPrimitive>& Backend::GPUScene::getMesh(unsigned int index){
    return this->gMeshes.at(index).mesh;
}
