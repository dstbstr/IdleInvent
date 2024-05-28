#include "InventLib/Resources/Storage.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"
#include "InventLib/GameState/GameState.h"

namespace Invent {
	bool Storage::CanLevelUp() const {
        if(Advance.CurrentLevel < CurrentLevel) return false;

		const auto& gameState = ServiceLocator::Get().GetRequired<GameState>();

		return ResourcesToLevel <= gameState.CurrentResources;
	}

	void Storage::LevelUp() {
		auto& gameState = ServiceLocator::Get().GetRequired<GameState>();

		gameState.CurrentResources -= ResourcesToLevel;
        CurrentLevel++;
        ResourcesToLevel = GetResourceCosts(ResourceType, CurrentLevel);

		ServiceLocator::Get().GetRequired<PubSub<Storage>>().Publish(*this);
	}

	void Storage::Tick(BaseTime elapsed) {
        Advance.Tick(elapsed);
	}	
}