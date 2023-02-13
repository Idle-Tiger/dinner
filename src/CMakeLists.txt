if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/W3 /WX /utf-8)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(tempo)

add_subdirectory(base)

add_subdirectory(dinner)
add_subdirectory(storyteller)
