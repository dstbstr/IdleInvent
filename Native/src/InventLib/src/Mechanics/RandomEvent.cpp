#include "InventLib/Mechanics/RandomEvent.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"
#include "Core/Utilities/IRandom.h"

#include <unordered_map>
#include <optional>

namespace {
	std::unordered_map<Invent::RandomEvent, size_t> EventChances{};
	// Event to [current, target]
	std::unordered_map<Invent::RandomEvent, std::pair<size_t, size_t>> EventCurrentSlot{};
	std::optional<std::pair<Invent::RandomEvent, size_t>> ActiveEvent;

	size_t GetRandomNumber(size_t max) {
		static auto& random = ServiceLocator::Get().GetRequired<IRandom>();
		return random.GetNext(1, max);
	}
}

namespace Invent {
	namespace RandomEvents {
		void RegisterEvent(const RandomEvent& randomEvent, size_t chance) {
			EventChances.emplace(randomEvent, chance);
			EventCurrentSlot.emplace(randomEvent, std::make_pair(0, GetRandomNumber(chance)));
		}

		void UnregisterEvent(const RandomEvent& randomEvent) {
			EventChances.erase(randomEvent);
			EventCurrentSlot.erase(randomEvent);
		}

		size_t GetEventChance(const RandomEvent& randomEvent) {
			return EventChances[randomEvent];
		}

		void Tick() {
			if (ActiveEvent.has_value()) {
				auto& [event, duration] = ActiveEvent.value();
				if (duration == 0) {
					ActiveEvent.reset();
				}
				else {
					ServiceLocator::Get().GetRequired<PubSub<RandomEvent>>().Publish(event);
					duration--;
				}
			}
			else {
				for (auto& [event, chance] : EventCurrentSlot) {
					auto& [current, target] = chance;
					current++;
					if (current == target) {
						ActiveEvent.emplace(event, event.Duration);
					}
					else if (current == EventChances[event]) {
						current = 0;
						target = GetRandomNumber(EventChances[event]);
					}
				}
			}
		}
	}
}