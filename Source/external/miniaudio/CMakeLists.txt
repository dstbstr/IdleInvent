project(miniaudio)

include(FetchContent)

FetchContent_Declare(miniaudio_upstream
	GIT_REPOSITORY https://github.com/mackron/miniaudio.git
	GIT_TAG 0.11.25
	SOURCE_SUBDIR _fetch_only
)
FetchContent_MakeAvailable(miniaudio_upstream)

add_library(miniaudio STATIC "${miniaudio_upstream_SOURCE_DIR}/miniaudio.c")
target_include_directories(miniaudio SYSTEM PUBLIC "${miniaudio_upstream_SOURCE_DIR}")
set_target_properties(miniaudio PROPERTIES POSITION_INDEPENDENT_CODE ON)

find_package(Threads REQUIRED)
target_link_libraries(miniaudio PRIVATE Threads::Threads ${CMAKE_DL_LIBS})

if(UNIX)
    target_link_libraries(miniaudio PRIVATE m)
endif()

target_compile_options(miniaudio PRIVATE
    $<$<C_COMPILER_ID:MSVC>:/w>
    $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:-w>
)