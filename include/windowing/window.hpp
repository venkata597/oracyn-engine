#pragma once

#if defined (_WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
    #define SDL_MAIN_HANDLED
#elif defined (__linux__)
    #define VK_USE_PLATFORM_XLIB_KHR
    #define VK_USE_PLATFORM_WAYLAND_KHR
#elif defined (__APPLE__)
    #define VK_USE_PLATFORM_METAL_EXT
#endif
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_error.h>
#include <vector>

#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 728



class AppWindow{
private:
    SDL_Window* window = nullptr;
    SDL_Event event;
    bool status = true;
private:
    void _create_window(const char* title);

public:
    AppWindow(const char* title);
    ~AppWindow();

    bool isOpen();
    void handleEvents();
    void swapBuffers();
    std::vector<const char*> getExtensions();
    std::pair<SDL_Window*,std::vector<const char*>> getVulkanInitData();
    VkSurfaceKHR createVulkanSurface(const VkInstance& instance);
    std::pair<int,int> getFrameBufferSize();
};