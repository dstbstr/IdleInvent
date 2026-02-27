#include "Mechanics/Advancement.h"

#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"

void Advancement::Tick(BaseTime elapsed) {
    if(CurrentLevel == MaxLevel) return;

	CurrentExp += static_cast<u64>(Progress.GetProgress(elapsed));
	if(CurrentExp < ExpToNextLevel) return;

	auto& ps = ServiceLocator::Get().GetRequired<PubSub<Advancement>>();
	while (CurrentExp >= ExpToNextLevel) {
		CurrentExp -= ExpToNextLevel;
		ExpToNextLevel = NextLevelCost(++CurrentLevel) + InitialCost;
		ps.Publish(*this);
	}
}
