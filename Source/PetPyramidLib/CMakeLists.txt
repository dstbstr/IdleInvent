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
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY "clang-tidy")

target_sources(${PROJECT_NAME} PRIVATE
	src/PetPyramidGame.cpp

	src/Ui/Ui.cpp
	src/Ui/TopContent.cpp
	src/Ui/MainContent.cpp
	src/Ui/BottomContent.cpp

	src/Ui/Screens/Screens.cpp
	src/Ui/Screens/PetsScreen.cpp
	src/Ui/Screens/CombatScreen.cpp
	src/Ui/Screens/BestiaryScreen.cpp
	src/Ui/Screens/RebirthScreen.cpp
)
