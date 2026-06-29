#pragma once

#include <SDL2/SDL_scancode.h>

class InputHandler{
public:
    bool keys[SDL_NUM_SCANCODES];

    float mouse_delta_x = 0.0f;
    float mouse_delta_y = 0.0f;

    void resetMouseDelta() {mouse_delta_x = 0.0f;mouse_delta_y=0.0f;}
};


extern InputHandler input;
