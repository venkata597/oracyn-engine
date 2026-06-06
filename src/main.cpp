#include <iostream>
#include <vulkan/vulkan_core.h>
#include "../include/oracyn.hpp"


int main(){
    AppWindow window("Test Engine");
    VulkanBackend backend;
    backend.initBackend(window.getExtensions(),window);
    while(window.isOpen()){
        window.handleEvents();
        if(window.isResized()){
            backend.resize();
            window.clearResize();
        }
        backend.drawFrame();
        window.swapBuffers();
    }
    backend.deviceWait();
}