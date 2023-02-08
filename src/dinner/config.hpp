#pragma once

#include <filesystem>
#include <string>

struct Config {
    std::string windowTitle = "Dinner";
    int windowWidth = 1024;
    int windowHeight = 768;
    int gameFps = 60;
    bool fullscreen = true;
};

const Config& config();

void loadConfigFromFile(const std::filesystem::path& path);
void saveConfigToFile(const std::filesystem::path& path);

void loadConfigIfPresent();
void saveConfig();
