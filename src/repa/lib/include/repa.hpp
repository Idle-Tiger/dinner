#pragma once

#include "repa_generated.h"

#include "data.hpp"
#include "memory_mapped_file.hpp"

#include <cstddef>
#include <filesystem>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace repa {

struct Source {
    std::string name;
    std::filesystem::path path;
};

struct Manifest {
    std::vector<Source> sources;
};

void pack(
    const Manifest& manifest,
    const std::filesystem::path& outputHeaderPath,
    const std::filesystem::path& outputDataFilePath);
void packByYaml(
    const std::filesystem::path& yamlManifestPath,
    const std::filesystem::path& outputHeaderPath,
    const std::filesystem::path& outputDataFilePath);

struct Resource {
    std::string_view name;
    std::span<const std::byte> content;
};

class Repa {
public:
    Repa(const std::filesystem::path& path);

    [[nodiscard]] std::span<const std::byte> operator()(size_t resourceIndex) const;
    [[nodiscard]] std::span<const std::byte> operator()(
        std::string_view resourceName) const;

private:
    MemoryMappedFile _file;
    const fb::Repa* _repa = nullptr;
    data::NamedDataStorage _resources;
    std::map<std::string, size_t> _indexByName;
};

} // namespace repa
