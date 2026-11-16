project(IduelLib)

add_library(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
	Engine
	IdlePlatform
	IdleUi
	imgui
	implot
)

target_include_directories(${PROJECT_NAME} PUBLIC inc)
target_precompile_headers(${PROJECT_NAME} PRIVATE inc/pch.h)
enable_project_clang_tidy(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
	src/IduelGame.cpp
 
	src/Ui/IduelLayout.cpp 
	 
	src/Ui/Screens/IduelScreens.cpp 
	src/Ui/Screens/IduelMatchScreen.cpp
	src/Ui/Screens/IduelStartScreen.cpp
	src/Ui/Screens/IduelReviewScreen.cpp
 "inc/Iduel/Curve.h" "inc/Iduel/Economy/Generator.h" "inc/Iduel/Simulation/Simulation.h" "inc/Iduel/Simulation/IStrategy.h" "src/Ui/Screens/IduelEditorScreen.cpp" "src/Economy/Generator.cpp" "src/Simulation/Simulation.cpp" "inc/Iduel/Economy/EconomyState.h" "inc/Iduel/Simulation/ParticipantState.h" "src/Simulation/IStrategy.cpp")