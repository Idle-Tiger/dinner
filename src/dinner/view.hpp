#pragma once

#include <SDL.h>

#include <memory>

class View {
public:
    View();

    bool processInput();
    void present();

private:
    std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> _window;
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> _renderer;
};
