project(InventLib)

add_library(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
	Engine
    IdlePlatform
    imgui
)

target_include_directories(${PROJECT_NAME} PUBLIC inc)
# target_link_libraries(${PROJECT_NAME} PRIVATE
# 	stbimage
# )
#target_include_directories(${PROJECT_NAME} PRIVATE ${stbimage_INCLUDE_DIRS})

target_sources(${PROJECT_NAME} PRIVATE
	src/InventGame.cpp

	src/Achievements/Achievements.cpp
	src/Achievements/TimeAchievements.cpp

	src/Character/Death.cpp
	src/Character/GameCharacter.cpp
	src/Character/Life.cpp
	src/Character/RebirthBenefits.cpp
	src/Character/Society.cpp

	src/GameState/InventGameState.cpp

	src/Inventory/Item.cpp 

	src/Projects/Building.cpp
	src/Projects/Expeditions.cpp
	src/Projects/Population.cpp 
	src/Projects/Project.cpp
	src/Projects/Research.cpp

	src/RandomEvents/InfluenceEvents.cpp
	src/RandomEvents/KnowledgeEvents.cpp
	src/RandomEvents/LaborEvents.cpp
	src/RandomEvents/MagicEvents.cpp
	src/RandomEvents/NaturalEvents.cpp
	src/RandomEvents/RandomEvents.cpp
	src/RandomEvents/WealthEvents.cpp

	src/Resources/InventResources.cpp
	src/Resources/InventStorage.cpp

	src/Technology/Inventions/StoneAge.cpp
	src/Technology/Inventions/LaborTechs.cpp
	src/Technology/Inventions/KnowledgeTechs.cpp
	src/Technology/Inventions/WealthTechs.cpp
	src/Technology/Inventions/InfluenceTechs.cpp
	src/Technology/Inventions/MagicTechs.cpp
	src/Technology/Invention.cpp
	src/Technology/Technology.cpp
	src/Technology/Inventions/StorageTechs.cpp

	src/Ui/Ui.cpp
    src/Ui/Splash.cpp
    src/Ui/StartLife.cpp

    src/Ui/Components/ProjectComponent.cpp

    src/Ui/Layout/EmptyBar.cpp
    src/Ui/Layout/Layout.cpp
    src/Ui/Layout/NavBar.cpp
    src/Ui/Layout/NewsFeed.cpp
    src/Ui/Layout/Popups.cpp
    src/Ui/Layout/PrimaryResource.cpp
    src/Ui/Layout/ResourceBar.cpp
    src/Ui/Layout/TopBar.cpp

    src/Ui/Screens/Achievements.cpp
    src/Ui/Screens/Build.cpp
    src/Ui/Screens/Explore.cpp
    src/Ui/Screens/Home.cpp
    src/Ui/Screens/Purchases.cpp
    src/Ui/Screens/Research.cpp
    src/Ui/Screens/Screens.cpp
    src/Ui/Screens/Settings.cpp
)