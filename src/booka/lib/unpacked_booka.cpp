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
        offsets.push_back(data.length());
        data += string;
    }

    return fb::CreateStrings(
        builder, builder.CreateString(data), builder.CreateVector(offsets));
}

flatbuffers::Offset<fb::BinaryData> pack(
    flatbuffers::FlatBufferBuilder& builder,
    const std::vector<std::vector<uint8_t>>& blobs)
{
    auto data = std::vector<uint8_t>{};
    auto offsets = std::vector<uint32_t>{};
    for (const auto& blob : blobs) {
        offsets.push_back(data.size());
        std::copy(blob.begin(), blob.end(), std::back_inserter(data));
    }

    return fb::CreateBinaryData(
        builder, builder.CreateVector(data), builder.CreateVector(offsets));
}

} // namespace

void UnpackedBooka::pack(const std::filesystem::path& path)
{
    std::map<std::string, uint32_t> characters;
    auto imageNames = std::vector<std::string>{};
    auto phrases = std::vector<std::string>{};
    auto showTextActions = std::vector<fb::ShowTextAction>{};
    auto showImageActions = std::vector<fb::ShowImageAction>{};
    auto actions = std::vector<fb::Action>{};

    for (const auto& action : this->actions) {
        std::visit(Overloaded{
            [&](const booka::UnpackedShowTextAction& showTextAction) {
                auto characterIndex = uint32_t(-1);
                if (!showTextAction.character.empty()) {
                    characters.emplace(showTextAction.character, characters.size());
                    characterIndex = characters.at(showTextAction.character);
                }

                const uint32_t phraseIndex = phrases.size();
                phrases.push_back(showTextAction.text);
                showTextActions.emplace_back(characterIndex, phraseIndex);
                actions.emplace_back(fb::ActionType::Text, phraseIndex);
            },
            [&](const booka::UnpackedShowImageAction& showImageAction) {
                showImageActions.emplace_back(showImageAction.imageIndex);
            },
        }, action);
    }

    auto characterNames = std::vector<std::string>{};
    for (const auto& [characterName, characterIndex] : characters) {
        characterNames.push_back(characterName);
    }

    for (const auto& characterName : characterNames) {
        std::cout << "characterName: " << characterName << "\n";
    }
    for (const auto& phrase : phrases) {
        std::cout << "phrase: " << phrase << "\n";
    }

    auto builder = flatbuffers::FlatBufferBuilder{};
    auto booka = fb::CreateBooka(
        builder,
        ::booka::pack(builder, images),
        ::booka::pack(builder, imageNames),
        ::booka::pack(builder, characterNames),
        ::booka::pack(builder, phrases),
        builder.CreateVectorOfStructs(showTextActions),
        builder.CreateVectorOfStructs(showImageActions),
        builder.CreateVectorOfStructs(actions));
    builder.Finish(booka);

    auto output = std::ofstream{path, std::ios::binary};
    output.exceptions(std::ios::badbit | std::ios::failbit);
    output.write(
        reinterpret_cast<const char*>(builder.GetBufferPointer()),
        builder.GetSize());
}

} // namespace booka
