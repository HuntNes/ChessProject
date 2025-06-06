cmake_minimum_required(VERSION 3.31)
project(CHESS3)

set(CMAKE_CXX_STANDARD 20)

# Include directories
include_directories(include)
include_directories(${CMAKE_SOURCE_DIR})

add_executable(CHESS3
        main.cpp
        ChessBoard.cpp
        MoveValidator.cpp
        Piece.cpp
        ConfigReader.cpp
        Portal.cpp
        BoardPrinter.cpp   # <-- Bunu ekle!
        Portal.h
        BoardPrinter.h     # <-- Bunu eklemene gerek yok, ama eklesen de sorun olmaz
)

# Copy JSON file to build directory
configure_file(${CMAKE_SOURCE_DIR}/chess_pieces.json ${CMAKE_BINARY_DIR}/chess_pieces.json COPYONLY)