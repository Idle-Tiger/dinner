configure_file(build-info.hpp.in include/build-info.hpp @ONLY)

add_library(base
    fs.cpp
    memory_mapped_file.cpp
    story.cpp
)

target_include_directories(base PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

set_target_properties (base PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
