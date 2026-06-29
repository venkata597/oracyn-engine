#include "../../include/windowing/window.hpp"
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_mouse.h>
#include <iostream>
#include "../../include/resources/inputhandler.hpp"


void AppWindow::_create_window(const char* title){
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if(window==NULL){
        std::cout << "[ORACYN (WINDOWING)]: Unable to create window" << SDL_GetError() << '\n';
        this->status = false;
    }
    else{
        SDL_PumpEvents();
    }

    glContext = SDL_GL_CreateContext(window);
    if(glContext == NULL){
        std::cout << "[ORACYN (WINDOWING)]: Unable to create an OpenGL context" << SDL_GetError() << '\n';
    }

    if(glewInit()!=GLEW_OK){
        std::cout << "[ORACYN (WINDOWING)]: Unable to initialize GLEW" << '\n';
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);
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
        switch(event.type){
            case SDL_QUIT:
                this->status = false;
                break;
            case SDL_KEYUP:
                input.keys[event.key.keysym.scancode] = true;
                break;
            case SDL_KEYDOWN:
                input.keys[event.key.keysym.scancode] = false;
                break;
            case SDL_MOUSEMOTION:
                input.mouse_delta_x = (float)event.motion.xrel;
                input.mouse_delta_y = (float)event.motion.yrel;
                break;
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

std::pair<int,int> AppWindow::getFrameBufferSize(){
    int width,height;
    SDL_GL_GetDrawableSize(window,&width,&height);
    return std::make_pair(width,height);
}

AppWindow::~AppWindow(){
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit();
}
