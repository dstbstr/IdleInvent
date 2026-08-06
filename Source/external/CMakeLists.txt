project(IdleExternal)

# GLFW provides the desktop window/input backend for imgui on Linux.
# Prefer a system install, fall back to fetching the source.
if(UNIX AND NOT ANDROID AND NOT APPLE)
    # GLOBAL so the imported targets are visible outside this directory (Platform links both)
    find_package(OpenGL REQUIRED GLOBAL)
    find_package(glfw3 3.3 QUIET GLOBAL)
    if(NOT glfw3_FOUND)
        include(FetchContent)
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
        # X11 only: Wayland requires wayland-scanner/wayland-protocols at build time,
        # and XWayland covers Wayland sessions.
        set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
        set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
        FetchContent_MakeAvailable(glfw)
        # This project's warning flags land in the global CMAKE_C_FLAGS, and glfw is not
        # written to build clean under them. Suppress warnings for third-party code.
        target_compile_options(glfw PRIVATE -w)
    endif()
endif()

add_subdirectory(imgui)
add_subdirectory(stb-image)
