#include "config.hpp"
#include "overloaded.hpp"
#include "sdl.hpp"
#include "story.hpp"
#include "view.hpp"

#include <tempo.hpp>

#include <SDL_image.h>
#include <SDL_mixer.h>

#include <cstdlib>
#include <exception>
#include <iostream>

#ifdef __cplusplus
extern "C"
#endif
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) try
{
    sdlCheck(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS));
    sdlCheck(TTF_Init());
    constexpr auto imgInitFlags = IMG_INIT_PNG;
    sdlCheck(IMG_Init(imgInitFlags) == imgInitFlags);

    sdlCheck(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096));
    Mix_VolumeMusic(40);

    loadConfigIfPresent();

    {
        auto story = Story{};
        story.initializeTestStory();

        auto view = View{story};
        view.loadImage(bi::SOURCE_ROOT / "assets/test-level/bg/bird.png");
        view.loadImage(bi::SOURCE_ROOT / "assets/test-level/bg/elephant.png");
        view.loadImage(bi::SOURCE_ROOT / "assets/test-level/bg/mouse.png");
        view.loadImage(bi::SOURCE_ROOT / "assets/test-level/bg/tea-table.png");

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
