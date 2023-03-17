#include "unpacked_booka.hpp"

#include "fs.hpp"
#include "overloaded.hpp"

#include <fstream>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include <iostream>

namespace booka {

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
        data::pack(builder, imageNames),
        data::pack(builder, imageData),
        data::pack(builder, musicNames),
        data::pack(builder, musicData),
        data::pack(builder, characterNames),
        data::pack(builder, phrases),
        builder.CreateVectorOfStructs(showTextActions),
        builder.CreateVectorOfStructs(actions));
    builder.Finish(booka);

    file::write(
        path,
        reinterpret_cast<const std::byte*>(builder.GetBufferPointer()),
        builder.GetSize());
}

} // namespace booka
