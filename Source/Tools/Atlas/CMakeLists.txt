project(Atlas LANGUAGES CXX)
add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
	CliParser
	stbimage
)

target_include_directories(${PROJECT_NAME} PUBLIC
	inc
)

target_sources(${PROJECT_NAME} PRIVATE
	src/Main.cpp
	src/Commands.cpp
)