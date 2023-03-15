#include "unpacked_booka.hpp"

#include "overloaded.hpp"

#include <fstream>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include <iostream>

namespace booka {

namespace {

flatbuffers::Offset<fb::Strings> pack(
    flatbuffers::FlatBufferBuilder& builder,
    const std::vector<std::string>& strings)
{
    auto data = std::string{};
    auto offsets = std::vector<uint32_t>{};
    for (const auto& string : strings) {
        offsets.push_back((uint32_t)data.length());
        data += string;
    }

    return fb::CreateStrings(
        builder, builder.CreateString(data), builder.CreateVector(offsets));
}

flatbuffers::Offset<fb::BinaryData> pack(
    flatbuffers::FlatBufferBuilder& builder,
    const std::vector<std::vector<char>>& blobs)
{
    auto data = std::vector<int8_t>{};
    auto offsets = std::vector<uint32_t>{};
    for (const auto& blob : blobs) {
        offsets.push_back((uint32_t)data.size());
        std::copy(blob.begin(), blob.end(), std::back_inserter(data));
    }

    return fb::CreateBinaryData(
        builder, builder.CreateVector(data), builder.CreateVector(offsets));
}

} // namespace

void UnpackedBooka::pack(const std::filesystem::path& path)
{
    std::map<std::string, uint32_t> characters;
    auto phrases = std::vector<std::string>{};
    auto showTextActions = std::vector<fb::ShowTextAction>{};
    auto actions = std::vector<fb::Action>{};

    for (const auto& action : this->actions) {
        std::visit(Overloaded{
            [&](const booka::UnpackedShowTextAction& showTextAction) {
                auto characterIndex = uint32_t(-1);
                if (!showTextAction.character.empty()) {
                    characters.emplace(showTextAction.character, (uint32_t)characters.size());
                    characterIndex = characters.at(showTextAction.character);
                }

                const auto phraseIndex = (uint32_t)phrases.size();
                phrases.push_back(showTextAction.text);
                showTextActions.emplace_back(characterIndex, phraseIndex);
                actions.emplace_back(fb::ActionType::Text, phraseIndex);
            },
            [&](const booka::UnpackedShowImageAction& showImageAction) {
                actions.emplace_back(fb::ActionType::Image, showImageAction.imageIndex);
            },
            [&](const booka::UnpackedPlayMusicAction& playMusicAction) {
                actions.emplace_back(fb::ActionType::Music, playMusicAction.musicIndex);
            },
        }, action);
    }

    auto characterNames = std::vector<std::string>{};
    for (const auto& [characterName, characterIndex] : characters) {
        characterNames.push_back(characterName);
    }

    auto builder = flatbuffers::FlatBufferBuilder{};
    auto booka = fb::CreateBooka(
        builder,
        ::booka::pack(builder, imageNames),
        ::booka::pack(builder, imageData),
        ::booka::pack(builder, musicNames),
        ::booka::pack(builder, musicData),
        ::booka::pack(builder, characterNames),
        ::booka::pack(builder, phrases),
        builder.CreateVectorOfStructs(showTextActions),
        builder.CreateVectorOfStructs(actions));
    builder.Finish(booka);

    auto output = std::ofstream{path, std::ios::binary};
    output.exceptions(std::ios::badbit | std::ios::failbit);
    output.write(
        reinterpret_cast<const char*>(builder.GetBufferPointer()),
        builder.GetSize());
}

} // namespace booka
