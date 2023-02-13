add_executable(storyteller
    main.cpp
)
target_link_libraries(storyteller PRIVATE base)

if(WIN32)
    add_custom_command(TARGET storyteller POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_RUNTIME_DLLS:storyteller> $<TARGET_FILE_DIR:storyteller>
        COMMAND_EXPAND_LISTS
    )
endif()
