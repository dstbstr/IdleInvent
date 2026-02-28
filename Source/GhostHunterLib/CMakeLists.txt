project(GhostHunterLib)

add_library(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
	Engine
    IdlePlatform
    imgui
    #stbimage
)

target_include_directories(${PROJECT_NAME} PUBLIC inc)
target_precompile_headers(${PROJECT_NAME} PRIVATE inc/pch.h)

target_sources(${PROJECT_NAME} PRIVATE
    src/GhostHunterGame.cpp
    src/Formatting.cpp
    src/Purchase.cpp
    src/Upgrades.cpp

    src/Ui/Ui.cpp 
    src/Ui/MainContent.cpp 
    src/Ui/BottomContent.cpp 
    src/Ui/TopContent.cpp
  
    src/UI/Screens/Screens.cpp
    src/UI/Screens/EditingScreen.cpp
    src/UI/Screens/InvestigateScreen.cpp
    src/UI/Screens/MarketScreen.cpp
    src/UI/Screens/PrestigeScreen.cpp
    src/UI/Screens/SettingsScreen.cpp
    
    src/Inventory/Inventory.cpp

    src/Investigation/Investigation.cpp

    src/GameState/Life.cpp
    src/Locations/Locations.cpp
    src/Media/Media.cpp
    src/Media/Market.cpp
    
    
    src/Tools/Tools.cpp
    src/Tools/UseTool.cpp   
  "inc/GhostHunter/Tools/ToolAccumulators.h" "src/Tools/ToolAccumulators.cpp" "inc/GhostHunter/Locations/Room.h" "inc/GhostHunter/Locations/Ghost.h" "src/LocationDetails.cpp" "inc/GhostHunter/Teams/Team.h" "inc/GhostHunter/Teams/TeamMember.h"  "src/Teams/TeamMember.cpp" "inc/GhostHunter/GameState/Unlocks.h" "src/Sales.cpp" "inc/GhostHunter/Constants.h")