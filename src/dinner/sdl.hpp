#pragma once

#include "error.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

#include <source_location>

inline void sdlCheck(
    bool condition,
    std::source_location sourceLocation = std::source_location::current())
{
    if (!condition) {
        throw Error{sourceLocation} << SDL_GetError();
    }
}

inline void sdlCheck(
    int code,
    std::source_location sourceLocation = std::source_location::current())
{
    sdlCheck(code == 0, sourceLocation);
}

template <class T>
T* sdlCheck(
    T* ptr,
    std::source_location sourceLocation = std::source_location::current())
{
    if (!ptr) {
        throw Error{sourceLocation} << SDL_GetError();
    }
    return ptr;
}

inline void ttfCheck(
    int code,
    std::source_location sourceLocation = std::source_location::current())
{
    if (code != 0) {
        throw Error{sourceLocation} << TTF_GetError();
    }
}

template <class T>
T* ttfCheck(
    T* ptr,
    std::source_location sourceLocation = std::source_location::current())
{
    if (!ptr) {
        throw Error{sourceLocation} << TTF_GetError();
    }
    return ptr;
}
