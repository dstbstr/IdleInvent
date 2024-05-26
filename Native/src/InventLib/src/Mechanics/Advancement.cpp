#include "InventLib/Mechanics/Advancement.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"

namespace Invent {
	void Advancement::Tick() {
		CurrentExp += Progress.GetProgress();
		if(CurrentExp < ExpToNextLevel) return;

		auto& ps = ServiceLocator::Get().GetRequired<PubSub<Advancement>>();
		while (CurrentExp >= ExpToNextLevel) {
			CurrentExp -= ExpToNextLevel;
			ExpToNextLevel = NextLevelCost(++CurrentLevel);
			ps.Publish(*this);
		}
	}
}