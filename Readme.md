# Idle Invent
This is really a collection of a bunch of different games built on the same engine.

## Getting Started

### Required Tools
* git
* git-lfs
* C++ Compiler (clang/msvc) + Ninja
* Clang-Tidy (LLVM tools)

### Commands
```bash
cd <your repo dir>
git clone https://github.com/dstbstr/IdleInvent.git
cd IdleInvent
git submodule update --init --recursive
git-lfs pull
```

## Creating new games
* Figure out what you want to call the new project
* Under Source, create a folder for <Game>Lib
* Add inc, src folders
* Add CMakeLists.txt
* Copy an existing lib (like TechMazeLib/CMakeLists.txt)
* In the root CMakeLists.txt
    * add_subdirectory(<Game>Lib)
* In the Game/CMakeLists.txt
    * Add <Game> to the list of game proejcts
    * Set the GAME to <Game>
    * Add a block to set the GAME_LIB, GAME_HEADER, and GAME_TYPE to match
* In <Game>Lib/inc/<Game>
    * Create a <Game>Game.h
    * Copy an existing one
* In <Game>Lib/src
    * Create the <Game>Game.cpp file
    * Copy an existing one
* Optionally
    * Create <Game>LibTest project
* Clear the CMake cache and rebuild