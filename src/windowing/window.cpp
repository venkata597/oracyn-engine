#include "../../include/windowing/window.hpp"
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL_video.h>
#include <iostream>


void AppWindow::_create_window(const char* title){
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if(window==NULL){
        std::cout << "[ORACYN (WINDOWING)]: Unable to create window" << SDL_GetError() << '\n';
        this->status = false;
    }
    else{
        SDL_PumpEvents();
    }
}

AppWindow::AppWindow(const char* title){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "[ORACYN (WINDOWING)]: Unable to Initialize SDL_VIDEO" << SDL_GetError() << '\n';
        this->status = false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);

    _create_window(title);
}

void AppWindow::handleEvents(){
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            this->status = false;
        }
        else if(event.type==SDL_WINDOWEVENT){
            if(event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
                this->resized = true;
            }
        }
    }
}

void AppWindow::swapBuffers(){
    SDL_GL_SwapWindow(window);
}

bool AppWindow::isOpen(){
    return status;
}

bool AppWindow::isResized(){
    return resized;
}

void AppWindow::clearResize(){
    resized = false;
}

std::vector<const char*> AppWindow::getExtensions(){
    unsigned int extension_count = 0;
    if(!SDL_Vulkan_GetInstanceExtensions(window,&extension_count,nullptr)){
        std::cerr << "[ORACYN (WINDOWING)]: Unable to get Vulkan instance extension count" << SDL_GetError() << '\n'; 
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    std::vector<const char*> extensions(extension_count);
    if(!SDL_Vulkan_GetInstanceExtensions(window,&extension_count,extensions.data())){
        std::cerr << "[ORACYN (WINDOWING)]: Unable to get Vulkan extensions" << SDL_GetError() << '\n';
    }
    return extensions;
}

VkSurfaceKHR AppWindow::createVulkanSurface(const VkInstance& instance){
    VkSurfaceKHR surface;
    if(SDL_Vulkan_CreateSurface(window,instance,&surface) != SDL_TRUE){
        std::cerr << "[ORACYN (WINDOWING)]: Failed to create a Vulkan Surface" << SDL_GetError() << '\n';
        return nullptr;
    }
    return surface;
}

std::pair<int,int> AppWindow::getFrameBufferSize(){
    int width,height;
    SDL_Vulkan_GetDrawableSize(window,&width,&height);
    return std::make_pair(width,height);
}

AppWindow::~AppWindow(){
    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit();
}

