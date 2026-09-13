project(imgui)

include(FetchContent)

FetchContent_Declare(imgui_upstream
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG v1.92.9b
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui_upstream)

add_library(${PROJECT_NAME})
target_compile_definitions(${PROJECT_NAME} PUBLIC
	IMGUI_USER_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/IdleImGuiConfig.h"
)

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
	${imgui_upstream_SOURCE_DIR}
	${imgui_upstream_SOURCE_DIR}/backends
)
target_sources(${PROJECT_NAME} PRIVATE
	${imgui_upstream_SOURCE_DIR}/imgui.cpp
	${imgui_upstream_SOURCE_DIR}/imgui_demo.cpp
	${imgui_upstream_SOURCE_DIR}/imgui_draw.cpp
	${imgui_upstream_SOURCE_DIR}/imgui_tables.cpp
	${imgui_upstream_SOURCE_DIR}/imgui_widgets.cpp
)

if(WIN32)
	target_sources(${PROJECT_NAME} PRIVATE
		${imgui_upstream_SOURCE_DIR}/backends/imgui_impl_win32.cpp
		${imgui_upstream_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
	)
	#TODO: Double check this
	target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_WIN32_DISABLE_GAMEPAD)
elseif(ANDROID)
	target_sources(${PROJECT_NAME} PRIVATE
		${imgui_upstream_SOURCE_DIR}/backends/imgui_impl_android.cpp
		${imgui_upstream_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
	)
	target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_ES3)
elseif(UNIX)
	target_sources(${PROJECT_NAME} PRIVATE
		${imgui_upstream_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
		${imgui_upstream_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
	)
	target_link_libraries(${PROJECT_NAME} PUBLIC glfw OpenGL::GL)
endif()

# Suppress warnings when compiling imgui sources (third-party code).
# Use generator expressions to apply appropriate flags per compiler.
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/w>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-w>
)