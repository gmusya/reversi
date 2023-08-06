cmake_minimum_required(VERSION 3.13)
project(reversi)

set(CMAKE_CXX_STANDARD 20)

set(ASAN OFF)
set(UBSAN OFF)

if (ASAN)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif ()

if (UBSAN)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif ()

add_executable(reversi
        source/main.cpp
        source/board.cpp
        source/engine.cpp
        )