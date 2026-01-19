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
    src/Formatting.cpp

    src/Ui/Ui.cpp 
    src/Ui/MainContent.cpp 
    src/Ui/BottomContent.cpp 
    src/Ui/TopContent.cpp
  
    src/UI/Screens/Screens.cpp
    src/UI/Screens/JournalScreen.cpp
    src/UI/Screens/LocationScreen.cpp
    src/UI/Screens/MediaScreen.cpp
    src/UI/Screens/PrestigeScreen.cpp
    src/UI/Screens/SettingsScreen.cpp
    src/UI/Screens/ToolsScreen.cpp
    
    src/Inventory/Inventory.cpp
    src/Locations/Locations.cpp
    src/Media/Media.cpp
    src/Media/Market.cpp
    src/Quality/Quality.cpp
    src/Resources/GhostHunterResources.cpp
    src/Tools/Tools.cpp
 "inc/GhostHunter/Investigation/Investigation.h" "src/Investigation/Investigation.cpp")