#include "CommonTest.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"
#include "InventLib/EntryPoint.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/Mechanics/Unlockable.h"
#include "InventLib/Mechanics/Achievement.h"

#include <unordered_map>
#include <string>

namespace Invent {
	/*
	TEST(Game, AllTheThings) {
		auto& services = ServiceLocator::Get();
		Invent::Initialize();

		ASSERT_TRUE(services.IsSet<GameState>());
		auto hasUnlockables = services.IsSet<std::unordered_map<std::string, Unlockable>>();
		ASSERT_TRUE(hasUnlockables);
		ASSERT_TRUE(services.IsSet<PubSub<Unlockable>>());

		auto achievementCount = 0;
		services.GetRequired<PubSub<Achievement>>().Subscribe([&achievementCount](const Achievement&) {
			achievementCount++;
		});
		Invent::Tick();
		ASSERT_EQ(achievementCount, 0);
		Invent::Tick();
		ASSERT_EQ(achievementCount, 1);
		Invent::Tick();
		ASSERT_EQ(achievementCount, 2);
	}
	*/
}