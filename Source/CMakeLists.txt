cmake_minimum_required(VERSION 3.31)
project(IdleGame)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# No C++20 modules here, so skip the scan. Clang would otherwise need clang-scan-deps installed.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# The project does not currently use C++ modules. Disable Ninja's automatic
# module dependency scanning until module sources are enabled.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(ENABLE_CLANG_TIDY "Run clang-tidy during compilation" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy REQUIRED)
endif()

function(enable_project_clang_tidy target)
    if(ENABLE_CLANG_TIDY)
        set_property(TARGET ${target} PROPERTY CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE}")
    endif()
endfunction()

add_subdirectory(external)

include(FetchContent)

#################################
# GTest
#################################
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE) #disable cmake 3.5 warning in gtest

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

#################################
# Durandal Libs
#################################

# Only the Atlas tool uses CliParser, and that is Windows only (see Tools/CMakeLists.txt)
if(WIN32)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CliParser)
        add_subdirectory("CliParser")
    else()
        FetchContent_Declare(CliParser GIT_REPOSITORY https://github.com/dstbstr/CliParser GIT_TAG origin/main)
        FetchContent_MakeAvailable(CliParser)
    endif()
endif()

#################################

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Wconversion -Wshadow")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        list(APPEND compiler_definitions GNU)
    else()
        list(APPEND compiler_definitions CLANG)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc")
    list(APPEND compiler_definitions MSVC)
    add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/std:c++latest>)
endif()
if (NOT CONFIGURED_ONCE)
    set(CMAKE_CXX_FLAGS "${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_C_FLAGS   "${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    list(APPEND compiler_definitions DEBUG)
else()
    list(APPEND compiler_definitions RELEASE)
endif()

if(WIN32)
    list(APPEND compiler_definitions WIN32)
    set(IncludeTests ON)
elseif(ANDROID)
    list(APPEND compiler_definitions __ANDROID__)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(IncludeTests OFF)
    include(CMake/AndroidPackaging.cmake)
elseif(UNIX)
    # __linux__ is predefined by the compiler, so there is nothing to add here
    set(IncludeTests ON)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

#set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

# Targets opt in via CXX_CLANG_TIDY; an empty value disables it where clang-tidy isn't installed
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(NOT CLANG_TIDY_EXE)
    set(CLANG_TIDY_EXE "") # find_program leaves a -NOTFOUND string behind, which CXX_CLANG_TIDY would try to run
    message(STATUS "clang-tidy not found, skipping static analysis")
endif()

add_subdirectory(Tools)
add_subdirectory(Platform)
add_subdirectory(Ui)
add_subdirectory(Engine)
add_subdirectory(InventLib)
add_subdirectory(TechMazeLib)
add_subdirectory(GhostHunterLib)
add_subdirectory(PetPyramidLib)
add_subdirectory(Game)
add_subdirectory(SampleUI)
if(IncludeTests)
    FetchContent_MakeAvailable(googletest)
    enable_testing()

    add_subdirectory(EngineTest)
    add_subdirectory(InventLibTest)
    add_subdirectory(GhostHunterLibTest)
    add_subdirectory(PetPyramidLibTest)
endif()


# Apply compiler definitions to all targets
foreach(target IN ITEMS IdlePlatform Engine InventLib PetPyramidLib SampleUI)
    target_compile_definitions(${target} PRIVATE ${compiler_definitions})
endforeach()
if(IncludeTests)
    foreach(target IN ITEMS EngineTest InventLibTest GhostHunterLibTest PetPyramidLibTest)
        target_compile_definitions(${target} PRIVATE ${compiler_definitions} TEST)
    endforeach()
endif()