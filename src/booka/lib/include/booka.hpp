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

    IndexIterator operator++(int) const
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

struct Image {
    std::string_view name;
    std::span<const std::byte> data;
};

class Images {
public:
    Images(const fb::Booka* booka);

    [[nodiscard]] IndexIterator<Images> begin() const;
    [[nodiscard]] IndexIterator<Images> end() const;

    Image operator[](uint32_t index) const;

private:
    const fb::Booka* _booka = nullptr;
};

struct ShowImageAction {
    uint32_t imageIndex = 0;
};

struct ShowTextAction {
    std::string_view character;
    std::string_view text;
};

using Action = std::variant<
    ShowImageAction,
    ShowTextAction
>;

class Actions {
public:
    Actions(const fb::Booka* booka);

    [[nodiscard]] IndexIterator<Actions> begin() const;
    [[nodiscard]] IndexIterator<Actions> end() const;

    Action operator[](uint32_t index) const;

private:
    const fb::Booka* _booka;
};

class Booka {
public:
    Booka(const std::filesystem::path& path);

    [[nodiscard]] Images images() const;
    [[nodiscard]] Strings characterNames() const;
    [[nodiscard]] Actions actions() const;

private:
    MemoryMappedFile _file;
    const fb::Booka* _booka = nullptr;
};

} // namespace booka
