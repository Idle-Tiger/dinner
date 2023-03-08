#pragma once

#include "story.hpp"

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class View {
public:
    View(Story& story);

    bool processInput();
    void present();
    void loadImage(const std::filesystem::path& path);
    void showText(const std::string& text);

    void showTest();

private:
    bool update();

    Story& _story;

    std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> _window {nullptr, SDL_DestroyWindow};
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> _renderer {nullptr, SDL_DestroyRenderer};

    std::vector<std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>> _textures;

    int _backgroundIndex = -1;

    SDL_Rect _textRect {};
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> _font {nullptr, TTF_CloseFont};
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> _text {nullptr, SDL_DestroyTexture};
    std::unique_ptr<Mix_Music, void(*)(Mix_Music*)> _music {nullptr, Mix_FreeMusic};
};
