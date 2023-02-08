#include "config.hpp"

#include <yaml-cpp/yaml.h>

#include <cstdlib>
#include <fstream>
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
}

fs::path standardConfigPath()
{
    // TODO: write proper path deduction

    constexpr auto fileName = "dinner.yaml";

    if (const char* xdgConfigHome = std::getenv("XDG_CONFIG_HOME")) {
        return fs::path{xdgConfigHome} / fileName;
    }

    if (const char* home = std::getenv("HOME")) {
        return fs::path{home} / ".config" / fileName;
    }
    throw Error{} <<
        "cannot deduce config path: HOME environment variable is not set";
}

} // namespace

const Config& config()
{
    return globalConfig;
}

void loadConfigFromFile(const fs::path& path)
{
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

    auto output = std::ofstream{path};
    output.exceptions(std::ios::badbit | std::ios::failbit);
    output << yaml;
}

void loadConfigIfPresent()
{
    auto path = standardConfigPath();
    if (fs::exists(path)) {
        loadConfigFromFile(path);
    }
}

void saveConfig()
{
    saveConfigToFile(standardConfigPath());
}
