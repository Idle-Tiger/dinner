#pragma once

#include "sdl.hpp"

#include <cstdint>
#include <string>

class Widget {
public:
    virtual ~Widget() = default;

    virtual void update([[maybe_unused]] double delta) {}
    virtual void render() = 0;

    virtual bool click([[maybe_unused]] int32_t x, [[maybe_unused]] int32_t y)
    {
        return false;
    }

    bool visible = true;
};

class SpeechBox : public Widget {
    static constexpr int marginPx = 10;

public:
    SpeechBox(
        sdl::Renderer& renderer,
        int x,
        int y,
        int w,
        int h,
        ttf::Font& font,
        SDL_Color textColor)
        : _renderer(renderer)
        , _frameRect{x, y, w, h}
        , _font(font)
        , _textColor(textColor)
    { }

    void showText(const std::string& text)
    {
        this->visible = true;

        auto textSurface = sdl::Surface{
            ttf::check(TTF_RenderUTF8_Blended_Wrapped(
                _font,
                text.c_str(),
                _textColor,
                _frameRect.w - marginPx * 2))
        };

        _textRect = SDL_Rect{
            .x = _frameRect.x + marginPx,
            .y = _frameRect.y + marginPx,
            .w = textSurface->w,
            .h = textSurface->h
        };

        _textTexture = sdl::Texture{
            sdl::check(SDL_CreateTextureFromSurface(_renderer, textSurface))};
    }

    void update(double delta) override
    {
        // TODO: smooth text presentation
        (void)delta;
    }

    void render() override
    {
        sdl::check(SDL_SetRenderDrawColor(_renderer, 0x99, 0xcc, 0xff, 200));
        sdl::check(SDL_RenderFillRect(_renderer, &_frameRect));
        if (_textTexture) {
            sdl::check(SDL_RenderCopy(_renderer, _textTexture, nullptr, &_textRect));
        }
    }

private:
    sdl::Renderer& _renderer;
    SDL_Rect _frameRect;
    ttf::Font& _font;
    SDL_Color _textColor;
    SDL_Rect _textRect {};
    sdl::Texture _textTexture;
};
