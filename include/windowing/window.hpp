#pragma once
#include "../resources/inputhandler.hpp"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_error.h>
#include <utility>

#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 728

class AppWindow{
private:
    SDL_Window* window = nullptr;
    SDL_Event event;
    bool status = true;
    bool resized = false;

    InputHandler& input_ref;

    SDL_GLContext glContext;
private:
    void _create_window(const char* title);

public:
    AppWindow(const char* title,InputHandler& input);
    ~AppWindow();

    bool isOpen();
    void handleEvents();
    void swapBuffers();
    bool isResized();
    void clearResize();
    std::pair<int,int> getFrameBufferSize();
};
