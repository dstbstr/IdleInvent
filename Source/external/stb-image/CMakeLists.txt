project(stbimage)

# Provide an INTERFACE target so consumers can link against `stbimage`.
# Mark the include directory as SYSTEM so compiler warnings from these headers are suppressed.
add_library(stbimage INTERFACE)
target_include_directories(stbimage INTERFACE SYSTEM
    ${CMAKE_CURRENT_SOURCE_DIR}
)