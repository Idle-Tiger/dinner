#include "config.hpp"

#include "fs.hpp"

#include <yaml-cpp/yaml.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

#include "error.hpp"

namespace fs = std::filesystem;

namespace {

Config globalConfig;
std::mutex globalConfigMutex;

template <class S>
void serialize(Config& config, const S& s)
{
    s(config.windowTitle, "window title");
    s(config.windowWidth, "window width");
    s(config.windowHeight, "window height");
    s(config.gameFps, "game fps");
    s(config.fullscreen, "fullscreen");
    s(config.mute, "mute");
}

} // namespace

Config& config()
{
    return globalConfig;
}

void loadConfigFromFile(const fs::path& path)
{
    std::cout << "loading config from " << path << "\n";
    auto yaml = YAML::LoadFile(path.string());

    auto lock = std::lock_guard{globalConfigMutex};
    serialize(
        globalConfig,
        [&yaml] <class T> (T& field, const std::string& name) {
            auto node = yaml[name];
            if (node.IsDefined()) {
                field = node.as<T>();
            }
        });
}

void saveConfigToFile(const fs::path& path)
{
    auto yaml = YAML::Node{};
    {
        auto lock = std::lock_guard{globalConfigMutex};
        serialize(
            globalConfig,
            [&yaml] (const auto& field, const std::string& name) {
                yaml[name] = field;
            });
    }

    fs::create_directories(path.parent_path());
    auto output = std::ofstream{path};
    output.exceptions(std::ios::badbit | std::ios::failbit);
    output << yaml;
}

void processConfig()
{
    auto path = paths::userConfigPath();
    if (fs::exists(path)) {
        loadConfigFromFile(path);
    }
    saveConfigToFile(path);
}
