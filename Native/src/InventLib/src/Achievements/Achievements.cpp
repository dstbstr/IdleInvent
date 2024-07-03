#include "InventLib/Achievements/Achievements.h"

#include "Core/DesignPatterns/PubSub.h"
#include "Core/DesignPatterns/ServiceLocator.h"

#include "InventLib/Mechanics/Unlockable.h"

#include <unordered_map>
#include <ranges>

namespace {
    u64 Unlocked = 0;
}

namespace Invent {
	namespace Achievements {
		std::vector<Achievement> All;

		void Initialize() {
            Log::Info("Initializing Achievements");
            All.clear();
            All.insert(All.end(), Time.begin(), Time.end());

			auto& achievementPs = ServiceLocator::Get().GetOrCreate<PubSub<Achievement>>();

			achievementPs.Subscribe([](const Achievement& achievement) {
                auto where = std::find(All.begin(), All.end(), achievement);
				auto index = std::distance(All.begin(), where);
				Unlocked |= 1ull << index;
			});
		}

		void Load(const SaveState& save) {
            Log::Info("Loading Achievements");
            Unlocked = save.Achievements0;
            auto& unlockables = ServiceLocator::Get().GetOrCreate<std::unordered_map<std::string, Unlockable>>();

            // add in unlockables based on save data
            for(auto i = 0u; i < std::min(size_t(64), All.size()); i++) {
                bool unlocked = save.Achievements0 & (1ull << i);
                if(!unlocked) {
                    unlockables.emplace(All[i].Name, All[i].Unlock);
                }
            }
		}

		void Save(SaveState& save) {
			Log::Info("Saving Achievements");
			save.Achievements0 = Unlocked;
		}
	}
}