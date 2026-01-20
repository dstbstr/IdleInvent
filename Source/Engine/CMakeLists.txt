project(Engine)

add_library(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
	IdlePlatform
)

target_include_directories(${PROJECT_NAME} PUBLIC inc)

target_sources(${PROJECT_NAME} PRIVATE
	src/Engine.cpp

	src/GameState/GameState.cpp

	src/Instrumentation/Logging.cpp
	src/Instrumentation/ISink.cpp
	src/Instrumentation/LogWriter/StdOutLogWriter.cpp

	src/Mechanics/Advancement.cpp
	src/Mechanics/Effect.cpp
	src/Mechanics/Modifier.cpp
	src/Mechanics/Progression.cpp
	src/Mechanics/Purchasable.cpp
	src/Mechanics/RandomEvent.cpp
	src/Mechanics/Unlockable.cpp

	src/Resources/Resource.cpp
	
	src/Resources/Storage.cpp

	src/Settings/PurchaseAmount.cpp
	src/Resources/ResourceConversion.cpp 
 "src/Mechanics/EventManager.cpp")
