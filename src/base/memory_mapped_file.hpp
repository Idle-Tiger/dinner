#pragma once

#include <cstddef>
#include <filesystem>
#include <span>

#ifdef _WIN32
#include <windows.h>
#endif

class MemoryMappedFile {
public:
    MemoryMappedFile(const std::filesystem::path& path);
    MemoryMappedFile(const MemoryMappedFile& other) = delete;
    MemoryMappedFile(MemoryMappedFile&& other) noexcept;
    ~MemoryMappedFile();

    [[nodiscard]] std::span<const std::byte> span() const;

private:
#ifdef __linux__
    int _fd = -1;
#elif defined(_WIN32)
    HANDLE _fileHandle = INVALID_HANDLE_VALUE;
    HANDLE _fileMappingHandle = INVALID_HANDLE_VALUE;
#endif
    std::span<std::byte> _span;
};
