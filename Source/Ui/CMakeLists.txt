project(IdleUi LANGUAGES CXX)

add_library(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PUBLIC
    imgui
    stbimage
    IdlePlatform
    Engine
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    inc 
)

enable_project_clang_tidy(${PROJECT_NAME})

# Enable precompiled headers for build performance
target_precompile_headers(${PROJECT_NAME} PRIVATE inc/pch.h)

target_sources(${PROJECT_NAME} PRIVATE
    src/Dialog.cpp
    src/Overlay.cpp
    src/UiGeometry.cpp
    src/UiUtil.cpp

    src/Combat/BattleFormation.cpp

    src/Panel/Panel.cpp
    src/Panel/CanvasPanel.cpp

    src/Particle/ParticleSystem.cpp

    inc/Ui/Direction.h
    inc/Ui/Overlay.h
    inc/Ui/Dialog.h
)