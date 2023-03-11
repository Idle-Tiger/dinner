#include "memory_mapped_file.hpp"

#include "error.hpp"

#ifdef linux
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

MemoryMappedFile::MemoryMappedFile(const std::filesystem::path& path)
{
#ifdef linux
    _fd = open(path.string().c_str(), O_RDONLY); // NOLINT
    check(_fd != -1);

    size_t fileSize = 0;
    {
        struct stat sb{};
        check(fstat(_fd, &sb) != -1);
        fileSize = sb.st_size;
    }

    void* address = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, _fd, 0);
    check(address != MAP_FAILED); // NOLINT

    _span = {static_cast<std::byte*>(address), static_cast<size_t>(fileSize)};
#elif defined(_WIN32)
    _fileHandle = CreateFile(
        path.string().c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (_fileHandle == INVALID_HANDLE_VALUE) {
        throw Error{} << "CreateFile failed: " << GetLastError();
    }

    LARGE_INTEGER fileSize{};
    if (!GetFileSizeEx(_fileHandle, &fileSize)) {
        throw Error{} << "GetFileSizeEx failed: " << GetLastError();
    }

    _fileMappingHandle = CreateFileMapping(_fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
    if (_fileMappingHandle == NULL) {
        throw Error{} << "CreateFileMapping failed: " << GetLastError();
    }

    LPVOID address = MapViewOfFile(_fileMappingHandle, FILE_MAP_READ, 0, 0, 0);
    if (!address) {
        throw Error{} << "MapViewOfFile failed: " << GetLastError();
    }

    _span = {static_cast<std::byte*>(address), static_cast<size_t>(fileSize.QuadPart)};
#endif
}

MemoryMappedFile::MemoryMappedFile(MemoryMappedFile&& other) noexcept
#ifdef linux
    : _fd(other._fd)
#elif defined(_WIN32)
    : _fileHandle(other._fileHandle)
    , _fileMappingHandle(other._fileMappingHandle)
#endif
{
    std::swap(_span, other._span);
}

MemoryMappedFile::~MemoryMappedFile()
{
    if (!_span.empty()) {
#ifdef linux
        munmap(_span.data(), _span.size());
        close(_fd);
#elif defined(_WIN32)
        UnmapViewOfFile(_span.data());
        CloseHandle(_fileHandle);
#endif
    }
}

std::span<const std::byte> MemoryMappedFile::span() const
{
    return _span;
}
