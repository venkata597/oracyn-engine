#include <SDL2/SDL_timer.h>
#include <iostream>
#include <iterator>
#include "../include/oracyn.hpp"


int main(){
    AppWindow window("Test Engine");
    Scene scene;
    scene.loadScene("scenes/scene.json");
    Camera camera;
    Renderer renderer;
    Backend::GLRenderDevice render_device;
    RenderContext context;
    context.uploadToGPU(scene,render_device);

    float lastFrame = 0.0f;
    float deltaTime = 0.0f;


    while(window.isOpen()){
        window.handleEvents();

        float currentFrame = SDL_GetTicks() / 1000.0f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        camera.update(deltaTime);

        renderer.beginFrame(camera);
        renderer.draw(scene.getScene());
        renderer.endFrame();

        render_device.clearScreen();
        render_device.submitState(renderer.getState());
        render_device.drawScene();

        window.swapBuffers();
    }
}
