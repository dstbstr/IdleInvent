project(TechMazeLib)

add_library(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
	Engine
    IdlePlatform
    imgui
)

target_include_directories(${PROJECT_NAME} PUBLIC inc)
target_sources(${PROJECT_NAME} PRIVATE
    src/Technology.cpp
    src/Finder.cpp
    src/Decoder.cpp
 "inc/TechMaze/Ui/Ui.h" "src/Ui/Ui.cpp" "inc/TechMaze/Ui/MainContent.h" "src/Ui/MainContent.cpp" "inc/TechMaze/TechMazeGame.h" "src/TechMazeGame.cpp" "inc/TechMaze/Ui/BottomContent.h" "src/Ui/BottomContent.cpp" "inc/TechMaze/Ui/TopContent.h" "src/Ui/TopContent.cpp")