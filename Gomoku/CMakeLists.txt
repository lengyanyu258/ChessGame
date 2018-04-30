cmake_minimum_required(VERSION 3.9)
project(Gomoku_Terminator)

set(CMAKE_CXX_STANDARD 11)

add_executable(Gomoku_Terminator main.cpp Gomoku.cpp Gomoku.h Chessboard.cpp Chessboard.h)