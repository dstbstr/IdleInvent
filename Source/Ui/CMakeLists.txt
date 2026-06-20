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
    src/Panel.cpp
    src/UiUtil.cpp
 "src/TreePanel.cpp")