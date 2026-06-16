#include "../../include/renderer/renderer.hpp"
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

void Renderer::draw(const SceneMap& map){
    state.draw_queue.clear();

    _build_render_queue(map);
}

const std::vector<DrawCommand>& Renderer::getQueue(){
    return state.draw_queue;
}