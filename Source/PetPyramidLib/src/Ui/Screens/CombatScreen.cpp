#include "Pets/Ui/Screens/CombatScreen.h"
#include "Pets/Ui/Ui.h"
#include "Pets/Combat/HuntTypes.h"
#include "Pets/Combat/HuntEncounter.h"

#include <Combat/CombatSchedule.h>
#include <DesignPatterns/ServiceLocator.h>
#include <Manage/TickManager.h>
#include <Ui/UiUtil.h>
#include <Utilities/Handle.h>

#include <memory>

namespace {
    std::unique_ptr<Pets::HuntEncounter> Encounter{nullptr};
    std::vector<ScopedHandle> Subs{};
}
namespace Pets::Ui::Screens::Combat {
	bool Initialize() { 
		auto& services = ServiceLocator::Get();
		auto& Inv = services.GetRequired<Pets::Inventory>();
        auto rules = HuntRules{Inv};
        auto schedule = std::make_unique<::Combat::RealTimeScheduler>();
        Encounter = std::make_unique<HuntEncounter>(rules, std::move(schedule));

		TickManager::Get().Register(Subs, [](BaseTime elapsed) {
			if (Encounter) {
                Encounter->Update(elapsed);
			}
	    });
		return true; 
	}

	void ShutDown() {
        Subs.clear();
        Encounter.reset();
	}

	void Render() {
		TextCenteredX("Combat");
	}
} // namespace Pets::Ui::Screens::Combat
