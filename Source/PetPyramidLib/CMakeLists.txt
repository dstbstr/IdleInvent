project(PetPyramidLib)

add_library(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
	Engine
	IdlePlatform
	IdleUi
	imgui
)

target_include_directories(${PROJECT_NAME} PUBLIC inc)
target_precompile_headers(${PROJECT_NAME} PRIVATE inc/pch.h)
enable_project_clang_tidy(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
	src/PetPyramidGame.cpp

	src/Character/Ability.cpp

	src/Combat/HuntControllers.cpp
	src/Combat/HuntManager.cpp
    src/Combat/HuntRules.cpp
	src/Combat/PartyProvider.cpp
	src/Combat/PreyProvider.cpp

	src/Inventory/Inventory.cpp

	src/Pets/PetDetails.cpp
	src/Pets/Pets.cpp

	src/Ui/BottomContent.cpp
	src/Ui/MainContent.cpp
	src/Ui/PetVisual.cpp
	src/Ui/TopContent.cpp
	src/Ui/Ui.cpp

	src/Ui/Screens/BestiaryScreen.cpp
	src/Ui/Screens/CombatScreen.cpp
	src/Ui/Screens/PetsScreen.cpp
	src/Ui/Screens/RebirthScreen.cpp
	src/Ui/Screens/Screens.cpp
)