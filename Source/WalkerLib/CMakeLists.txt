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
	src/WalkerRates.cpp

	src/Crew/CrewManager.cpp
	src/Crew/JobProgress.cpp
	src/Crew/WalkerTech.cpp

	src/Home/Garage.cpp
	src/Home/HomeBase.cpp
	src/Home/TimeBank.cpp

	src/Journey/CargoTransfer.cpp
	src/Journey/EndpointDetails.cpp
	src/Journey/EndpointNames.cpp
	src/Journey/Endpoints.cpp
	src/Journey/Journey.cpp

	src/Milestones/WalkerStats.cpp

	src/Travel/TravelLeg.cpp
	src/Travel/Vehicle.cpp
	src/Travel/VehicleDetails.cpp
	
	src/Ui/EtaProgress.cpp
	src/Ui/Selectors.cpp
	src/Ui/WalkerContent.cpp
	src/Ui/WalkerHeader.cpp
	src/Ui/WalkerLayout.cpp
	src/Ui/WalkerNav.cpp

	src/Ui/Screens/CrewScreen.cpp
	src/Ui/Screens/RebirthScreen.cpp
	src/Ui/Screens/SettingsScreen.cpp
	src/Ui/Screens/StoreScreen.cpp
	src/Ui/Screens/TravelScreen.cpp
	src/Ui/Screens/WalkerScreens.cpp
  "src/Milestones/Milestones.cpp" "src/Milestones/MilestoneDetails.cpp" "inc/Walker/Ui/Screens/MilestonesScreen.h" "src/Ui/Screens/MilestonesScreen.cpp")