project(imgui)

add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/backends
)
target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp
)

if(WIN32)
	target_sources(${PROJECT_NAME} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_win32.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
	)
	#TODO: Double check this
	target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_WIN32_DISABLE_GAMEPAD)
elseif(ANDROID)
	target_sources(${PROJECT_NAME} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_android.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
	)
	target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_ES3)
endif()