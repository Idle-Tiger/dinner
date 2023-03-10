#include "booka.hpp"

#include <concepts>

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

StringsIterator::StringsIterator(const fb::Strings* strings, uint32_t index)
    : _strings(strings)
    , _index(index)
{ }

StringsIterator& StringsIterator::operator++()
{
    ++_index;
    return *this;
}

StringsIterator StringsIterator::operator++(int)
{
    auto tmp = *this;
    ++*this;
    return tmp;
}

std::string_view StringsIterator::operator*() const
{
    auto [begin, end] = calculateRange(_strings, _index);
    return _strings->data()->string_view().substr(begin, end - begin);
}

Strings::Strings(const fb::Strings* fbStrings)
    : _fbStrings(fbStrings)
{ }

StringsIterator Strings::begin() const
{
    return {_fbStrings, 0};
}

StringsIterator Strings::end() const
{
    return {_fbStrings, _fbStrings->offsets()->size()};
}

std::string_view Strings::operator[](uint32_t index) const
{
    auto [begin, end] = calculateRange(_fbStrings, index);
    return _fbStrings->data()->string_view().substr(begin, end - begin);
}

BinaryDataIterator::BinaryDataIterator(
    const fb::BinaryData* binaryData, uint32_t index)
    : _binaryData(binaryData)
    , _index(index)
{ }

BinaryDataIterator& BinaryDataIterator::operator++()
{
    ++_index;
    return *this;
}

BinaryDataIterator BinaryDataIterator::operator++(int)
{
    auto tmp = *this;
    ++*this;
    return tmp;
}

std::span<const std::byte> BinaryDataIterator::operator*() const
{
    auto [begin, end] = calculateRange(_binaryData, _index);
    const auto* ptr =
        reinterpret_cast<const std::byte*>(_binaryData->data()->data() + begin);
    return {ptr, begin - end};
}

BinaryData::BinaryData(const fb::BinaryData* fbBinaryData)
    : _fbBinaryData(fbBinaryData)
{ }

[[nodiscard]] BinaryDataIterator BinaryData::begin() const
{
    return BinaryDataIterator{_fbBinaryData, 0};
}

[[nodiscard]] BinaryDataIterator BinaryData::end() const
{
    return BinaryDataIterator{_fbBinaryData, _fbBinaryData->offsets()->size()};
}

std::span<const std::byte> BinaryData::operator[](uint32_t index) const
{
    auto [begin, end] = calculateRange(_fbBinaryData, index);
    const auto* ptr =
        reinterpret_cast<const std::byte*>(_fbBinaryData->data()->data() + begin);
    return {ptr, begin - end};
}

Booka::Booka(const std::filesystem::path& path)
    : _file(path)
    , _booka(fb::GetBooka(_file.span().data()))
{
}

Strings Booka::imageNames() const
{
    return {_booka->imageNames()};
}

BinaryData Booka::images() const
{
    return {_booka->imageData()};
}

Strings Booka::characterNames() const
{
    return {_booka->characterNames()};
}
