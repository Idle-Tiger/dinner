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

    _characterBox.emplace(
        _renderer,
        50, 700, 300, 80,
        _font,
        SDL_Color{0, 0, 0, 255},
        SpeechBox::Mode::Flexi);

    _speechBox.emplace(
        _renderer,
        50, 780, 1824, 256,
        _font,
        SDL_Color{0, 0, 0, 255},
        SpeechBox::Mode::Wrappy);

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
        if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN &&
                    event.key.keysym.sym == SDLK_ESCAPE)) {
            return false;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {
            if (!update()) {
                return false;
            }
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

    _characterBox.value().render();
    _speechBox.value().render();

    _renderer.present();
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
                _speechBox.value().hide();


                // TODO: remove
                repeat = true;
            },
            [&] (const booka::ShowTextAction& showTextAction) {
                if (showTextAction.character.empty()) {
                    _characterBox->hide();
                } else {
                    _characterBox->showText(std::string{showTextAction.character});
                }

                std::cout << "show text action\n";
                _speechBox.value().showText(std::string{showTextAction.text});
            },
            [&] (const booka::PlayMusicAction& playMusicAction) {
                auto music = _booka.music()[playMusicAction.musicIndex];
                _music.reset(sdl::check(Mix_LoadMUS_RW(
                    SDL_RWFromMem((void*)music.data.data(), (int)music.data.size()),
                    1)));
                if (!config().mute) {
                    sdl::check(Mix_PlayMusic(_music.get(), -1));
                }
                repeat = true;
            },
        }, *_actionIterator++);

        if (!repeat) {
            break;
        }
    }

    return true;
}
