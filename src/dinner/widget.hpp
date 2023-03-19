#pragma once

#include "sdl.hpp"

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Widget {
public:
    virtual ~Widget() = default;

    virtual void update([[maybe_unused]] double delta) {}
    virtual void render(sdl::Renderer& renderer) = 0;

    // Always return false if the widget does not process any input. This makes
    // it effectively transparent for input events.
    [[nodiscard]] virtual bool inside(
        [[maybe_unused]] int x, [[maybe_unused]] int y) const
    {
        return false;
    }

    virtual void click() {}

    // A cursor-oriented widget may be in one of the following visual states:
    //   * Unfocused: Its default, normal state. Widget is not being interacted
    //                with.
    //   * Hover:     Cursor is hanging over the widget, not buttons are pressed.
    //   * Pressed:   Widget is pressed, cursor is still hovering over it.
    //   * Lost:      Widget was pressed, but the cursor then left its area.
    virtual void unfocused() {}
    virtual void hovered() {}
    virtual void pressed() {}
    virtual void lost() {}
};

class Button : public Widget {
    static constexpr int borderSize = 5;
    static constexpr int textMargin = 3;
    static constexpr SDL_Color borderColor {30, 30, 30, 255};
    static constexpr SDL_Color insideColor {120, 100, 100, 255};
    static constexpr SDL_Color textColor {0, 0, 0, 255};
    static constexpr SDL_Color hoverInsideColor {100, 200, 100, 255};
    static constexpr SDL_Color pressedInsideColor {80, 80, 80, 255};
    static constexpr SDL_Color lostInsideColor {50, 50, 50, 255};

public:
    Button(
        int x,
        int y,
        int w,
        int h,
        const std::span<const std::byte>& fontData,
        std::string text,
        std::function<void()> action)
        : _outerRect{.x = x, .y = y, .w = w, .h = h}
        , _innerRect{
            .x = _outerRect.x + borderSize,
            .y = _outerRect.y + borderSize,
            .w = _outerRect.w - 2 * borderSize,
            .h = _outerRect.h - 2 * borderSize}
        , _textRect{
            .x = _innerRect.x + textMargin,
            .y = _innerRect.y + textMargin,
            .w = _innerRect.w - 2 * textMargin,
            .h = _innerRect.h - 2 * textMargin}
        , _fontData(fontData)
        , _text(std::move(text))
        , _action(std::move(action))
    { }

    [[nodiscard]] bool inside(int x, int y) const override
    {
        return
            x >= _outerRect.x && x < _outerRect.x + _outerRect.w &&
            y >= _outerRect.y && y < _outerRect.y + _outerRect.h;
    }

    void render(sdl::Renderer& renderer) override
    {
        renderer.fillRect(_outerRect, borderColor);
        renderer.fillRect(_innerRect, _insideColor);

        if (!_textTexture) {
            int optimalFontSize = 0;
            {
                int maxTextWidth = _innerRect.w - 2 * textMargin;
                int maxTextHeight = _innerRect.h - 2 * textMargin;
                int l = 1;
                int r = 50;
                while (l < r) {
                    int m = (l + 1 + r) / 2;
                    auto font = ttf::Font{_fontData, m};
                    auto [w, h] = ttf::sizeUtf8(font, _text);
                    if (w <= maxTextWidth && h <= maxTextHeight) {
                        l = m;
                    } else {
                        r = m - 1;
                    }
                }
                optimalFontSize = l;
            }

            auto font = ttf::Font{_fontData, optimalFontSize};
            auto textSurface = ttf::renderUtf8Blended(
                font,
                _text,
                textColor,
                _innerRect.w - 2 * textMargin);
            check(textSurface->w <= _textRect.w && textSurface->h <= _textRect.h);
            int horizontalGap = _textRect.w - textSurface->w;
            int verticalGap = _textRect.h - textSurface->h;
            _realTextRect = SDL_Rect{
                .x = _textRect.x + horizontalGap / 2,
                .y = _textRect.y + verticalGap / 2,
                .w = textSurface->w,
                .h = textSurface->h};
            _textTexture = renderer.createTextureFromSurface(textSurface);
        }

        renderer.copy(_textTexture, {}, _realTextRect);
    }

    void unfocused() override
    {
        _insideColor = insideColor;
    }

    void hovered() override
    {
        _insideColor = hoverInsideColor;
    }

    void pressed() override
    {
        _insideColor = pressedInsideColor;
    }

    void lost() override
    {
        _insideColor = lostInsideColor;
    }

