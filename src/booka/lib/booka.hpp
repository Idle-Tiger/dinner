#pragma once

#include "booka_generated.h"

#include "memory_mapped_file.hpp"

#include <cstddef>
#include <filesystem>
#include <span>
#include <string_view>
#include <utility>

struct Action {
};

class StringsIterator {
public:
    StringsIterator(const fb::Strings* strings, size_t index);

    StringsIterator& operator++();
    StringsIterator operator++(int);

    std::string_view operator*() const;

private:
    const fb::Strings* _strings = nullptr;
    size_t _index = 0;
};

class Strings {
public:
    Strings(const fb::Strings* fbStrings);

    [[nodiscard]] StringsIterator begin() const;
    [[nodiscard]] StringsIterator end() const;

    std::string_view operator[](size_t index) const;

private:
    const fb::Strings* _fbStrings = nullptr;
};

class BinaryDataIterator {
public:
    BinaryDataIterator(const fb::BinaryData* binaryData, size_t index);

    BinaryDataIterator& operator++();
    BinaryDataIterator operator++(int);

    std::span<const std::byte> operator*() const;

private:
    const fb::BinaryData* _binaryData = nullptr;
    size_t _index = 0;
};

class BinaryData {
public:
    BinaryData(const fb::BinaryData* fbBinaryData);

    [[nodiscard]] BinaryDataIterator begin() const;
    [[nodiscard]] BinaryDataIterator end() const;

    std::span<const std::byte> operator[](size_t index) const;

private:
    const fb::BinaryData* _fbBinaryData = nullptr;
};

class Actions {
public:
};

class Booka {
public:
    Booka(const std::filesystem::path& path);

    [[nodiscard]] Strings imageNames() const;
    [[nodiscard]] BinaryData images() const;
    [[nodiscard]] Strings characterNames() const;

private:
    MemoryMappedFile _file;
    const fb::Booka* _booka = nullptr;
};
