#include "config.hpp"
#include "overloaded.hpp"
#include "sdl.hpp"
#include "view.hpp"

#include <booka.hpp>

#include <arg.hpp>
#include <tempo.hpp>

#include <SDL_image.h>
#include <SDL_mixer.h>

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char* argv[]) try
{
    auto storyFilePath = arg::option<fs::path>()
        .keys("--story")
        .defaultValue(bi::BUILD_ROOT / "assets" / "stories" / "test-1.booka")
        .help("path to story booka file");
    arg::parse(argc, argv);

    processConfig();

    std::cout << "initializing SDL\n";
    sdlCheck(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS));
    sdlCheck(TTF_Init());
    constexpr auto imgInitFlags = IMG_INIT_PNG;
    sdlCheck(IMG_Init(imgInitFlags) == imgInitFlags);

    sdlCheck(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096));
    Mix_VolumeMusic(40);

    {
        std::cout << "loading booka story from " << storyFilePath << "\n";
        auto booka = booka::Booka{storyFilePath};

        std::cout << "creating view\n";
        auto view = View{booka};

        std::cout << "starting game\n";
        bool done = false;
        auto frameTimer = tempo::FrameTimer{config().gameFps};
        while (!done) {
            if (!view.processInput()) {
                break;
            }

            if (frameTimer() > 0) {
                view.present();
            }

            frameTimer.relax();
        }
    }

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
} catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
}
