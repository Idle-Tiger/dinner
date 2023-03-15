#pragma once

#include "booka_generated.h"

#include "memory_mapped_file.hpp"

#include <concepts>
#include <cstddef>
#include <filesystem>
#include <span>
#include <string_view>
#include <utility>
#include <variant>

namespace booka {

template <class Container>
class IndexIterator {
public:
    IndexIterator(const Container& container, uint32_t index)
        : _container(container)
        , _index(index)
    { }

    IndexIterator& operator++()
    {
        ++_index;
        return *this;
    }

    IndexIterator operator++(int)
    {
        auto temp = *this;
        ++*this;
        return temp;
    }

    auto operator*() const
    {
        return _container[_index];
    }

    friend bool operator==(const IndexIterator& lhs, const IndexIterator& rhs)
    {
        return lhs._index == rhs._index;
    }

    friend bool operator!=(const IndexIterator& lhs, const IndexIterator& rhs)
    {
        return !(lhs == rhs);
    }

private:
    const Container& _container;
    uint32_t _index = 0;
};

class Strings {
public:
    Strings(const fb::Strings* fbStrings);

    [[nodiscard]] IndexIterator<Strings> begin() const;
    [[nodiscard]] IndexIterator<Strings> end() const;

    std::string_view operator[](uint32_t index) const;

private:
    const fb::Strings* _fbStrings = nullptr;
};

class BinaryData {
public:
    BinaryData(const fb::BinaryData* fbBinaryData);

    [[nodiscard]] IndexIterator<BinaryData> begin() const;
    [[nodiscard]] IndexIterator<BinaryData> end() const;

    std::span<const std::byte> operator[](uint32_t index) const;

private:
    const fb::BinaryData* _fbBinaryData = nullptr;
};

struct NamedData {
    std::string_view name;
    std::span<const std::byte> data;
};

class NamedDataStorage {
public:
    NamedDataStorage(const fb::Strings* names, const fb::BinaryData* data);

    [[nodiscard]] IndexIterator<NamedDataStorage> begin() const;
    [[nodiscard]] IndexIterator<NamedDataStorage> end() const;

    NamedData operator[](uint32_t index) const;

private:
    const fb::Strings* _names = nullptr;
    const fb::BinaryData* _data = nullptr;
};

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
    using Iterator = IndexIterator<Actions>;

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

    [[nodiscard]] const NamedDataStorage& images() const { return _images; }
    [[nodiscard]] const NamedDataStorage& music() const { return _music; }
    [[nodiscard]] const Strings& characterNames() const { return _characterNames; }
    [[nodiscard]] const Actions& actions() const { return _actions; }

private:
    MemoryMappedFile _file;
    const fb::Booka* _booka = nullptr;

    NamedDataStorage _images;
    NamedDataStorage _music;
    Strings _characterNames;
    Actions _actions;
};

} // namespace booka
