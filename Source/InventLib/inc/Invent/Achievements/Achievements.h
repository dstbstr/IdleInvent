#pragma once

#include <Mechanics/Achievement.h>
#include <Mechanics/Unlockable.h>
#include <Platform/NumTypes.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace Invent {
	namespace Achievements {
		struct SaveState {
            u64 Achievements0; // 0-63
		};

		void Initialize();
		void Load(const SaveState& save);
		void Save(SaveState& save);

		extern std::vector<Achievement> All;
        extern std::vector<Achievement> Time;
        /*
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
		*/
	}
}