project(CoreTest)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
  Core
  gtest
  gmock
)

target_include_directories(${PROJECT_NAME} PUBLIC
	inc
)

enable_testing()

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
  src/main.cpp
  src/DesignPatterns/ServiceLocator.test.cpp
  src/DesignPatterns/PubSub.test.cpp 
  src/Instrumentation/DebugContext.test.cpp 
  src/Instrumentation/Logging.test.cpp
  src/Utilities/Format.test.cpp
  src/Utilities/IRandom.test.cpp)
