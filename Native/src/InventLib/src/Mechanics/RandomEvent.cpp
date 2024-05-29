#include "InventLib/Mechanics/RandomEvent.h"
#include "InventLib/GameState/GameTime.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"
#include "Core/Utilities/IRandom.h"

#include <unordered_map>
#include <optional>

namespace {
	using namespace Invent;
	std::unordered_map<Invent::RandomEvent, BaseTime> EventChances{};
	// Event to [current, target]
	std::unordered_map<RandomEvent, std::pair<BaseTime, BaseTime>> EventCurrentSlot{};
	std::optional<std::pair<RandomEvent, BaseTime>> ActiveEvent;

	size_t GetRandomNumber(size_t max) {
		static auto& random = ServiceLocator::Get().GetRequired<IRandom>();
		return random.GetNext(1, max);
	}
}

namespace Invent {
	namespace RandomEvents {
        using namespace std::chrono_literals;

		void RegisterEvent(const RandomEvent& randomEvent, BaseTime chance) {
			EventChances.emplace(randomEvent, chance);
			auto triggerTime = BaseTime{GetRandomNumber(chance.count())};
            EventCurrentSlot.emplace(randomEvent, std::make_pair(0s, triggerTime));
		}

		void UnregisterEvent(const RandomEvent& randomEvent) {
			EventChances.erase(randomEvent);
			EventCurrentSlot.erase(randomEvent);
		}

		BaseTime GetEventChance(const RandomEvent& randomEvent) {
			return EventChances[randomEvent];
		}

		void Tick(BaseTime elapsed) {
			if (ActiveEvent.has_value()) {
				auto& [event, duration] = ActiveEvent.value();
				if (duration <= 0s) {
					ActiveEvent.reset();
				}
				else {
					ServiceLocator::Get().GetRequired<PubSub<RandomEvent>>().Publish(event);
					duration -= elapsed;
				}
			}
			else {
				for (auto& [event, chance] : EventCurrentSlot) {
					auto& [current, target] = chance;
					
					if(current <= target && current + elapsed >= target) {
						// TODO: This assumes that all effects have the same duration
                        ActiveEvent.emplace(event, event.Effects[0].Duration);
                    }
					else if (current >= EventChances[event]) {
						current = 0s;
                        target = BaseTime{GetRandomNumber(EventChances[event].count())};
					}
				}
			}
		}
	}
}