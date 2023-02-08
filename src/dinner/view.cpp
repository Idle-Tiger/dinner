#include "view.hpp"

#include "build-info.hpp"
#include "config.hpp"
#include "sdl.hpp"

#include <SDL_image.h>

View::View()
{
    auto createWindowFlags = Uint32{0};
    if (config().fullscreen) {
        createWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    _window.reset(sdlCheck(SDL_CreateWindow(
        config().windowTitle.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        config().windowWidth,
        config().windowHeight,
        createWindowFlags)));
    _renderer.reset(sdlCheck(SDL_CreateRenderer(
        _window.get(),
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)));

    sdlCheck(SDL_SetRenderDrawBlendMode(_renderer.get(), SDL_BLENDMODE_BLEND));

    _bird.reset(sdlCheck(IMG_LoadTexture(
        _renderer.get(),
        (bi::SOURCE_ROOT / "assets/test-level/bg/bird.png").c_str())));
    _elephant.reset(sdlCheck(IMG_LoadTexture(
        _renderer.get(),
        (bi::SOURCE_ROOT / "assets/test-level/bg/elephant.png").c_str())));
    _mouse.reset(sdlCheck(IMG_LoadTexture(
        _renderer.get(),
        (bi::SOURCE_ROOT / "assets/test-level/bg/mouse.png").c_str())));
    _teaTable.reset(sdlCheck(IMG_LoadTexture(
        _renderer.get(),
        (bi::SOURCE_ROOT / "assets/test-level/bg/tea-table.png").c_str())));

    _font.reset(ttfCheck(TTF_OpenFont(
        (bi::SOURCE_ROOT /
            "assets/test-level/fonts/open-sans/OpenSans-Regular.ttf").c_str(),
        32)));

    {
        SDL_Surface* textSurface = ttfCheck(TTF_RenderUTF8_Blended_Wrapped(
            _font.get(),
            "Как хорошо, что все зверики собрались у меня сегодня!",
            SDL_Color{0, 0, 0, 255},
            1780));
        _textRect = SDL_Rect{70, 800, textSurface->w, textSurface->h};
        _text.reset(sdlCheck(
            SDL_CreateTextureFromSurface(_renderer.get(), textSurface)));
        SDL_FreeSurface(textSurface);
    }

    _music.reset(sdlCheck(Mix_LoadMUS(
        (bi::SOURCE_ROOT / "assets/test-level/music/bg.wav").c_str())));

    sdlCheck(Mix_PlayMusic(_music.get(), -1));
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

    sdlCheck(SDL_RenderCopy(_renderer.get(), _teaTable.get(), nullptr, nullptr));

    const auto textBackgroundRect = SDL_Rect{50, 780, 1820, 250};
    sdlCheck(SDL_SetRenderDrawColor(_renderer.get(), 0x99, 0xcc, 0xff, 200));
    sdlCheck(SDL_RenderFillRect(_renderer.get(), &textBackgroundRect));

    sdlCheck(SDL_RenderCopy(_renderer.get(), _text.get(), nullptr, &_textRect));

    SDL_RenderPresent(_renderer.get());
}