    void click() override
    {
        _action();
    }

private:
    SDL_Rect _outerRect;
    SDL_Rect _innerRect;
    SDL_Rect _textRect;
    SDL_Rect _realTextRect {};
    std::span<const std::byte> _fontData;
    std::string _text;
    sdl::Texture _textTexture;
    std::function<void()> _action;
    SDL_Color _insideColor = insideColor;
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
        , _borderTexture(_renderer.loadTexture(
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

        _textTexture = _renderer.createTextureFromSurface(textSurface);
    }

    void update(double delta) override
    {
        // TODO: smooth text presentation
        (void)delta;
    }

    void render(sdl::Renderer& renderer) override
    {
        if (!this->_visible) {
            return;
        }

        sdl::check(SDL_SetRenderDrawColor(renderer, 0x99, 0xcc, 0xff, 200));
        renderer.fillRect({
            .x = _frameRect.x + _borderWidth / 2,
            .y = _frameRect.y + _borderHeight / 2,
            .w = _frameRect.w - _borderWidth,
            .h = _frameRect.h - _borderHeight});

        renderer.copy(
            _borderTexture,
            SDL_Rect{.x = 0, .y = 0, .w = _borderWidth, .h = _borderHeight},
            SDL_Rect{.x = _frameRect.x, .y = _frameRect.y, .w = _borderWidth, .h = _borderHeight});
        renderer.copy(
            _borderTexture,
            SDL_Rect{.x = 2 * _borderWidth, .y = 0, .w = _borderWidth, .h = _borderHeight},
            SDL_Rect{.x = _frameRect.x + _frameRect.w - _borderWidth, .y = _frameRect.y, .w = _borderWidth, .h = _borderHeight});
        renderer.copy(
            _borderTexture,
            SDL_Rect{.x = 0, .y = 2 * _borderHeight, .w = _borderWidth, .h = _borderHeight},
            SDL_Rect{.x = _frameRect.x, .y = _frameRect.y + _frameRect.h - _borderHeight, .w = _borderWidth, .h = _borderHeight});
        renderer.copy(
            _borderTexture,
            SDL_Rect{.x = 2 * _borderWidth, .y = 2 * _borderHeight, .w = _borderWidth, .h = _borderHeight},
            SDL_Rect{.x = _frameRect.x + _frameRect.w - _borderWidth, .y = _frameRect.y + _frameRect.h - _borderHeight, .w = _borderWidth, .h = _borderHeight});

        for (int x = _frameRect.x + _borderWidth; x < _frameRect.x + _frameRect.w - _borderWidth; x += _borderWidth) {
            renderer.copy(
                _borderTexture,
                SDL_Rect{.x = _borderWidth, .y = 0, .w = _borderWidth, .h = _borderHeight},
                SDL_Rect{
                    .x = x,
                    .y = _frameRect.y,
                    .w = std::min(_borderWidth, _frameRect.x + _frameRect.w - x),
                    .h = _borderHeight
                });
            renderer.copy(
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
            renderer.copy(
                _borderTexture,
                SDL_Rect{.x = 0, .y = _borderHeight, .w = _borderWidth, .h = _borderHeight},
                SDL_Rect{
                    .x = _frameRect.x,
                    .y = y,
                    .w = _borderWidth,
                    .h = std::min(_borderHeight, _frameRect.y + _frameRect.h - y)
                });
            renderer.copy(
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
            sdl::check(SDL_RenderCopy(renderer, _textTexture, nullptr, &_textRect));
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

class Widgets {
public:
    template <class W, class... Args>
    requires std::derived_from<W, Widget> && std::constructible_from<W, Args...>
    W* add(Args&&... args)
    {
        auto uniquePtr = std::make_unique<W>(std::forward<Args>(args)...);
        W* rawPtr = uniquePtr.get();
        _widgets.push_back(std::move(uniquePtr));
        return rawPtr;
    }

    void render(sdl::Renderer& renderer)
    {
        for (const auto& widget : _widgets) {
            widget->render(renderer);
        }
    }

    void motion(int x, int y)
    {
        if (!_widgetUnderCursor || !_widgetUnderCursor->inside(x, y)) {
            if (_widgetUnderCursor) {
                if (_pressedWidget == _widgetUnderCursor) {
                    _widgetUnderCursor->lost();
                } else {
                    _widgetUnderCursor->unfocused();
                }
            }
            _widgetUnderCursor = nullptr;
            for (const auto& w : _widgets) {
                if (w->inside(x, y)) {
                    _widgetUnderCursor = w.get();
                    break;
                }
            }
            if (_widgetUnderCursor) {
                if (!_pressedWidget) {
                    _widgetUnderCursor->hovered();
                } else if (_widgetUnderCursor == _pressedWidget) {
                    _widgetUnderCursor->pressed();
                }
            }
        }
    }

    bool press(int x, int y)
    {
        if (_pressedWidget) {
            // This should never happen in a normal environment. Consider the
            // old "press" bogus, and revert its effects without "clicking".
            _pressedWidget->unfocused();
            _pressedWidget = nullptr;
        }

        if (_widgetUnderCursor && _widgetUnderCursor->inside(x, y)) {
            _pressedWidget = _widgetUnderCursor;
        } else {
            for (const auto& w : _widgets) {
                if (w->inside(x, y)) {
                    _pressedWidget = w.get();
                    break;
                }
            }
            if (_pressedWidget) {
                if (_widgetUnderCursor) {
                    // This should never happen in a normal environment.
                    // Consider the old widget under cursor inactive now.
                    _widgetUnderCursor->unfocused();
                }
                _widgetUnderCursor = _pressedWidget;
            }
        }

        if (_pressedWidget) {
            _pressedWidget->pressed();
        }

        return _pressedWidget != nullptr;
    }

    void release(int x, int y)
    {
        if (!_widgetUnderCursor || !_widgetUnderCursor->inside(x, y)) {
            if (_widgetUnderCursor) {
                _widgetUnderCursor->unfocused();
                _widgetUnderCursor = nullptr;
            }
            for (const auto& w : _widgets) {
                if (w->inside(x, y)) {
                    _widgetUnderCursor = w.get();
                    break;
                }
            }
        }

        if (_pressedWidget) {
            if (_pressedWidget == _widgetUnderCursor) {
                _pressedWidget->click();
            } else {
                _pressedWidget->unfocused();
            }
        }
        if (_widgetUnderCursor) {
            _widgetUnderCursor->hovered();
        }
        _pressedWidget = nullptr;
    }

private:
    std::vector<std::unique_ptr<Widget>> _widgets;
    Widget* _widgetUnderCursor = nullptr;
    Widget* _pressedWidget = nullptr;
};
