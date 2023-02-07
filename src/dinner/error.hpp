#pragma once

#include "build-info.hpp"

#include <exception>
#include <filesystem>
#include <ostream>
#include <source_location>
#include <sstream>
#include <string>
#include <utility>

template <class T>
concept Streamable = requires(std::ostream output, T value)
{
    {output << value} -> std::same_as<std::ostream&>;
};

class Error : public std::exception {
public:
    explicit Error(
        std::source_location sourceLocation = std::source_location::current())
        : _sourceLocation(sourceLocation)
    { }

    const char* what() const noexcept override
    {
        if (_cache.empty()) {
            auto relativeFilePath = std::filesystem::proximate(
                _sourceLocation.file_name(), bi::SOURCE_ROOT).string();

            auto stream = std::ostringstream{};
            stream <<
                relativeFilePath << ":" <<
                _sourceLocation.line() << ":" <<
                _sourceLocation.column() << " (" <<
                _sourceLocation.function_name() << ") " <<
                _message;
            _cache = stream.str();
        }
        return _cache.c_str();
    }

    template <Streamable T>
    Error& operator<<(T&& value) &
    {
        _cache.clear();
        _message = appendToString(std::move(_message), std::forward<T>(value));
        return *this;
    }

    template <Streamable T>
    Error operator<<(T&& value) &&
    {
        _cache.clear();
        _message = appendToString(std::move(_message), std::forward<T>(value));
        return *this;
    }

private:
    template <Streamable T>
    static std::string appendToString(std::string&& string, T&& value)
    {
        auto stream =
            std::ostringstream{std::move(string), std::ios_base::ate};
        stream << std::forward<T>(value);
        return stream.str();
    }

    std::string _message;
    std::source_location _sourceLocation;
    mutable std::string _cache;
};
