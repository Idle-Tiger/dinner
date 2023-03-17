add_executable(repa
    main.cpp
)
target_link_libraries(repa PRIVATE arg repa-lib)

add_subdirectory(lib)
