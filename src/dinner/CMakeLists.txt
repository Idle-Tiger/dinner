configure_file(build-info.hpp.in include/build-info.hpp @ONLY)

add_executable(dinner
    config.cpp
    main.cpp
    view.cpp
)

target_include_directories(dinner PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include")

target_link_libraries(dinner PRIVATE
    tempo

    SDL2::SDL2
    yaml-cpp::yaml-cpp
)
