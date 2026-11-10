project(IduelLib)

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
	src/IduelGame.cpp
 
	src/Ui/IduelLayout.cpp 
	 
	src/Ui/Screens/IduelScreens.cpp 
	src/Ui/Screens/IduelMatchScreen.cpp
	src/Ui/Screens/IduelStartScreen.cpp
	src/Ui/Screens/IduelReviewScreen.cpp
)