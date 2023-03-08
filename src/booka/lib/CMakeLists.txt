add_custom_command(
    COMMENT "Generating booka_generated.h from booka.fbs"
    COMMAND $<TARGET_FILE:flatc>
        --cpp
        --scoped-enums
        --cpp-field-case-style lower
        --cpp-std c++17
        -o "${CMAKE_CURRENT_BINARY_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/booka.fbs"
    DEPENDS booka.fbs
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/booka_generated.h
)

add_library(booka-lib
    booka.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/include/booka_generated.h
)
target_include_directories(booka-lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
