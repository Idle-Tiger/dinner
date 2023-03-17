#include "booka.hpp"

#include "error.hpp"
#include "logging.hpp"

#include <concepts>

#include <iostream>

namespace booka {

Actions::Actions(const fb::Booka* booka)
    : _booka(booka)
{ }

data::IndexIterator<Actions> Actions::begin() const
{
    return {*this, 0};
}

data::IndexIterator<Actions> Actions::end() const
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

            auto characterNames = data::Strings{_booka->characterNames()};
            auto characterName = std::string_view{};
            if (fbShowTextAction->characterIndex() != uint32_t(-1)) {
                characterName =
                    characterNames[fbShowTextAction->characterIndex()];
            }

            auto phrases = data::Strings{_booka->phrases()};
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
