#include "repa.hpp"

#include "fs.hpp"

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <regex>

namespace fs = std::filesystem;

namespace repa {

namespace {

Manifest loadManifestFromYaml(const fs::path& yamlManifestPath)
{
    auto manifest = Manifest{};

    auto yaml = YAML::LoadFile(yamlManifestPath);
    for (const auto& yamlResource : yaml["sources"]) {
        manifest.sources.push_back(Source{
            .name = yamlResource["name"].as<std::string>(),
            .path = yamlManifestPath.parent_path() /
                yamlResource["path"].as<std::string>(),
        });
    }

    return manifest;
}

} // namespace

void packByYaml(
    const fs::path& yamlManifestPath,
    const fs::path& outputHeaderPath,
    const fs::path& outputDataFilePath)
{
    pack(
        loadManifestFromYaml(yamlManifestPath),
        outputHeaderPath,
        outputDataFilePath);
}

void pack(
    const Manifest& manifest,
    const fs::path& outputHeaderPath,
    const fs::path& outputDataFilePath)
{
    std::vector<std::string> resourceNames;
    std::vector<std::vector<std::byte>> resourceData;

    auto header = std::ofstream{outputHeaderPath};
    header.exceptions(std::ios::badbit | std::ios::failbit);
    header << R"(#pragma once

enum class R {
)";

    for (const auto& source : manifest.sources) {
        std::string enumName =
            std::regex_replace(source.name, std::regex{"[^a-zA-Z0-9]+"}, "_");
        std::transform(
            enumName.begin(), enumName.end(), enumName.begin(),
            [] (unsigned char c) { return std::toupper(c); });
        header << "    " << enumName << ",\n";

        resourceNames.push_back(source.name);
        resourceData.push_back(file::read(source.path));
    }

    header << "};";

    auto builder = flatbuffers::FlatBufferBuilder{};
    auto repa = fb::CreateRepa(
        builder,
        data::pack(builder, resourceNames),
        data::pack(builder, resourceData));
    builder.Finish(repa);

    file::write(
        outputDataFilePath,
        reinterpret_cast<const std::byte*>(builder.GetBufferPointer()),
        builder.GetSize());
}

Repa::Repa(const std::filesystem::path& path)
    : _file(path)
    , _repa(fb::GetRepa(_file.span().data()))
    , _resources(_repa->resourceNames(), _repa->resourceData())
{
    for (size_t i = 0; i < _resources.size(); i++) {
        _indexByName[std::string{_resources[i].name}] = i;
    }
}

std::span<const std::byte> Repa::operator()(size_t resourceIndex) const
{
    return _resources[resourceIndex].data;
}

std::span<const std::byte> Repa::operator()(
    std::string_view resourceName) const
{
    return (*this)(_indexByName.at(std::string{resourceName}));
}

} // namespace repa
