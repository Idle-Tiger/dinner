#pragma once

#include "sdl.hpp"

class Client {
public:
    Client();

private:
    sdl::Window _window;
    sdl::Renderer _renderer;
};
