#include "data.hpp"

namespace data {

namespace {

template <class FbObject>
requires
    std::same_as<FbObject, fb::Strings> ||
    std::same_as<FbObject, fb::BinaryData>
std::tuple<uint32_t, uint32_t> calculateRange(
    const FbObject* fbObject, uint32_t index)
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

size_t Strings::size() const
{
    return _fbStrings->offsets()->size();
}

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

size_t BinaryData::size() const
{
    return _fbBinaryData->offsets()->size();
}

flatbuffers::Offset<fb::BinaryData> pack(
    flatbuffers::FlatBufferBuilder& builder,
    const std::vector<std::vector<std::byte>>& blobs)
{
    auto data = std::vector<uint8_t>{};
    auto offsets = std::vector<uint32_t>{};
    for (const auto& blob : blobs) {
        offsets.push_back((uint32_t)data.size());
        for (std::byte b : blob) {
            data.push_back(static_cast<uint8_t>(b));
        }
    }

    return fb::CreateBinaryData(
        builder, builder.CreateVector(data), builder.CreateVector(offsets));
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

size_t NamedDataStorage::size() const
{
    return _names->offsets()->size();
}

} // namespace data
