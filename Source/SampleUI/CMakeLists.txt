project(SampleUI LANGUAGES CXX)

add_executable(${PROJECT_NAME})

set(SAMPLE_ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/${PROJECT_NAME})
set(SHARED_ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/Shared)

if(WIN32)
	set(ASSET_DEST ${CMAKE_CURRENT_BINARY_DIR}/assets)
elseif(ANDROID)
	set(ASSET_DEST ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_apk/src/main/assets)
endif()

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		Engine
		IdlePlatform
		IdleUi
)

target_include_directories(${PROJECT_NAME} PUBLIC
	inc
)

set_Target_properties(${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY "clang-tidy")

target_sources(${PROJECT_NAME} PRIVATE
	src/Main.cpp
	src/Ui/Ui.cpp
	src/Ui/Screens/Screens.cpp
	src/Ui/Screens/Landing.cpp
	src/Ui/Screens/SampleTreePanel.cpp
 "src/Ui/Screens/SampleParticles.cpp")

if(EXISTS ${SHARED_ASSET_DIR})
	add_custom_target(SyncSampleUIAssets
		COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${SHARED_ASSET_DIR} ${ASSET_DEST}
		VERBATIM
	)

	if(EXISTS ${SAMPLE_ASSET_DIR})
		add_custom_command(TARGET SyncSampleUIAssets POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${SAMPLE_ASSET_DIR} ${ASSET_DEST}
			VERBATIM
		)
	endif()

	add_dependencies(${PROJECT_NAME} SyncSampleUIAssets)
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${ASSET_DEST})
endif()
