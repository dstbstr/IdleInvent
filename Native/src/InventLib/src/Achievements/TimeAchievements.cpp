#include "InventLib/Achievements/Achievements.h"
#include "Core/DesignPatterns/PubSub.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/GameState/GameTime.h"

#include <vector>
#include <optional>

namespace {
	using namespace Invent;

	std::vector<std::pair<std::string, size_t>> AchievementLevels {
		{"First Minute", OneMinute},
		{"Ten Minutes", OneMinute * 10},
		{"One Hour", OneHour},
		{"Twelve Hours", OneHour * 12},
		{"One Day", OneDay},
		{"One Week", OneDay * 7},
		{"One Month", OneDay * 28},
		{"Three Months", OneDay * 90}
	};

	size_t GetCurrentTime() {
		static auto& gameState = ServiceLocator::Get().GetRequired<Invent::GameState>();
		return gameState.ElapsedTicks;
	}

	Unlockable MakeUnlockable(const std::string& current, std::optional<std::string> next, size_t requiredTime) {
		return Unlockable{
			current,
			[requiredTime]() { return GetCurrentTime() >= requiredTime; },
			[current, next]() -> std::vector<Unlockable> {
				ServiceLocator::Get().GetRequired<PubSub<Achievement>>().Publish(Achievements::TimeAchievements.at(current).second);
				if (next.has_value()) {
					return { Achievements::TimeAchievements.at(next.value()).first };
				}
				return {};
			}
		};
	}

	Achievement MakeAchievement(const std::string& name, const std::string description) {
		auto MakeEffect = [](ResourceName resource) {
			return Effects::CreatePermanent(resource, EffectTarget::Resources, Effects::Buff::Mul::Small);
		};

		return Achievement{ .Name = name, .Description = description, .Effects = {
			MakeEffect(ResourceName::Influence),
			MakeEffect(ResourceName::Knowledge),
			MakeEffect(ResourceName::Wealth),
			MakeEffect(ResourceName::Labor),
			MakeEffect(ResourceName::Magic)
			}
		};
	}

	std::pair<std::string, std::pair<Unlockable, Achievement>> MakePair(size_t index, const std::string& description) {
		auto [current, requiredTime] = AchievementLevels[index];
		auto next = index + 1 < AchievementLevels.size() ? std::make_optional(AchievementLevels[index + 1].first) : std::nullopt;
		return std::make_pair(current, std::make_pair(
			MakeUnlockable(current, next, requiredTime ), 
			MakeAchievement(current, description)));
	}
}

namespace Invent {
	namespace Achievements {
		std::string FirstTimeAchievement = AchievementLevels[0].first;

		std::unordered_map<std::string, std::pair<Unlockable, Achievement>> TimeAchievements{
			MakePair(0, "How have you survived an entire minute?  Have this achievement."),
			MakePair(1, "Certainly you have something better to do with your time."),
			MakePair(2, "Hate to be the bearer of bad news, but you've wasted an hour of your life here.  You'll never get it back."),
			MakePair(3, "You've spent half a day here.  You should probably go outside."),
			MakePair(4, "24 hours gone.  Think of what you could have accomplished."),
			MakePair(5, "Playing games for 7 days makes one WEEK!  Hah, I crack myself up."),
			MakePair(6, "I think you've been here longer than I spent working on this game..."),
			MakePair(7, "You've been here for 3 months.  You should write your own game if you haven't yet.")
		};
	}
}