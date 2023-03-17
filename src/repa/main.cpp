#include "repa.hpp"

#include <arg.hpp>

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    auto manifestPath = arg::option<fs::path>()
        .keys("--manifest")
        .markRequired()
        .help("path to manifest file, describing input resource files");
    auto outputHeaderPath = arg::option<fs::path>()
        .keys("--output-header")
        .markRequired()
        .help("path to output header file");
    auto outputDataFilePath = arg::option<fs::path>()
        .keys("--output-data-file")
        .markRequired()
        .help("path to output data file");
    arg::helpKeys("-h", "--help");
    arg::parse(argc, argv);

    repa::packByYaml(manifestPath, outputHeaderPath, outputDataFilePath);
}
