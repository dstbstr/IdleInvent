project(GhostHunterLibTest)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
  Engine
  GhostHunterLib

  gtest
  gmock
)

target_include_directories(${PROJECT_NAME} PUBLIC
	inc
)

include(GoogleTest)
#gtest_discover_tests(${PROJECT_NAME})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
  src/main.cpp
  src/Media/Market.Test.cpp
)
