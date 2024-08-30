project(InventLibTest)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
  Core
  InventLib

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
  src/Game.test.cpp
  
  src/Achievements/Achievements.test.cpp

  src/Character/Life.test.cpp
  src/Character/Society.test.cpp

  src/Mechanics/Advancement.tests.cpp
  src/Mechanics/Purchasable.test.cpp
  src/Mechanics/Progression.tests.cpp
  src/Mechanics/ScaleFunc.tests.cpp

  src/Projects/Building.test.cpp
  src/Projects/Expeditions.test.cpp
  src/Projects/Population.test.cpp
  src/Projects/Project.test.cpp

  src/Resources/Resource.tests.cpp
  src/Resources/ResourceConversion.tests.cpp
  src/Resources/Storage.tests.cpp

  src/Technology/Technology.test.cpp
  src/Technology/InfluenceTechs.tests.cpp
)
