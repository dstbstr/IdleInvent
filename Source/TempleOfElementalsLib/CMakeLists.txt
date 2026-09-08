project(TempleOfElementalsLib)

add_library(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
        Engine
        IdlePlatform
        IdleUi
        imgui
)

target_include_directories(${PROJECT_NAME} PUBLIC inc)
enable_project_clang_tidy(${PROJECT_NAME})

# Enable precompiled headers for build performance
target_precompile_headers(${PROJECT_NAME} PRIVATE inc/pch.h)

target_sources(${PROJECT_NAME} PRIVATE
    src/ToeGame.cpp
)
