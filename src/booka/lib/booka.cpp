#include "booka.hpp"

#include "error.hpp"
#include "logging.hpp"

#include <concepts>

#include <iostream>

namespace booka {

namespace {

template <class FbObject>
requires std::same_as<FbObject, fb::Strings> ||
    std::same_as<FbObject, fb::BinaryData>
std::tuple<uint32_t, uint32_t> calculateRange(const FbObject* fbObject, uint32_t index)
{
    return {
        fbObject->offsets()->Get(index),
        index + 1 < fbObject->offsets()->size() ?
            fbObject->offsets()->Get(index + 1) : fbObject->data()->size()
    };
}

} // namespace

Strings::Strings(const fb::Strings* fbStrings)
    : _fbStrings(fbStrings)
{ }

IndexIterator<Strings> Strings::begin() const
{
    return {*this, 0};
}

IndexIterator<Strings> Strings::end() const
{
    return {*this, _fbStrings->offsets()->size()};
}

std::string_view Strings::operator[](uint32_t index) const
{
    auto [begin, end] = calculateRange(_fbStrings, index);
    return _fbStrings->data()->string_view().substr(begin, end - begin);
}

BinaryData::BinaryData(const fb::BinaryData* fbBinaryData)
    : _fbBinaryData(fbBinaryData)
{ }

[[nodiscard]] IndexIterator<BinaryData> BinaryData::begin() const
{
    return {*this, 0};
}

[[nodiscard]] IndexIterator<BinaryData> BinaryData::end() const
{
    return {*this, _fbBinaryData->offsets()->size()};
}

std::span<const std::byte> BinaryData::operator[](uint32_t index) const
{
    auto [begin, end] = calculateRange(_fbBinaryData, index);
    const auto* ptr =
        reinterpret_cast<const std::byte*>(_fbBinaryData->data()->data() + begin);
    return {ptr, end - begin};
}

NamedDataStorage::NamedDataStorage(const fb::Strings* names, const fb::BinaryData* data)
    : _names(names)
    , _data(data)
{ }

[[nodiscard]] IndexIterator<NamedDataStorage> NamedDataStorage::begin() const
{
    return {*this, 0};
}

[[nodiscard]] IndexIterator<NamedDataStorage> NamedDataStorage::end() const
{
    return {*this, _names->offsets()->size()};
}

NamedData NamedDataStorage::operator[](uint32_t index) const
{
    auto result = NamedData {
        .name = Strings{_names}[index],
        .data = BinaryData{_data}[index],
    };
    return result;
}

Actions::Actions(const fb::Booka* booka)
    : _booka(booka)
{ }

IndexIterator<Actions> Actions::begin() const
{
    return {*this, 0};
}

IndexIterator<Actions> Actions::end() const
{
    return {*this, _booka->story()->size()};
}

Action Actions::operator[](uint32_t index) const
{
    std::cout << "getting action " << index << "\n";
    const auto* fbAction = _booka->story()->Get(index);
    std::cout << "got action pointer\n";
    switch (fbAction->type()) {
        case fb::ActionType::Text:
        {
            const fb::ShowTextAction* fbShowTextAction =
                _booka->showTextActions()->Get(fbAction->index());
            std::cout << "character index: " <<
                fbShowTextAction->characterIndex() << "; phrase index: " <<
                fbShowTextAction->phraseIndex() << "\n";

            auto characterNames = Strings{_booka->characterNames()};
            auto characterName = std::string_view{};
            if (fbShowTextAction->characterIndex() != uint32_t(-1)) {
                characterName =
                    characterNames[fbShowTextAction->characterIndex()];
            }

            auto phrases = Strings{_booka->phrases()};
            const std::string_view phrase =
                phrases[fbShowTextAction->phraseIndex()];
            return ShowTextAction{
                .character = characterName,
                .text = phrase,
            };
        }
        case fb::ActionType::Image:
            return ShowImageAction{.imageIndex = fbAction->index()};
        case fb::ActionType::Music:
            return PlayMusicAction{.musicIndex = fbAction->index()};
    }

    throw Error{} << "unknown fb::Action type";
}

} // namespace booka
