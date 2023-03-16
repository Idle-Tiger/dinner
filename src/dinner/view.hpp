#pragma once

#include "booka.hpp"
#include "sdl.hpp"
#include "widget.hpp"

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class View {
public:
    View(booka::Booka& booka);

    bool processInput();
    void present();

    void showTest();

private:
    bool update();

    booka::Booka& _booka;
    booka::Actions::Iterator _actionIterator;
    size_t _backgroundIndex = size_t(-1);

    sdl::Window _window;
    sdl::Renderer _renderer;

    std::vector<std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>> _textures;

    ttf::Font _font;
    std::optional<SpeechBox> _characterBox;
    std::optional<SpeechBox> _speechBox;
    std::unique_ptr<Mix_Music, void(*)(Mix_Music*)> _music {nullptr, Mix_FreeMusic};

    std::vector<std::unique_ptr<Widget>> _widgets;
};
