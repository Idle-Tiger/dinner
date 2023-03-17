#pragma once

#include "booka_generated.h"

#include "data.hpp"
#include "memory_mapped_file.hpp"

#include <concepts>
#include <cstddef>
#include <filesystem>
#include <span>
#include <string_view>
#include <utility>
#include <variant>

namespace booka {

struct ShowImageAction {
    uint32_t imageIndex = 0;
};

struct PlayMusicAction {
    uint32_t musicIndex = 0;
};

struct ShowTextAction {
    std::string_view character;
    std::string_view text;
};

using Action = std::variant<
    PlayMusicAction,
    ShowImageAction,
    ShowTextAction>;

class Actions {
public:
    using Iterator = data::IndexIterator<Actions>;

    Actions(const fb::Booka* booka);

    [[nodiscard]] Iterator begin() const;
    [[nodiscard]] Iterator end() const;

    Action operator[](uint32_t index) const;

private:
    const fb::Booka* _booka;
};

class Booka {
public:
    Booka(const std::filesystem::path& path)
        : _file(path)
        , _booka(fb::GetBooka(_file.span().data()))
        , _images(_booka->imageNames(), _booka->imageData())
        , _music(_booka->musicNames(), _booka->musicData())
        , _characterNames(_booka->characterNames())
        , _actions(_booka)
    { }

    [[nodiscard]] const data::NamedDataStorage& images() const { return _images; }
    [[nodiscard]] const data::NamedDataStorage& music() const { return _music; }
    [[nodiscard]] const data::Strings& characterNames() const { return _characterNames; }
    [[nodiscard]] const Actions& actions() const { return _actions; }

private:
    MemoryMappedFile _file;
    const fb::Booka* _booka = nullptr;

    data::NamedDataStorage _images;
    data::NamedDataStorage _music;
    data::Strings _characterNames;
    Actions _actions;
};

} // namespace booka
