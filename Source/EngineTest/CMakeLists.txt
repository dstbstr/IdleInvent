project(EngineTest)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
  Engine
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
  src/Main.cpp

  src/Constexpr/ConstexprMath.test.cpp
  src/Constexpr/ConstexprSaveUtils.test.cpp
  src/Constexpr/ConstexprStrUtils.test.cpp

  src/DesignPatterns/PubSub.test.cpp 
  src/DesignPatterns/ServiceLocator.test.cpp

  src/Mechanics/Advancement.tests.cpp
  src/Mechanics/EventManager.tests.cpp
  src/Mechanics/Purchasable.test.cpp
  src/Mechanics/Progression.tests.cpp
  src/Mechanics/ScaleFunc.tests.cpp

  src/Instrumentation/DebugContext.test.cpp 
  src/Instrumentation/Logging.test.cpp

  src/Utilities/Format.test.cpp
  src/Utilities/IRandom.test.cpp 
  src/Utilities/EnumUtils.test.cpp
 "src/Mechanics/Upgrade.test.cpp" "src/Utilities/Concepts.test.cpp")
 
