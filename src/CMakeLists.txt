if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX)
    add_compile_options(/W3 /WX /utf-8)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
    #set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

add_subdirectory(arg)
add_subdirectory(tempo)

add_subdirectory(booka)

add_subdirectory(base)

add_subdirectory(dinner)
add_subdirectory(storyteller)
