#pragma once

#include "error.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <filesystem>
#include <memory>
#include <optional>
#include <source_location>
#include <span>
#include <tuple>

namespace sdl {

inline void check(
    bool condition,
    std::source_location sourceLocation = std::source_location::current())
{
    if (!condition) {
        throw Error{sourceLocation} << SDL_GetError();
    }
}

inline void check(
    int code,
    std::source_location sourceLocation = std::source_location::current())
{
    check(code == 0, sourceLocation);
}

template <class T>
T* check(
    T* ptr,
    std::source_location sourceLocation = std::source_location::current())
{
    if (!ptr) {
        throw Error{sourceLocation} << SDL_GetError();
    }
    return ptr;
}

class Surface {
public:
    Surface() = default;

    explicit Surface(SDL_Surface* raw)
    {
        _ptr.reset(raw);
    }

    SDL_Surface* operator->() { return _ptr.get(); }

    operator SDL_Surface*() { return _ptr.get(); }
    operator const SDL_Surface*() const { return _ptr.get(); }

private:
    std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> _ptr {
        nullptr, SDL_FreeSurface};
};

class Texture {
public:
    Texture() = default;

    explicit Texture(SDL_Texture* raw)
    {
        _ptr.reset(raw);
    }

    explicit operator bool() const noexcept { return !!_ptr; }
    operator SDL_Texture*() { return _ptr.get(); }
    operator const SDL_Texture*() const { return _ptr.get(); }

private:
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> _ptr {
        nullptr, SDL_DestroyTexture};
};

class Window {
public:
    Window() = default;

    Window(
        const std::string& title,
        int x,
        int y,
        int w,
        int h,
        uint32_t flags,
        std::source_location sourceLocation = std::source_location::current())
    {
        _ptr.reset(check(
            SDL_CreateWindow(title.c_str(), x, y, w, h, flags),
            sourceLocation));
    }

    operator SDL_Window*() { return _ptr.get(); }
    operator const SDL_Window*() const { return _ptr.get(); }

private:
    std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> _ptr {
        nullptr, SDL_DestroyWindow};
};

class Renderer {
public:
    Renderer() = default;

    Renderer(
        Window& window,
        int index,
        uint32_t flags,
        std::source_location sourceLocation = std::source_location::current())
    {
        _ptr.reset(check(
            SDL_CreateRenderer(window, index, flags),
            sourceLocation));
    }

    operator SDL_Renderer*() { return _ptr.get(); }
    operator const SDL_Renderer*() const { return _ptr.get(); }

    void setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        sdl::check(SDL_SetRenderDrawColor(_ptr.get(), r, g, b, a));
    }

    void setDrawColor(const SDL_Color& color)
    {
        setDrawColor(color.r, color.g, color.b, color.a);
    }

    void clear()
    {
        sdl::check(SDL_RenderClear(_ptr.get()));
    }

    void fillRect(
        const SDL_Rect& rect,
        const std::optional<SDL_Color>& color = std::nullopt)
    {
        if (color) {
            setDrawColor(*color);
        }
        check(SDL_RenderFillRect(_ptr.get(), &rect));
    }

    void copy(
        Texture& texture,
        const std::optional<SDL_Rect>& srcrect,
        const std::optional<SDL_Rect>& dstrect)
    {
        const SDL_Rect* s = srcrect ? &*srcrect : nullptr;
        const SDL_Rect* d = dstrect ? &*dstrect : nullptr;
        check(SDL_RenderCopy(_ptr.get(), texture, s, d));
    }

    void present()
    {
        SDL_RenderPresent(_ptr.get());
    }

    sdl::Texture loadTexture(const std::filesystem::path& path)
    {
        return sdl::Texture{
            sdl::check(IMG_LoadTexture(_ptr.get(), path.string().c_str()))
        };
    }

    sdl::Texture loadTextureFromMemory(const std::span<const std::byte>& data)
    {
        return sdl::Texture{
            sdl::check(IMG_LoadTexture_RW(
                _ptr.get(),
                sdl::check(SDL_RWFromMem(
                    (void*)data.data(),
                    static_cast<int>(data.size()))),
                1 /* freesrc */))
        };
    }

    sdl::Texture createTextureFromSurface(sdl::Surface& surface)
    {
        return sdl::Texture{
            sdl::check(SDL_CreateTextureFromSurface(_ptr.get(), surface))
        };
    }

private:
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> _ptr {
        nullptr, SDL_DestroyRenderer};
};

} // namespace sdl

namespace ttf {

inline void check(
    int code,
    std::source_location sourceLocation = std::source_location::current())
{
    if (code != 0) {
        throw Error{sourceLocation} << TTF_GetError();
    }
}

template <class T>
T* check(
    T* ptr,
    std::source_location sourceLocation = std::source_location::current())
{
    if (!ptr) {
        throw Error{sourceLocation} << TTF_GetError();
    }
    return ptr;
}

class Font {
public:
    Font() = default;

    Font(
        const std::span<const std::byte>& data,
        int ptsize,
        std::source_location sourceLocation = std::source_location::current())
    {
        _ptr.reset(check(
            TTF_OpenFontRW(
                sdl::check(SDL_RWFromMem((void*)data.data(), (int)data.size())),
                1,
                ptsize),
            sourceLocation));
    }

    Font(
        const std::filesystem::path& path,
        int ptsize,
        std::source_location sourceLocation = std::source_location::current())
    {
        _ptr.reset(check(
            TTF_OpenFont(path.string().c_str(), ptsize),
            sourceLocation));
    }

    operator TTF_Font*() { return _ptr.get(); }
    operator const TTF_Font*() const { return _ptr.get(); }

private:
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> _ptr {nullptr, TTF_CloseFont};
};

inline sdl::Surface renderUtf8Lcd(
    Font& font,
    const std::string& text,
    const SDL_Color& fg,
    const SDL_Color& bg,
    uint32_t wrapLength)
{
    if (wrapLength > 0) {
        return sdl::Surface{
            ttf::check(TTF_RenderUTF8_LCD_Wrapped(
                font, text.c_str(), fg, bg, wrapLength))
        };
    }

    return sdl::Surface{
        ttf::check(TTF_RenderUTF8_LCD(font, text.c_str(), fg, bg))
    };
}

inline sdl::Surface renderUtf8Blended(
    Font& font,
    const std::string& text,
    const SDL_Color& fg,
    uint32_t wrapLength)
{
    if (wrapLength > 0) {
        return sdl::Surface{
            ttf::check(TTF_RenderUTF8_Blended_Wrapped(
                font, text.c_str(), fg, wrapLength))
        };
    }

    return sdl::Surface{
        ttf::check(TTF_RenderUTF8_Blended(font, text.c_str(), fg))
    };
}

inline std::tuple<int, int> sizeUtf8(Font& font, const std::string& text)
{
    int w = 0;
    int h = 0;
    ttf::check(TTF_SizeUTF8(font, text.c_str(), &w, &h));
    return {w, h};
}

} // namespace ttf
