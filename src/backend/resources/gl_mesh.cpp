#include "../../../include/backend/resources/gl_mesh.hpp"
#include <cstdint>
#include <cstring>
#include <iostream>

void Backend::GPUMesh::makeMesh(Mesh m,unsigned int ibid){
    for(int pc = 0;pc<m.primitives.size();pc++){
        Backend::GPUPrimitive prim;

        prim.vao.createVAO();
        prim.vao.bind();
        std::cout << "after VAO bind, err:" << glGetError() << std::endl;

        prim.vb.create();
        prim.vb.bind();
        prim.vb.setData(m.primitives.at(pc).vertices);
        std::cout << "after VBO setData, err:" << glGetError()
                          << " vertex count:" << m.primitives.at(pc).vertices.size() << std::endl;

        prim.ib.create();
        prim.ib.bind();
        prim.ib.setData(m.primitives.at(pc).indices);
        std::cout << "after IBO setData, err:" << glGetError() << std::endl;


        prim.vao.setAttribPointers();
         std::cout << "after setAttribPointers, err:" << glGetError() << std::endl;

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

    glBindBufferBase(GL_UNIFORM_BUFFER,2,id);
}

void Backend::NodeUBO::updateUniformBufferData(){
    this->bindUniformBufferObject();
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GPUNodeUBOData), nullptr, GL_DYNAMIC_DRAW);
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
        std::cout << "CPU globalTransform full matrix:\n";
        for(int row = 0; row < 4; row++){
            std::cout << "  " << global[0][row] << ", " << global[1][row] << ", "
                       << global[2][row] << ", " << global[3][row] << std::endl;
        }

        std::cout << "== node hasMesh:" << node.hasGPUMesh
                  << " children:" << scene_comp.children.size() << " ==\n"
                  << " local  T:(" << local[3][0] << "," << local[3][1] << "," << local[3][2] << ")"
                  << " raw translate:(" << scene_comp.localTransform.translate.x << ","
                                          << scene_comp.localTransform.translate.y << ","
                                          << scene_comp.localTransform.translate.z << ")\n"
                  << " global T:(" << global[3][0] << "," << global[3][1] << "," << global[3][2] << ")"
                  << " parent T:(" << globalTransform[3][0] << "," << globalTransform[3][1] << "," << globalTransform[3][2] << ")"
                  << std::endl;

        std::cout << "hasMatrix:" << scene_comp.localTransform.hasMatrix;
        if(scene_comp.localTransform.hasMatrix){
            auto& m = scene_comp.localTransform.transform;
            std::cout << " matrix col0:(" << m[0][0] << "," << m[0][1] << "," << m[0][2] << ")"
                       << " col1:(" << m[1][0] << "," << m[1][1] << "," << m[1][2] << ")"
                       << " col2:(" << m[2][0] << "," << m[2][1] << "," << m[2][2] << ")";
        }
        std::cout << std::endl;

        if(scene_comp.mesh.hasMesh){
            // ...existing meshIndex assignment...
            std::cout << " meshIndex:" << node.meshIndex
                      << " primCount:" << gMeshes[node.meshIndex].mesh.size();
            for(auto& prim : gMeshes[node.meshIndex].mesh){
                std::cout << " indices:" << prim.indices_count;
            }
            std::cout << std::endl;
        }

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
