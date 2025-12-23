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
  "inc/GhostHunter/UI/Screens/Screens.h" "inc/GhostHunter/UI/Screens/MediaScreen.h" "inc/GhostHunter/UI/Screens/ToolsScreen.h" "inc/GhostHunter/UI/Screens/LocationScreen.h" "inc/GhostHunter/UI/Screens/JournalScreen.h" "inc/GhostHunter/UI/Screens/PrestigeScreen.h" "src/UI/Screens/Screens.cpp" "inc/GhostHunter/UI/Screens/SettingsScreen.h" "src/UI/Screens/JournalScreen.cpp" "src/UI/Screens/LocationScreen.cpp" "src/UI/Screens/MediaScreen.cpp" "src/UI/Screens/PrestigeScreen.cpp" "src/UI/Screens/SettingsScreen.cpp" "src/UI/Screens/ToolsScreen.cpp")