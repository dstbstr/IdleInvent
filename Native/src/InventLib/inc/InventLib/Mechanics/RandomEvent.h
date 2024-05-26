#pragma once

#include "Core/NumTypes.h"
#include "InventLib/Mechanics/Effect.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Invent
{
	struct RandomEvent {
		std::string Headline{};
		std::string Description{};
		size_t Duration{ 0 };

		std::vector<Effect> Effects{};

		constexpr bool operator==(const RandomEvent& other) const {
			return Headline == other.Headline;
		}
	};

	namespace RandomEvents {
		void Tick();

		void RegisterEvent(const RandomEvent& event, size_t chance);
		void UnregisterEvent(const RandomEvent& event);
		size_t GetEventChance(const RandomEvent& randomEvent);
	}
}

template<>
struct std::hash<Invent::RandomEvent> {
	size_t operator()(const Invent::RandomEvent& event) const {
		return std::hash<std::string>{}(event.Headline);
	}
};