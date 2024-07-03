#include "InventLib/Mechanics/Advancement.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"

namespace Invent {
	void Advancement::Tick(BaseTime elapsed) {
        if(CurrentLevel == MaxLevel) return;

		CurrentExp += Progress.GetProgress(elapsed);
		if(CurrentExp < ExpToNextLevel) return;

		auto& ps = ServiceLocator::Get().GetRequired<PubSub<Advancement>>();
		while (CurrentExp >= ExpToNextLevel) {
			CurrentExp -= ExpToNextLevel;
			ExpToNextLevel = NextLevelCost(++CurrentLevel) + InitialCost;
			ps.Publish(*this);
		}
	}
}