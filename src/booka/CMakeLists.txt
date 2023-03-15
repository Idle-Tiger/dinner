add_executable(booka
    main.cpp
)
target_link_libraries(booka PRIVATE
    arg
    base

    booka-lib
)

add_subdirectory(lib)

if(WIN32)
    add_custom_command(TARGET booka POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_RUNTIME_DLLS:booka> $<TARGET_FILE_DIR:booka>
        COMMAND_EXPAND_LISTS
    )
endif()
