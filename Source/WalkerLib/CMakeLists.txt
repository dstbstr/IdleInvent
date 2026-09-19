project(WalkerLib)

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
	src/WalkerGame.cpp

	src/Journey/EndpointDetails.cpp
	src/Journey/Endpoints.cpp
	src/Journey/Journey.cpp

	src/Travel/VehicleDetails.cpp
	
	src/Ui/WalkerContent.cpp
	src/Ui/WalkerHeader.cpp
	src/Ui/WalkerLayout.cpp
	src/Ui/WalkerNav.cpp

	src/Ui/Screens/CrewScreen.cpp
	src/Ui/Screens/RebirthScreen.cpp
	src/Ui/Screens/SettingsScreen.cpp
	src/Ui/Screens/TravelScreen.cpp
	src/Ui/Screens/WalkerScreens.cpp
)