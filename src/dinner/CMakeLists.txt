configure_file(build-info.hpp.in include/build-info.hpp @ONLY)

add_executable(dinner
    config.cpp
    main.cpp
    story.cpp
    view.cpp
)

target_include_directories(dinner PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include")

target_link_libraries(dinner PRIVATE
    tempo

    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
    yaml-cpp::yaml-cpp
)

if(WIN32)
    add_custom_command(TARGET dinner POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:dinner> $<TARGET_FILE_DIR:dinner>
        COMMAND_EXPAND_LISTS
    )
endif()
