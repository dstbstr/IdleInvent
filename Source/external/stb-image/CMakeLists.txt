project(stbimage)

include(FetchContent)

FetchContent_Declare(stb_upstream
	GIT_REPOSITORY https://github.com/nothings/stb.git
	GIT_TAG 2c980bb59875b0d32144a71867fbdebb2f77cd20
)
FetchContent_MakeAvailable(stb_upstream)

add_library(stbimage INTERFACE)
target_include_directories(stbimage SYSTEM INTERFACE "${stb_upstream_SOURCE_DIR}")
