add_executable(repa
    main.cpp
)
target_link_libraries(repa PRIVATE arg repa-lib)

if(WIN32)
    add_custom_command(TARGET repa POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_RUNTIME_DLLS:repa> $<TARGET_FILE_DIR:repa>
        COMMAND_EXPAND_LISTS
    )
endif()

add_subdirectory(lib)
