#include "InventLib/Achievements/Achievements.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/GameState/GameTime.h"
#include "InventLib/Mechanics/Effect.h"

#include "Core/DesignPatterns/PubSub.h"
#include "Core/DesignPatterns/ServiceLocator.h"

#include <vector>
#include <optional>

namespace {
	using namespace Invent;

	BaseTime GetCurrentTime() {
		const auto& gameState = ServiceLocator::Get().GetRequired<Invent::GameState>();
		return gameState.TimeElapsed;
	}

	std::vector<Effect> Effects = []() -> std::vector<Effect> {
        return {{.Target = EffectTarget::AllSecondaryRate, .Modifier = Effects::Increase::Add::Small}};
    }();

	Unlockable MakeUnlockable(const std::string& name, size_t current, BaseTime requiredTime) {
        return Unlockable(
            name,
            [requiredTime]() { return GetCurrentTime() >= requiredTime; },
            [current]() -> std::vector<Unlockable> {
                auto& services = ServiceLocator::Get();
                services.GetRequired<PubSub<Achievement>>().Publish(Achievements::Time[current]);
                services.GetRequired<PubSub<std::vector<Effect>>>().Publish(Effects);
                return {};
            }
        );
	}

	Achievement MakeAchievement(const std::string& name, const std::string& description, BaseTime requiredTime) {
        static size_t achievementIndex = 0;
		auto unlock = MakeUnlockable(name, achievementIndex++, requiredTime);

		return Achievement{ .Name = name, .Description = description, .Unlock = unlock };
	}
}

namespace Invent {
	namespace Achievements {
        std::vector<Achievement> Time{
			MakeAchievement("First Minute", "How have you survived an entire minute?  Have this achievement.", OneMinute),
			MakeAchievement("Ten Minutes", "Certainly you have something better to do with your time.", OneMinute * 10),
			MakeAchievement("One Hour", "Hate to be the bearer of bad news, but you've wasted an hour of your life here.  You'll never get it back.", OneHour),
			MakeAchievement("Twelve Hours", "You've spent half a day here.  You should probably go outside.", OneHour * 12),
			MakeAchievement("One Day", "24 hours gone.  Think of what you could have accomplished.", OneDay),
			MakeAchievement("One Week", "Playing games for 7 days makes one WEEK!  Hah, I crack myself up.", OneDay * 7),
			MakeAchievement("One Month", "I think you've been here longer than I spent working on this game...", OneDay * 30),
			MakeAchievement("Three Months", "You've been here for 3 months.  You should write your own game if you haven't yet.", OneDay * 90)
		};
	}
}