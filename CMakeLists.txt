cmake_minimum_required(VERSION 3.10)
project(CHESS3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR})

add_executable(CHESS3
        main.cpp
        BoardPrinter.cpp
        ChessBoard.cpp
        ConfigReader.cpp
        MoveValidator.cpp
        Piece.cpp
        Portal.cpp
        Archer.cpp
        Portal.h
        BoardPrinter.h
)
configure_file(${CMAKE_SOURCE_DIR}/chess_pieces.json ${CMAKE_BINARY_DIR}/chess_pieces.json COPYONLY)

target_include_directories(CHESS3 PRIVATE include)