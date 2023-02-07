#include "config.hpp"
#include "view.hpp"

#include <tempo.hpp>

#include <SDL.h>

int main()
{
    loadConfigIfPresent();

    auto view = View{};

    auto frameTimer = tempo::FrameTimer{config().gameFps};
    for (;;) {
        if (!view.processInput()) {
            break;
        }

        if (auto framesPassed = frameTimer(); framesPassed > 0) {
            // TODO: update

            view.present();
        }
    }
}
