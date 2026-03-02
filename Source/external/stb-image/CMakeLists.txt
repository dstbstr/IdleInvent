project(stbimage)

# Provide an INTERFACE target so consumers can link against `stbimage`.
# Mark the include directory as SYSTEM so compiler warnings from these headers are suppressed.
add_library(stbimage INTERFACE)
#target_include_directories(stbimage INTERFACE
#    ${CMAKE_CURRENT_SOURCE_DIR}
#)
set_target_properties(stbimage PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
	INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)

