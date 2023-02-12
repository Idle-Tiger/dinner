add_subdirectory(SDL)

set(SDL2IMAGE_INSTALL FALSE)
add_subdirectory(SDL_image)

set(SDL2MIXER_INSTALL FALSE)
set(SDL2MIXER_VENDORED TRUE)
add_subdirectory(SDL_mixer)

set(SDL2TTF_INSTALL FALSE)
add_subdirectory(SDL_ttf)

add_subdirectory(yaml-cpp)
