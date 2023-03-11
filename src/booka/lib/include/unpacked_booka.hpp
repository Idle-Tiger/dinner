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

using UnpackedAction = std::variant<
    UnpackedShowImageAction,
    UnpackedShowTextAction
>;

struct UnpackedBooka {
    std::vector<std::vector<uint8_t>> images;
    std::vector<UnpackedAction> actions;

    void pack(const std::filesystem::path& path);
};

} // namespace booka
