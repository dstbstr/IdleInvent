cmake_minimum_required(VERSION 3.16)
project(IdleGame)

include(FetchContent)

#################################
# GTest
#################################
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE) #disable cmake 3.5 warning in gtest

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

#################################

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        list(APPEND compiler_definitions GNU)
    else()
        list(APPEND compiler_definitions CLANG)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc")
    list(APPEND compiler_definitions MSVC)
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
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external)
add_subdirectory(Platform)
add_subdirectory(Engine)
add_subdirectory(InventLib)
add_subdirectory(TechMazeLib)
add_subdirectory(Game)
if(IncludeTests)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(EngineTest)
    add_subdirectory(InventLibTest)
    enable_testing()
endif()


# Apply compiler definitions to all targets
foreach(target IN ITEMS IdlePlatform Engine InventLib IdleGame)
    target_compile_definitions(${target} PRIVATE ${compiler_definitions})
endforeach()
if(IncludeTests)
    foreach(target IN ITEMS EngineTest InventLibTest)
        target_compile_definitions(${target} PRIVATE ${compiler_definitions} TEST)
    endforeach()
endif()