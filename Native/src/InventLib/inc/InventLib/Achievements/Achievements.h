#pragma once

#include "InventLib/Mechanics/Achievement.h"
#include "InventLib/Mechanics/Unlockable.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace Invent {
	namespace Achievements {
		void Initialize();

		extern std::string FirstTimeAchievement;
		extern std::unordered_map<std::string, std::pair<Unlockable, Achievement>> TimeAchievements;

		extern std::string FirstResourceAchievement;
		extern std::unordered_map<std::string, std::pair<Unlockable, Achievement>> ResourceAchievements;

		extern std::string FirstStorageAchievement;
		extern std::unordered_map<std::string, std::pair<Unlockable, Achievement>> StorageAchievements;

		extern std::string FirstTechnologyAchievement;
		extern std::unordered_map<std::string, std::pair<Unlockable, Achievement>> TechnologyAchievements;

		extern std::string FirstInventionAchievement;
		extern std::unordered_map<std::string, std::pair<Unlockable, Achievement>> InventionAchievements;

		extern std::string FirstHiddenAchievement;
		extern std::unordered_map<std::string, std::pair<Unlockable, Achievement>> HiddenAchievements;
	}
}