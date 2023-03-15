#pragma once

#include <cstddef>
#include <ostream>
#include <utility>
#include <vector>

class Size {
public:
    Size(size_t value) : _value(value) {}

    friend std::ostream& operator<<(std::ostream& output, const Size& size)
    {
        static const std::vector<std::pair<std::string, unsigned long long>> names {
            {"GB", 1ull * 1024 * 1024 * 1024},
            {"MB", 1ull * 1024 * 1024},
            {"KB", 1ull * 1024},
        };

        const auto x = size._value;
        for (const auto& [name, size] : names) {
            if (x >= size) {
                return output << (x / size) << "." << 10 * (x % size) / size << " " << name;
            }
        }
        return output << x << " B";
    }

private:
    size_t _value = 0;
};
