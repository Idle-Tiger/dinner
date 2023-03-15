#include "view.hpp"

#include "build-info.hpp"
#include "config.hpp"
#include "logging.hpp"
#include "overloaded.hpp"
#include "sdl.hpp"

#include <SDL_image.h>

#include <iostream>
#include <variant>

View::View(booka::Booka& booka)
    : _booka(booka)
    , _actionIterator(_booka.actions().begin())
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

    _font.reset(ttfCheck(TTF_OpenFont(
        (bi::SOURCE_ROOT /
            "assets/test-level/fonts/open-sans/OpenSans-Regular.ttf").string().c_str(),
        32)));

    update();

    for (const auto& image : _booka.images()) {
        std::cout << "loading image '" << image.name << "', " <<
            Size{image.data.size()} << "\n";
        _textures.push_back(
            std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>{
                sdlCheck(IMG_LoadTexture_RW(
                    _renderer.get(),
                    SDL_RWFromMem((void*)image.data.data(), (int)image.data.size()),
                    1)),
                SDL_DestroyTexture
            });
    }
}

bool View::processInput()
{
    auto event = SDL_Event{};
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return false;
            case SDL_MOUSEBUTTONDOWN:
                return update();
        }
    }

    return true;
}

void View::present()
{
    sdlCheck(SDL_SetRenderDrawColor(_renderer.get(), 50, 50, 50, 255));
    sdlCheck(SDL_RenderClear(_renderer.get()));

    if (_backgroundIndex != size_t(-1)) {
        sdlCheck(SDL_RenderCopy(
            _renderer.get(),
            _textures.at(_backgroundIndex).get(),
            nullptr,
            nullptr));
    }

    if (_text) {
        const auto textBackgroundRect = SDL_Rect{50, 780, 1820, 250};
        sdlCheck(SDL_SetRenderDrawColor(_renderer.get(), 0x99, 0xcc, 0xff, 200));
        sdlCheck(SDL_RenderFillRect(_renderer.get(), &textBackgroundRect));
        sdlCheck(SDL_RenderCopy(_renderer.get(), _text.get(), nullptr, &_textRect));
    }

    SDL_RenderPresent(_renderer.get());
}

void View::showText(const std::string& text)
{
    SDL_Surface* textSurface = ttfCheck(TTF_RenderUTF8_Blended_Wrapped(
        _font.get(),
        text.c_str(),
        SDL_Color{0, 0, 0, 255},
        1780));
    _textRect = SDL_Rect{70, 800, textSurface->w, textSurface->h};
    _text.reset(sdlCheck(
        SDL_CreateTextureFromSurface(_renderer.get(), textSurface)));
    SDL_FreeSurface(textSurface);
}

bool View::update()
{
    if (_actionIterator == _booka.actions().end()) {
        return false;
    }

    for (;;) {
        bool repeat = false;
        std::visit(Overloaded{
            [&] (const booka::ShowImageAction& showImageAction) {
                std::cout << "show image action\n";
                _backgroundIndex = showImageAction.imageIndex;
                _text.reset();
            },
            [&] (const booka::ShowTextAction& showTextAction) {
                std::cout << "show text action\n";
                auto s = std::string{showTextAction.character} + ": " +
                    std::string{showTextAction.text};
                this->showText(s);
            },
            [&] (const booka::PlayMusicAction& playMusicAction) {
                auto music = _booka.music()[playMusicAction.musicIndex];
                _music.reset(sdlCheck(Mix_LoadMUS_RW(
                    SDL_RWFromMem((void*)music.data.data(), (int)music.data.size()),
                    1)));
                sdlCheck(Mix_PlayMusic(_music.get(), -1));
                repeat = true;
            },
        }, *_actionIterator++);

        if (!repeat) {
            break;
        }
    }

    return true;
}
