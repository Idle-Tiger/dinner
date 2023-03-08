add_executable(booka
    main.cpp
)
target_link_libraries(booka PRIVATE
    arg
    base

    booka-lib
)

add_subdirectory(lib)
