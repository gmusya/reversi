cmake_minimum_required(VERSION 3.13)
project(reversi)

set(CMAKE_CXX_STANDARD 20)

add_executable(reversi
        main.cpp
        ArgumentParser.cpp
        Board.cpp
        Cell.cpp
        Reader.cpp
        Writer.cpp
        )