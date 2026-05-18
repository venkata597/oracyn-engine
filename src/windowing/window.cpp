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
}

Window::Window(const char* title){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "[ORACYN (WINDOWING)]: Unable to Initialize SDL_VIDEO" << SDL_GetError() << '\n';
    }
    _create_window(title);
}

void Window::handleEvents(){
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            this->status = false;
        }
    }
}

bool Window::isOpen(){
    return status;
}

Window::~Window(){
    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit();
}