#include "../../include/windowing/window.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <iostream>


void Window::_create_window(const char* title){
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if(window==NULL){
        std::cout << "[ORACYN (WINDOWING)]: Unable to create window" << SDL_GetError() << '\n';
        this->status = false;
    }
    else{
        SDL_PumpEvents();
    }

    glcontext = SDL_GL_CreateContext(window);
}

Window::Window(const char* title){
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

void Window::handleEvents(){
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            this->status = false;
        }
    }
}

void Window::swapBuffers(){
    SDL_GL_SwapWindow(window);
}

bool Window::isOpen(){
    return status;
}

Window::~Window(){
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit();
}