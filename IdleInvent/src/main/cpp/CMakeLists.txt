cmake_minimum_required(VERSION 3.4.1)
project(IdleInvent)

set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")

get_filename_component(NativeDir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Native ABSOLUTE)
get_filename_component(ExternalDir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../external ABSOLUTE)

add_library(imgui STATIC
    ${ExternalDir}/imgui/imgui.cpp
    ${ExternalDir}/imgui/imgui_demo.cpp
    ${ExternalDir}/imgui/imgui_draw.cpp
    ${ExternalDir}/imgui/imgui_tables.cpp
    ${ExternalDir}/imgui/imgui_widgets.cpp
    ${ExternalDir}/imgui/backends/imgui_impl_android.cpp
    ${ExternalDir}/imgui/backends/imgui_impl_opengl3.cpp
)

target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_ES3)

target_include_directories(imgui PRIVATE
    ${ExternalDir}/imgui
    ${ExternalDir}/imgui/backends
)

target_link_libraries(imgui PRIVATE GLESv3)

add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23 -Wall -Wextra -Werror -fno-rtti -fno-exceptions -g")

message("Build Type: ${CMAKE_BUILD_TYPE}")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DDEBUG)
else()
    add_definitions(-DRELEASE)
endif()

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(${PROJECT_NAME} SHARED)

add_library(Core STATIC IMPORTED)
set_target_properties(Core PROPERTIES IMPORTED_LOCATION ${NativeDir}/dist/Core/lib/${ANDROID_ABI}/libCore.a)
add_library(InventLib STATIC IMPORTED)
set_target_properties(InventLib PROPERTIES IMPORTED_LOCATION ${NativeDir}/dist/InventLib/lib/${ANDROID_ABI}/libInventLib.a)


set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    android

    native_app_glue
    imgui

    EGL
    GLESv1_CM
    log

    Core
    InventLib
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src

    ${ANDROID_NDK}/sources/android/native_app_glue

    ${ExternalDir}/imgui
    ${ExternalDir}/stb-image

    ${NativeDir}/dist/Core/inc
    ${NativeDir}/dist/InventLib/inc
)

target_include_directories(${PROJECT_NAME} PUBLIC
    inc
)

target_sources(${PROJECT_NAME}
PRIVATE
    src/Engine.cpp
    src/Ui/Ui.cpp
    src/Ui/BottomBar.cpp
    src/Ui/TopBar.cpp
    src/Ui/Screens.cpp
    src/Ui/Screens/Achievements.cpp
    src/Ui/Screens/Resources.cpp
    src/Ui/Screens/Settings.cpp
    src/Ui/Screens/Splash.cpp
    src/Ui/Screens/Storages.cpp
)

if(ANDROID)
    target_sources(${PROJECT_NAME}
    PRIVATE
        src/Platform/Android/Main.cpp
        src/Platform/Android/GameLog.cpp
        src/Platform/Android/Platform.cpp
        src/Platform/Android/Graphics.cpp
        src/Platform/Android/Inputs.cpp
    )
endif()