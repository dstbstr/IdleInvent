#include "CommonTest.h"

#include "InventLib/InventGame.h"

#include <DesignPatterns/ServiceLocator.h>
#include <DesignPatterns/PubSub.h>
#include <GameState/GameState.h>
#include <Mechanics/Unlockable.h>
#include <Mechanics/Achievement.h>

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