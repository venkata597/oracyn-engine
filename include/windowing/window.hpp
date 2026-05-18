#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_error.h>

#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 728


class Window{
private:
    SDL_Window* window = nullptr;
    SDL_Event event;
    bool status = true;

private:
    void _create_window(const char* title);

public:
    Window(const char* title);
    ~Window();

    bool isOpen();
    void handleEvents();
};