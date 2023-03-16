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
    _window = sdl::Window{
        config().windowTitle,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        config().windowWidth,
        config().windowHeight,
        createWindowFlags};
    _renderer = sdl::Renderer{
        _window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC};

    sdl::check(SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND));

    _font = ttf::Font{
        bi::SOURCE_ROOT /
            "assets/test-level/fonts/open-sans/OpenSans-Regular.ttf",
        32};

    _speechBox.emplace(
        _renderer,
        50, 780, 1820, 250,
        _font,
        SDL_Color{0, 0, 0, 255});

    update();

    for (const auto& image : _booka.images()) {
        std::cout << "loading image '" << image.name << "', " <<
            Size{image.data.size()} << "\n";
        _textures.push_back(
            std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>{
                sdl::check(IMG_LoadTexture_RW(
                    _renderer,
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
        if (event.type == SDL_QUIT) {
            return false;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {
            return update();
        }
    }

    return true;
}

void View::present()
{
    sdl::check(SDL_SetRenderDrawColor(_renderer, 50, 50, 50, 255));
    sdl::check(SDL_RenderClear(_renderer));

    if (_backgroundIndex != size_t(-1)) {
        sdl::check(SDL_RenderCopy(
            _renderer,
            _textures.at(_backgroundIndex).get(),
            nullptr,
            nullptr));
    }

    if (_speechBox.value().visible) {
        _speechBox.value().render();
    }

    SDL_RenderPresent(_renderer);
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
                _speechBox.value().visible = false;
            },
            [&] (const booka::ShowTextAction& showTextAction) {
                std::cout << "show text action\n";
                auto s = std::string{showTextAction.character} + ": " +
                    std::string{showTextAction.text};
                _speechBox.value().showText(s);
            },
            [&] (const booka::PlayMusicAction& playMusicAction) {
                auto music = _booka.music()[playMusicAction.musicIndex];
                _music.reset(sdl::check(Mix_LoadMUS_RW(
                    SDL_RWFromMem((void*)music.data.data(), (int)music.data.size()),
                    1)));
                sdl::check(Mix_PlayMusic(_music.get(), -1));
                repeat = true;
            },
        }, *_actionIterator++);

        if (!repeat) {
            break;
        }
    }

    return true;
}
