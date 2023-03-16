#pragma once

#include <filesystem>

namespace paths {

std::filesystem::path userConfigPath();
std::filesystem::path globalConfigPath();

} // namespace paths
