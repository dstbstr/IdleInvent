#include "Mechanics/Unlockable.h"
#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"
#include "Instrumentation/Logging.h"

bool Unlockable::TryUnlock(std::vector<Unlockable>& outNewUnlockables) {
	if (!UnlockCondition()) {
		return false;
	}

	Log::Info("Unlocking: " + Name);
	auto result = OnUnlock();
	outNewUnlockables.insert(outNewUnlockables.end(), result.begin(), result.end());
	return true;
}

namespace Unlockables {
	void Tick() {
		auto& unlockables = ServiceLocator::Get().GetRequired<std::unordered_map<std::string, Unlockable>>();

		Log::Debug(std::format("Ticking unlockables: {}", unlockables.size()));
		std::vector<std::string> unlocked;
		std::vector<Unlockable> newUnlockables;
		for (auto& [name, unlockable] : unlockables) {
			if (unlockable.TryUnlock(newUnlockables)) {
				unlocked.push_back(name);
			}
		}

		for (auto& key : unlocked) {
			unlockables.erase(key);
		}
		for (auto& unlockable : newUnlockables) {
			unlockables.insert({ unlockable.Name, unlockable });
		}
	}
}
