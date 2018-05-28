cmake_minimum_required(VERSION 3.5.1)
project(ChessGame)

set(CMAKE_CXX_STANDARD 11)

set(GOMOKU_DIR Gomoku)

# Avoid using UNIX directory shortcuts `..` or `.` in `#include` path.
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(${GOMOKU_DIR})