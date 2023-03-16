#pragma once

#include "sdl.hpp"

#include <algorithm>
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
};

class SpeechBox : public Widget {
    static constexpr int horizontalMarginPx = 15;
    static constexpr int verticalMarginPx = 0;

public:
    enum class Mode {
        Flexi,
        Wrappy,
    };

    SpeechBox(
        sdl::Renderer& renderer,
        int x, int y, int w, int h,
        ttf::Font& font,
        SDL_Color textColor,
        Mode mode)
        : _renderer(renderer)
        , _frameRect{x, y, w, h}
        , _font(font)
        , _textColor(textColor)
        , _borderTexture(img::loadTexture(
            _renderer,
            bi::SOURCE_ROOT / "assets" / "textures" / "border.png"))
        , _mode(mode)
    {
        int borderTextureWidth = 0;
        int borderTextureHeight = 0;
        sdl::check(SDL_QueryTexture(
            _borderTexture,
            nullptr,
            nullptr,
            &borderTextureWidth,
            &borderTextureHeight));
        _borderWidth = borderTextureWidth / 3;
        _borderHeight = borderTextureHeight / 3;
    }

    void hide()
    {
        _visible = false;
    }

    void showText(const std::string& text)
    {
        _visible = true;

        sdl::Surface textSurface;
        switch (_mode) {
            case Mode::Flexi:
            {
                textSurface = sdl::Surface{
                    ttf::check(TTF_RenderUTF8_Blended(
                        _font, text.c_str(), _textColor))
                };
                _frameRect.w = textSurface->w + 2 * horizontalMarginPx + _borderWidth;
                if (int res = _frameRect.w % _borderWidth; res > 0) {
                    _frameRect.w += _borderWidth - res;
                }
                break;
            }
            case Mode::Wrappy:
            {
                textSurface = sdl::Surface{
                    ttf::check(TTF_RenderUTF8_Blended_Wrapped(
                        _font,
                        text.c_str(),
                        _textColor,
                        _frameRect.w - horizontalMarginPx * 2))
                };
                break;
            }
        }

        _textRect = SDL_Rect{
            .x = _frameRect.x + _borderWidth + horizontalMarginPx,
            .y = _frameRect.y + _borderHeight + verticalMarginPx,
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
        if (!this->_visible) {
            return;
        }

        sdl::check(SDL_SetRenderDrawColor(_renderer, 0x99, 0xcc, 0xff, 200));
        _renderer.fillRect({
            .x = _frameRect.x + _borderWidth / 2,
            .y = _frameRect.y + _borderHeight / 2,
            .w = _frameRect.w - _borderWidth,
            .h = _frameRect.h - _borderHeight});

        _renderer.copy(
            _borderTexture,
            SDL_Rect{.x = 0, .y = 0, .w = _borderWidth, .h = _borderHeight},
            SDL_Rect{.x = _frameRect.x, .y = _frameRect.y, .w = _borderWidth, .h = _borderHeight});
        _renderer.copy(
            _borderTexture,
            SDL_Rect{.x = 2 * _borderWidth, .y = 0, .w = _borderWidth, .h = _borderHeight},
            SDL_Rect{.x = _frameRect.x + _frameRect.w - _borderWidth, .y = _frameRect.y, .w = _borderWidth, .h = _borderHeight});
        _renderer.copy(
            _borderTexture,
            SDL_Rect{.x = 0, .y = 2 * _borderHeight, .w = _borderWidth, .h = _borderHeight},
            SDL_Rect{.x = _frameRect.x, .y = _frameRect.y + _frameRect.h - _borderHeight, .w = _borderWidth, .h = _borderHeight});
        _renderer.copy(
            _borderTexture,
            SDL_Rect{.x = 2 * _borderWidth, .y = 2 * _borderHeight, .w = _borderWidth, .h = _borderHeight},
            SDL_Rect{.x = _frameRect.x + _frameRect.w - _borderWidth, .y = _frameRect.y + _frameRect.h - _borderHeight, .w = _borderWidth, .h = _borderHeight});

        for (int x = _frameRect.x + _borderWidth; x < _frameRect.x + _frameRect.w - _borderWidth; x += _borderWidth) {
            _renderer.copy(
                _borderTexture,
                SDL_Rect{.x = _borderWidth, .y = 0, .w = _borderWidth, .h = _borderHeight},
                SDL_Rect{
                    .x = x,
                    .y = _frameRect.y,
                    .w = std::min(_borderWidth, _frameRect.x + _frameRect.w - x),
                    .h = _borderHeight
                });
            _renderer.copy(
                _borderTexture,
                SDL_Rect{.x = _borderWidth, .y = 2 * _borderHeight, .w = _borderWidth, .h = _borderHeight},
                SDL_Rect{
                    .x = x,
                    .y = _frameRect.y + _frameRect.h - _borderHeight,
                    .w = std::min(_borderWidth, _frameRect.x + _frameRect.w - x),
                    .h = _borderHeight
                });
        }
        for (int y = _frameRect.y + _borderHeight; y < _frameRect.y + _frameRect.h - _borderHeight; y += _borderHeight) {
            _renderer.copy(
                _borderTexture,
                SDL_Rect{.x = 0, .y = _borderHeight, .w = _borderWidth, .h = _borderHeight},
                SDL_Rect{
                    .x = _frameRect.x,
                    .y = y,
                    .w = _borderWidth,
                    .h = std::min(_borderHeight, _frameRect.y + _frameRect.h - y)
                });
            _renderer.copy(
                _borderTexture,
                SDL_Rect{.x = 2 * _borderWidth, .y = _borderHeight, .w = _borderWidth, .h = _borderHeight},
                SDL_Rect{
                    .x = _frameRect.x + _frameRect.w - _borderWidth,
                    .y = y,
                    .w = _borderWidth,
                    .h = std::min(_borderHeight, _frameRect.y + _frameRect.h - y)
                });
        }

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
    sdl::Texture _borderTexture;
    int _borderWidth = 0;
    int _borderHeight = 0;
    Mode _mode;
    bool _visible = true;
};
