#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <memory>

class View {
public:
    View();

    bool processInput();
    void present();

    void showTest();

private:
    std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> _window {nullptr, SDL_DestroyWindow};
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> _renderer {nullptr, SDL_DestroyRenderer};

    // For testing purposes only, until proper resource management is introduced
    // TODO: remove
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> _bird {nullptr, SDL_DestroyTexture};
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> _elephant {nullptr, SDL_DestroyTexture};
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> _mouse {nullptr, SDL_DestroyTexture};
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> _teaTable {nullptr, SDL_DestroyTexture};
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> _font {nullptr, TTF_CloseFont};
    SDL_Rect _textRect {};
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> _text {nullptr, SDL_DestroyTexture};
    std::unique_ptr<Mix_Music, void(*)(Mix_Music*)> _music {nullptr, Mix_FreeMusic};
};
