#pragma once

#include "booka.hpp"

#include <filesystem>
#include <vector>

namespace booka {

struct UnpackedShowImageAction {
    uint32_t imageIndex = 0;
};

struct UnpackedShowTextAction {
    std::string character;
    std::string text;
};

struct UnpackedPlayMusicAction {
    uint32_t musicIndex = 0;
};

using UnpackedAction = std::variant<
    UnpackedShowImageAction,
    UnpackedShowTextAction,
    UnpackedPlayMusicAction
>;

struct UnpackedNamedData {
    std::string name;
    std::vector<char> data;
};

struct UnpackedBooka {
    std::vector<std::string> imageNames;
    std::vector<std::vector<char>> imageData;
    std::vector<std::string> musicNames;
    std::vector<std::vector<char>> musicData;
    std::vector<UnpackedAction> actions;

    void pack(const std::filesystem::path& path);
};

} // namespace booka
