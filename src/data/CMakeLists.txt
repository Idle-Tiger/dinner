add_custom_command(
    COMMENT "generating data_generated.h from data.fbs"
    COMMAND $<TARGET_FILE:flatc>
        --cpp
        --scoped-enums
        --cpp-field-case-style lower
        --cpp-std c++17
        -o "${CMAKE_CURRENT_BINARY_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/data.fbs"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data.fbs"
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/include/data_generated.h"
)

add_library(data
    data.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/include/data_generated.h"
)
target_include_directories(data PUBLIC
    include
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)
target_link_libraries(data PUBLIC flatbuffers)
set_target_properties(data PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
