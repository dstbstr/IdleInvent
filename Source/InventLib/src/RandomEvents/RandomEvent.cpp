#include "InventLib/RandomEvents/RandomEvent.h"
#include "GameState/GameTime.h"

#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"
#include "Utilities/IRandom.h"

#include <unordered_map>
#include <optional>

namespace {
	std::unordered_map<RandomEvent, BaseTime> EventChances{};
	// Event to [current, target]
	std::unordered_map<RandomEvent, std::pair<BaseTime, BaseTime>> EventCurrentSlot{};
	std::optional<RandomEvent> ActiveEvent;

	size_t GetRandomNumber(size_t max) {
		auto& random = ServiceLocator::Get().GetRequired<IRandom>();
		return random.GetNext(1, max);
	}
}

namespace RandomEvents {
    using namespace std::chrono_literals;

	void RegisterEvent(const RandomEvent& randomEvent, BaseTime chance) {
		EventChances.emplace(randomEvent, chance);
		auto triggerTime = BaseTime{GetRandomNumber(chance.count())};
        EventCurrentSlot.emplace(randomEvent, std::make_pair(0s, triggerTime));

		Log::Info(std::format("Registered random event: {} which will trigger in {} seconds", randomEvent.Headline, triggerTime.count() / OneSecond.count()));
	}

	void UnregisterEvent(const RandomEvent& randomEvent) {
		EventChances.erase(randomEvent);
		EventCurrentSlot.erase(randomEvent);
	}

	bool IsEventRegistered(const RandomEvent& randomEvent) {
		return EventChances.contains(randomEvent);
	}

	BaseTime GetEventChance(const RandomEvent& randomEvent) {
		return EventChances.at(randomEvent);
	}

	void Tick(BaseTime elapsed) {
		if (ActiveEvent.has_value()) {
            ServiceLocator::Get().GetRequired<PubSub<RandomEvent>>().Publish(ActiveEvent.value());
			ServiceLocator::Get().GetRequired<PubSub<std::vector<Effect>>>().Publish(ActiveEvent.value().Effects);
			ActiveEvent.reset();
		}
		else {
			for (auto& [event, chance] : EventCurrentSlot) {
				auto& [current, target] = chance;
					
				if(current <= target && current + elapsed >= target) {
                    ActiveEvent = event;
					//Log::Info(std::format("Triggering random event: {} for {} seconds", event.Headline, std::chrono::duration_cast<std::chrono::seconds>(event.Effects[0].Duration).count()));
                }
				else if (current >= EventChances[event]) {
					current = 0s;
                    target = BaseTime{GetRandomNumber(EventChances[event].count())};
                    Log::Info(std::format("Reseting random event: {} which will trigger in {} seconds", event.Headline, target.count() / OneSecond.count()));
				}

				current += elapsed;
			}
		}
	}
}
