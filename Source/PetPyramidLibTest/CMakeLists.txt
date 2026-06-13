project(PetPyramidLibTest)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
  Engine
  PetPyramidLib

  gtest
  gmock
)

include(GoogleTest)
#gtest_discover_tests(${PROJECT_NAME})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
  src/main.cpp
  src/Smoke.Test.cpp
)
