#include "view.hpp"

#include "config.hpp"
#include "sdl.hpp"

View::View()
    : _window(nullptr, SDL_DestroyWindow)
    , _renderer(nullptr, SDL_DestroyRenderer)
{
    _window.reset(sdlCheck(SDL_CreateWindow(
        config().windowTitle.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        config().windowWidth,
        config().windowHeight,
        0)));
    _renderer.reset(sdlCheck(SDL_CreateRenderer(
        _window.get(),
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)));
}

bool View::processInput()
{
    auto event = SDL_Event{};
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return false;
        }
    }

    return true;
}

void View::present()
{
    sdlCheck(SDL_SetRenderDrawColor(_renderer.get(), 50, 50, 50, 255));
    sdlCheck(SDL_RenderClear(_renderer.get()));
    SDL_RenderPresent(_renderer.get());
}
