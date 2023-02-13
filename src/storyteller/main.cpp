#include "overloaded.hpp"
#include "story.hpp"

#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <variant>

int main() try
{
    auto story = Story{};
    story.initializeTestStory();

    bool done = false;
    while (!done) {
        std::visit(Overloaded{
            [&story](const ShowBackground& bg) {
                std::cout << "change background: " << bg.id << "\n";
                story.next();
            },
            [&](const Text& text) {
                std::cout << text.character << ": " << text.text;
                std::cin.get();
                story.next();
            },
            [&story](const Choice& choice) {
                for (size_t i = 0; i < choice.selections.size(); i++) {
                    std::cout << std::setw(2) << i << ": " <<
                        choice.selections.at(i).text << "\n";
                }

                int selection = 0;
                std::cin >> selection;
                std::string s;
                std::getline(std::cin, s);
                story.select(selection);
            },
            [&done](const Finish&) {
                done = true;
                return;
            },
        }, story.action());
    }
} catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
}
