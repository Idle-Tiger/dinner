#include "fs.hpp"

#include "error.hpp"

#include <concepts>
#include <fstream>
#include <utility>

#ifdef __linux__
#elif defined(_WIN32)
#include <ShlObj.h>
#endif

namespace fs = std::filesystem;

namespace paths {

namespace {

template <std::invocable F>
class Defer {
public:
    Defer(F&& action) : _action(std::forward<F>(action)) {}
    Defer(const Defer&) = delete;
    Defer(Defer&&) = delete;
    ~Defer() { _action(); }

private:
    F _action;
};

} // namespace

fs::path userConfigPath()
{
#ifdef __linux__
    auto configDirectory = fs::path{};
    if (const char* xdgConfigHome = std::getenv("XDG_CONFIG_HOME")) {
        configDirectory = fs::path{xdgConfigHome};
    } else if (const char* home = std::getenv("HOME")) {
        configDirectory = fs::path{home} / ".config";
    } else {
        throw Error{} <<
            "cannot deduce config path: XDG_CONFIG_HOME and HOME environment "
            "variables are not set";
    }

    return configDirectory / "dinner.yaml";
#elif defined(_WIN32)
    PWSTR localAppData;
    HRESULT result = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppData);
    const auto freePathString = Defer{[&localAppData] {
        CoTaskMemFree(localAppData);
    }};
    if (result != S_OK) {
        throw Error{} << "failed to get path to local AppData";
    }

    return fs::path{localAppData} / "Dinner" / "config.yaml";
#endif
}

fs::path globalConfigPath()
{
#ifdef __linux__
    return fs::path{"/etc/dinner.yaml"};
#elif defined(_WIN32)
    PWSTR programData;
    HRESULT result = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &programData);
    const auto freePathString = Defer{ [&programData] {
        CoTaskMemFree(programData);
    }};
    if (result != S_OK) {
        throw Error{} << "failed to get path to ProgramData";
    }

    return fs::path{programData} / "Dinner" / "config.yaml";
#endif
}

} // namespace paths

namespace file {

std::vector<std::byte> read(const fs::path& path)
{
    if (!fs::exists(path)) {
        throw Error{} << "file does not exist: " << path;
    }
    auto input = std::ifstream{};
    input.exceptions(std::ios::badbit | std::ios::failbit);
    input.open(path, std::ios::binary | std::ios::ate);
    const auto fileSize = input.tellg();
    input.seekg(0, std::ios::beg);
    auto data = std::vector<std::byte>(fileSize);
    input.read(reinterpret_cast<char*>(data.data()), fileSize);
    return data;
}

void write(const fs::path& path, const std::byte* data, size_t size)
{
    auto output = std::ofstream{path, std::ios::binary};
    output.exceptions(std::ios::badbit | std::ios::failbit);
    output.write(
        reinterpret_cast<const char*>(data),
        static_cast<std::streamsize>(size));
}

void write(const fs::path& path, const std::span<const std::byte>& data)
{
    write(path, data.data(), data.size());
}

} // namespace file
