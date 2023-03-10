#include "memory_mapped_file.hpp"

#include "error.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

MemoryMappedFile::MemoryMappedFile(const std::filesystem::path& path)
{
    _fd = open(path.string().c_str(), O_RDONLY); // NOLINT
    check(_fd != -1);

    {
        struct stat sb{};
        check(fstat(_fd, &sb) != -1);
        _fileSize = sb.st_size;
    }

    _addr = mmap(nullptr, _fileSize, PROT_READ, MAP_PRIVATE, _fd, 0);
    check(_addr != MAP_FAILED); // NOLINT
}

MemoryMappedFile::MemoryMappedFile(MemoryMappedFile&& other) noexcept
    : _fd(other._fd)
    , _fileSize(other._fileSize)
    , _addr(other._addr)
{
    other._addr = nullptr;
}

MemoryMappedFile::~MemoryMappedFile()
{
    if (_addr) {
        munmap(_addr, _fileSize);
        close(_fd);
    }
}

std::span<const std::byte> MemoryMappedFile::span() const
{
    return {static_cast<std::byte*>(_addr), _fileSize};
}
