#include <iostream>
#include <vulkan/vulkan_core.h>
#include "../include/oracyn.hpp"


int main(){
    AppWindow window("Test Engine");
    VulkanBackend backend;
    backend.initBackend(window.getExtensions(),window);
    while(window.isOpen()){
        window.handleEvents();
        backend.drawFrame();
        window.swapBuffers();
    }
}