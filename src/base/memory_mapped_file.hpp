#pragma once

#include <cstddef>
#include <filesystem>
#include <span>

class MemoryMappedFile {
public:
    MemoryMappedFile(const std::filesystem::path& path);
    MemoryMappedFile(const MemoryMappedFile& other) = delete;
    MemoryMappedFile(MemoryMappedFile&& other) noexcept;
    ~MemoryMappedFile();

    [[nodiscard]] std::span<const std::byte> span() const;

private:
    int _fd = -1;
    size_t _fileSize = 0;
    void* _addr = nullptr;
};
