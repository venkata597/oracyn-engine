#include "../../include/renderer/renderer.hpp"
#include <iostream>
#include <vector>

void Renderer::_build_render_queue(const SceneMap& map){
    for(const auto& [key,value]: map){
        state.draw_queue.push_back(
            {
                .assetID = key.assetID,
                .instance_count = (unsigned int)value.size(),
                .transforms = value
            }
        );
    }
}

void Renderer::beginFrame(Camera cam){
    state.cam_data.view = cam.getViewMatrix();
    state.cam_data.proj = cam.getProjectionMatrix();
    state.cam_data.pos = cam.getPositionVec();
}

void Renderer::draw(const SceneMap& map){
    state.draw_queue.clear();
    _build_render_queue(std::move(map));
}

void Renderer::endFrame(){
    input.resetMouseDelta();
}

RenderState Renderer::getState(){
    return std::move(state);
}
