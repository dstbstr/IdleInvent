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

set_Target_properties(${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY "clang-tidy")

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