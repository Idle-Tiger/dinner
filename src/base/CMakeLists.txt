configure_file(build-info.hpp.in include/build-info.hpp @ONLY)

add_library(base
    story.cpp
)

target_include_directories(base PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/include
)
