#pragma once

#include "error.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

#include <filesystem>
#include <memory>
#include <source_location>

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

private:
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> _ptr {
        nullptr, SDL_DestroyRenderer};
};

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

} // namespace ttf
