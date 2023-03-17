#pragma once

#include "data_generated.h"

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace data {

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
    [[nodiscard]] size_t size() const;

private:
    const fb::Strings* _fbStrings = nullptr;
};

flatbuffers::Offset<fb::Strings> pack(
    flatbuffers::FlatBufferBuilder& builder,
    const std::vector<std::string>& strings);

class BinaryData {
public:
    BinaryData(const fb::BinaryData* fbBinaryData);

    [[nodiscard]] IndexIterator<BinaryData> begin() const;
    [[nodiscard]] IndexIterator<BinaryData> end() const;

    std::span<const std::byte> operator[](uint32_t index) const;
    [[nodiscard]] size_t size() const;

private:
    const fb::BinaryData* _fbBinaryData = nullptr;
};

flatbuffers::Offset<data::fb::BinaryData> pack(
    flatbuffers::FlatBufferBuilder& builder,
    const std::vector<std::vector<std::byte>>& blobs);

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
    [[nodiscard]] size_t size() const;

private:
    const fb::Strings* _names = nullptr;
    const fb::BinaryData* _data = nullptr;
};

} // namespace data
