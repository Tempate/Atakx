cmake_minimum_required(VERSION 3.12)

# Project
project(Funes VERSION 1.0 LANGUAGES CXX)

# Base directory relative includes
include_directories(.)

# Flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -flto -march=native -mpopcnt")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add the executable
add_executable(
    Funes
    src/main.cpp
    src/bitboard.cpp
    src/board.cpp
    src/uai.cpp
    src/lookup.cpp
    src/hashtables.cpp
    src/moves.cpp
    src/alphabeta.cpp
    src/mcts.cpp
    src/eval.cpp
    src/tuner/tuner.cpp
    src/tuner/population.cpp
    src/tuner/player.cpp
)
