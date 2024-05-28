#include "InventLib/EntryPoint.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"
#include "Core/Instrumentation/Logging.h"
#include "Core/Utilities/IRandom.h"

#include "InventLib/Achievements/Achievements.h"

#include "InventLib/GameState/GameState.h"
#include "InventLib/GameState/GameTime.h"

#include "InventLib/Resources/Resource.h"

#include "InventLib/Mechanics/Achievement.h"
#include "InventLib/Mechanics/Unlockable.h"

#include "InventLib/Technology/Technology.h"
#include "InventLib/Technology/Invention.h"

#include <vector>
#include <unordered_map>
#include <string>

#include <ranges>

namespace {
	Invent::GameState* gameState{nullptr};
}

namespace Invent::EntryPoint {
	void Initialize() {
		auto& services = ServiceLocator::Get();

		services.SetThisAsThat<DefaultRandom, IRandom>();
		gameState = &services.GetOrCreate<GameState>();
		services.CreateIfMissing<std::unordered_map<std::string, Unlockable>>();
		services.CreateIfMissing<std::unordered_map<std::string, Purchasable>>();
		services.CreateIfMissing<PubSub<Unlockable>>();
		services.CreateIfMissing<PubSub<Storage>>();
		services.CreateIfMissing<PubSub<InventionLevel>>();
		services.CreateIfMissing<PubSub<Achievement>>();
		services.CreateIfMissing<PubSub<Advancement>>();

		Achievements::Initialize();
		Technologies::Initialize();
	}

	void Tick(BaseTime elapsed) {
		Unlockables::Tick();
		Purchasables::Tick();

		gameState->Tick(elapsed);
	}
}