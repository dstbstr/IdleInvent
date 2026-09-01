project(Engine)

add_library(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
	IdlePlatform
)

target_include_directories(${PROJECT_NAME} PUBLIC inc)
enable_project_clang_tidy(${PROJECT_NAME})

# Enable precompiled headers for build performance
target_precompile_headers(${PROJECT_NAME} PRIVATE inc/pch.h)

target_sources(${PROJECT_NAME} PRIVATE
	src/Engine.cpp

	src/Animation/Animation.cpp 

	src/Combat/CombatTypes.cpp
	src/Social/Faction.cpp
	src/Social/Faction.test.cpp
	src/Combat/RoundRobinScheduler.cpp

	src/Constexpr/ConstexprStrUtils.cpp

	src/DesignPatterns/PubSub.cpp

	src/GameState/GameState.cpp

	src/Instrumentation/Logging.cpp
	src/Instrumentation/ISink.cpp
	src/Instrumentation/LogWriter/StdOutLogWriter.cpp

	src/Manage/EventManager.cpp
	src/Manage/TickManager.cpp
	
	src/Mechanics/Advancement.cpp
	src/Mechanics/Modifier.cpp
	src/Mechanics/Progression.cpp
	src/Mechanics/Unlockable.cpp

	src/Resources/Resource.cpp
	src/Resources/Storage.cpp
	src/Resources/ResourceConversion.cpp 
	
	src/Settings/PurchaseAmount.cpp

	src/Utilities/Handle.cpp
	
	src/World/Noise.cpp
	src/World/World.cpp
	src/World/Movement.cpp
 "src/Combat/RealTimeScheduler.cpp")
