#include "view.hpp"

#include "build-info.hpp"
#include "config.hpp"
#include "logging.hpp"
#include "overloaded.hpp"
#include "repa.hpp"
#include "resources.hpp"
#include "sdl.hpp"

#include <SDL_image.h>

#include <iostream>
#include <variant>

View::View(booka::Booka& booka)
    : _booka(booka)
    , _actionIterator(_booka.actions().begin())
    , _repa(bi::BUILD_ROOT / "assets" / "resources.fb")
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

    _font = ttf::Font{_repa((size_t)R::FONT_OPEN_SANS), 32};

    _characterBox = _widgets.add<SpeechBox>(
        _renderer,
        50, 700, 300, 80,
        _font,
        SDL_Color{0, 0, 0, 255},
        SpeechBox::Mode::Flexi);

    _speechBox = _widgets.add<SpeechBox>(
        _renderer,
        50, 780, 1824, 256,
        _font,
        SDL_Color{0, 0, 0, 255},
        SpeechBox::Mode::Wrappy);

    _widgets.add<Button>(
        1670, 50, 200, 50,
        _repa((size_t)R::FONT_OPEN_SANS),
        "Quit",
        [this] {
            std::cout << "quit button pressed\n";
            _signalToExit = true;
        });

    update();

    for (const auto& image : _booka.images()) {
        std::cout << "loading image '" << image.name << "', " <<
            Size{image.data.size()} << "\n";
        _textures.push_back(_renderer.loadTextureFromMemory(image.data));
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
            bool processed = _widgets.press(event.button.x, event.button.y);
            if (!processed) {
                if (!update()) {
                    return false;
                }
            }
        } else if (event.type == SDL_MOUSEBUTTONUP &&
                event.button.button == SDL_BUTTON_LEFT) {
            _widgets.release(event.button.x, event.button.y);
        } else if (event.type == SDL_MOUSEMOTION) {
            _widgets.motion(event.motion.x, event.motion.y);
        }
    }

    return !_signalToExit;
}

void View::present()
{
    _renderer.setDrawColor(50, 50, 50, 255);
    _renderer.clear();

    if (_backgroundIndex != size_t(-1)) {
        _renderer.copy(_textures.at(_backgroundIndex), {}, {});
    }

    _widgets.render(_renderer);

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
                _speechBox->hide();


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
                _speechBox->showText(std::string{showTextAction.text});
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
