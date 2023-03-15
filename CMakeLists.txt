cmake_minimum_required(VERSION 3.24)

project(Dinner)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

add_subdirectory(deps)
add_subdirectory(src)

add_subdirectory(assets)
