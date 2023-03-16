set(PACKED_STORY_FILES "")

macro(pack_story)
    set(options "")
    set(oneValueArgs NAME SCRIPT)
    set(multiValueArgs FILES)
    cmake_parse_arguments(
        PACK_STORY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    set(output_file "${CMAKE_CURRENT_BINARY_DIR}/stories/${PACK_STORY_NAME}.booka")
    list(APPEND PACKED_STORY_FILES "${output_file}")

    add_custom_command(
        COMMENT "packing story: ${PACK_STORY_NAME} (${output_file})"
        COMMAND
            $<TARGET_FILE:booka>
            encode
            --input "${CMAKE_CURRENT_SOURCE_DIR}/${PACK_STORY_SCRIPT}"
            --output "${output_file}"
        DEPENDS
            ${PACK_STORY_SCRIPT}
            ${PACK_STORY_FILES}
        OUTPUT "${output_file}"
    )
endmacro()

pack_story(
    NAME test-1
    SCRIPT test-1/script.txt
    FILES
        test-1/music/feast.wav
)

add_custom_target(pack-stories ALL
    DEPENDS ${PACKED_STORY_FILES}
)
