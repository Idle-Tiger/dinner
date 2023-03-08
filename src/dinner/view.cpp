#include "view.hpp"

#include "build-info.hpp"
#include "config.hpp"
#include "overloaded.hpp"
#include "sdl.hpp"

#include <SDL_image.h>

#include <iostream>

View::View(Story& story)
    : _story(story)
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

    _music.reset(sdlCheck(Mix_LoadMUS(
        (bi::SOURCE_ROOT / "assets/test-level/music/bg.wav").string().c_str())));

    sdlCheck(Mix_PlayMusic(_music.get(), -1));

    update();
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

    if (_backgroundIndex >= 0) {
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

void View::loadImage(const std::filesystem::path& path)
{
    _textures.push_back(std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>{
        sdlCheck(IMG_LoadTexture(_renderer.get(), path.string().c_str())),
        SDL_DestroyTexture});
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
    bool done = false;
    std::visit(Overloaded{
        [this](const ShowBackground& bg) {
            _backgroundIndex = bg.id;
            _text.reset();
            std::cout << "showing background: " << _backgroundIndex << "\n";
            _story.next();
        },
        [this](const Text& text) {
            auto s = std::to_string(text.character) + ": " + text.text;
            std::cout << "showing text: " << s << "\n";
            this->showText(s);
            _story.next();
        },
        [this](const Choice& choice) {
            auto text = std::ostringstream{};
            for (size_t i = 0; i < choice.selections.size(); i++) {
                text << i << ": " <<
                    choice.selections.at(i).text << "\n";
            }
            std::cout << "showing text: " << text.str() << "\n";
            this->showText(text.str());
            _story.select(0);
        },
        [&done](const Finish&) {
            std::cout << "done\n";
            done = true;
        },
    }, _story.action());

    return !done;
}
