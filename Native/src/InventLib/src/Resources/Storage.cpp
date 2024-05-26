#include "InventLib/Resources/Storage.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"
#include "InventLib/GameState/GameState.h"

#include <math.h>

namespace {
	// The cost of level groups goes up by a factor of 10
	// so levels  10-99 cost 10x levels 1-9
	s64 FindExpToLevel(s64 level) {
		auto base = log10(level);
		return static_cast<s64>(pow(10, base + 3));
	}
}

namespace Invent {
	bool Storage::CanLevelUp() const {
		const auto& gameState = ServiceLocator::Get().GetRequired<GameState>();

		return gameState.CurrentResources < ResourcesToLevel;
	}

	void Storage::LevelUp() {
		auto& gameState = ServiceLocator::Get().GetRequired<GameState>();

		gameState.CurrentResources -= ResourcesToLevel;

		CurrentLevel++;
		CurrentExp = 0;
		ExpToLevel = FindExpToLevel(CurrentLevel);
		ServiceLocator::Get().GetRequired<PubSub<Storage>>().Publish(*this);
	}
}