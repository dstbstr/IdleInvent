project(PetPyramidLibTest)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
  Engine
  PetPyramidLib

  gtest
  gmock
)

# Enable precompiled headers for build performance
target_precompile_headers(${PROJECT_NAME} PRIVATE inc/pch.h)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
#add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
	src/main.cpp
	src/Smoke.Test.cpp
	src/Combat/HuntRules.test.cpp
)