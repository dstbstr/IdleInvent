project(implot)

include(FetchContent)

FetchContent_Declare(implot_upstream
	GIT_REPOSITORY https://github.com/epezent/implot.git
	GIT_TAG v1.1
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(implot_upstream)

add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
	${implot_upstream_SOURCE_DIR}
)
target_sources(${PROJECT_NAME} PRIVATE
	${implot_upstream_SOURCE_DIR}/implot.cpp
	${implot_upstream_SOURCE_DIR}/implot_items.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
	imgui
)

# Suppress warnings when compiling implot sources (third-party code).
# Use generator expressions to apply appropriate flags per compiler.
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/w>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-w>
)