#include "InventLib/Achievements/Achievements.h"

#include "Core/DesignPatterns/PubSub.h"
#include "Core/DesignPatterns/ServiceLocator.h"

#include "InventLib/Mechanics/Unlockable.h"

#include <unordered_map>

namespace Invent {
	namespace Achievements {
		void Initialize() {
			auto& services = ServiceLocator::Get();
			services.CreateIfMissing<PubSub<Achievement>>();
			auto& unlockables = services.GetOrCreate<std::unordered_map<std::string, Unlockable>>();

			if (!unlockables.contains(FirstTimeAchievement)) {
				unlockables.emplace(FirstTimeAchievement, TimeAchievements.at(FirstTimeAchievement).first);
			}
		}
	}
}