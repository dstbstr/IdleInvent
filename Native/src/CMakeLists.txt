cmake_minimum_required(VERSION 3.10)
project(NativeLibs)

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
FetchContent_MakeAvailable(googletest)


#################################

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_definitions(-DGNU)
    else()
        add_definitions(-DCLANG)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc")
    add_definitions(-DMSVC)
endif()
if (NOT CONFIGURED_ONCE)
    set(CMAKE_CXX_FLAGS "${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_C_FLAGS   "${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DDEBUG)
else()
    add_definitions(-DRELEASE)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(dist_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../dist)

function(make_native_lib)
    add_library(${PROJECT_NAME})
    set(out_DIR ${dist_DIR}/${ANDROID_ABI}/${PROJECT_NAME})

    target_include_directories(${PROJECT_NAME} PUBLIC inc)

    set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_DIR}/lib)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${out_DIR}/inc
    )
endfunction()

add_subdirectory(Core)
add_subdirectory(CoreTest)
add_subdirectory(InventLib)
add_subdirectory(InventLibTest)

enable_testing()