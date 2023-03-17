add_custom_command(
    COMMENT "generating repa_generated.h from repa.fbs"
    COMMAND $<TARGET_FILE:flatc>
        --cpp
        --scoped-enums
        --cpp-field-case-style lower
        --cpp-std c++17
        -I "${PROJECT_SOURCE_DIR}/src/data"
        -o "${CMAKE_CURRENT_BINARY_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/repa.fbs"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/repa.fbs"
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/include/repa_generated.h"
)

add_library(repa-lib
    repa.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/include/repa_generated.h"
)
target_include_directories(repa-lib PUBLIC
    include
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)
target_link_libraries(repa-lib PUBLIC
    base
    data
    yaml-cpp::yaml-cpp
)
set_target_properties(repa-lib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
