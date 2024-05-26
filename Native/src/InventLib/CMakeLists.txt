project(InventLib)

make_native_lib()

target_link_libraries(${PROJECT_NAME} PRIVATE
  Core
)

target_sources(${PROJECT_NAME} PRIVATE
	src/EntryPoint.cpp

	src/Achievements/Achievements.cpp
	src/Achievements/TimeAchievements.cpp

	src/GameState/GameState.cpp

	src/Mechanics/Advancement.cpp
	src/Mechanics/RandomEvent.cpp
	src/Mechanics/Unlockable.cpp
	src/Mechanics/Effect.cpp
	src/Mechanics/Purchasable.cpp

	src/RandomEvents/RandomEvents.cpp
	src/RandomEvents/NaturalEvents.cpp
	src/RandomEvents/InfluenceEvents.cpp
	src/RandomEvents/LaborEvents.cpp
	src/RandomEvents/WealthEvents.cpp
	src/RandomEvents/KnowledgeEvents.cpp
	src/RandomEvents/MagicEvents.cpp
	
	src/Resources/Resource.cpp
	src/Resources/Storage.cpp

	src/Technology/Inventions/StoneAge.cpp
	src/Technology/Inventions/LaborTechs.cpp
	src/Technology/Inventions/KnowledgeTechs.cpp
	src/Technology/Inventions/WealthTechs.cpp
	src/Technology/Inventions/InfluenceTechs.cpp
	src/Technology/Inventions/MagicTechs.cpp
	src/Technology/Invention.cpp
	src/Technology/Technology.cpp
)