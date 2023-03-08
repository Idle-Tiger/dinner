#include "arg.hpp"
#include "error.hpp"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <map>
#include <string>

namespace fs = std::filesystem;

enum class Action {
    Encode,
    Decode,
};

std::istream& operator>>(std::istream& input, Action& action)
{
    static const std::map<std::string, Action> mapping {
        {"decode", Action::Decode},
        {"encode", Action::Encode},
    };

    std::string s;
    input >> s;
    if (auto i = mapping.find(s); i != mapping.end()) {
        action = i->second;
    } else {
        auto error = Error{};
        error << "unknown action; valid values:";
        if (auto j = mapping.begin(); j != mapping.end()) {
            error << " " << j->first;
            ++j;
            for (; j != mapping.end(); ++j) {
                error << ", " << j->first;
            }
        }
        throw std::move(error);
    }
    return input;
}

void encode(const fs::path& inputFilePath, [[maybe_unused]] const fs::path& outputFilePath)
{
    auto input = std::ifstream{inputFilePath};
    input.exceptions(std::ios::badbit);

    for (std::string line; std::getline(input, line); ) {
        std::cout << "line: " << line << "\n";
    }
}

int main(int argc, char* argv[]) try
{
    auto parser = arg::Parser{};
    auto action = parser.argument<Action>()
        .metavar("ACTION")
        .markRequired()
        .help("action to perform");
    auto input = parser.option<fs::path>()
        .keys("--input")
        .markRequired()
        .help("path to input file");
    auto output = parser.option<fs::path>()
        .keys("--output")
        .markRequired()
        .help("path to output file");
    parser.helpKeys("-h", "--help");
    parser.parse(argc, argv);

    switch (action) {
        case Action::Decode:
            throw Error{} << "decoding is not implemented";
            break;
        case Action::Encode:
            encode(input, output);
            break;
    }

} catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
}
