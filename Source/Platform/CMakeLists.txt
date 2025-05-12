project(IdlePlatform LANGUAGES CXX)

add_library(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PUBLIC
    imgui
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    inc 
    ../external/stb-image
)

if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE
        src/Win/Win_DebugStream.cpp
        src/Win/Win_Graphics.cpp
        #src/Win/Win_Image.cpp
        src/Win/Win_Platform.cpp
        src/Win/D3dAllocator.cpp
        src/Win/D3dContext.cpp 
        "src/Ui/Panel.cpp"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE
        d3d12
        dxgi
        dxguid
    )
elseif (ANDROID)
    add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    target_sources(${PROJECT_NAME} PRIVATE
        src/Android/Android_DebugStream.cpp
        src/Android/Android_Graphics.cpp
        src/Android/Android_Platform.cpp
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        android
        native_app_glue

        EGL
        GLESv3
        log
    )
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue
    )
endif()