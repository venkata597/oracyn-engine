#include <SDL_scancode.h>

class InputHandler{
public:
    bool keys[SDL_NUM_SCANCODES];
};


extern InputHandler input;
