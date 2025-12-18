project(GhostHunterLib)

add_library(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
	Engine
    IdlePlatform
    imgui
)

target_include_directories(${PROJECT_NAME} PUBLIC inc)
target_sources(${PROJECT_NAME} PRIVATE
    src/GhostHunterGame.cpp

    src/Ui/Ui.cpp 
    src/Ui/MainContent.cpp 
    src/Ui/BottomContent.cpp 
    src/Ui/TopContent.cpp
 )