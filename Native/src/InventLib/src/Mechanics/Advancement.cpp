#include "InventLib/Mechanics/Advancement.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"

namespace Invent {
	void Advancement::Tick(std::chrono::milliseconds elapsed) {
		CurrentExp += Progress.GetProgress(elapsed);
		if(CurrentExp < ExpToNextLevel) return;

		auto& ps = ServiceLocator::Get().GetRequired<PubSub<Advancement>>();
		while (CurrentExp >= ExpToNextLevel) {
			CurrentExp -= ExpToNextLevel;
			ExpToNextLevel = NextLevelCost(++CurrentLevel);
			ps.Publish(*this);
		}
	}
}