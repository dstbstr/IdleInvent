#pragma once

#include "Mechanics/Effect.h"
#include "GameState/GameTime.h"

#include <Platform/NumTypes.h>

#include <string>
#include <vector>
#include <unordered_map>

struct RandomEvent {
	std::string Headline{};
	std::string Description{};
	std::vector<Effect> Effects{};

	constexpr bool operator==(const RandomEvent& other) const {
		return Headline == other.Headline;
	}
};

namespace RandomEvents {
	void Tick(BaseTime elapsed);

	bool IsEventRegistered(const RandomEvent& randomEvent);
	void RegisterEvent(const RandomEvent& event, BaseTime chance);
	void UnregisterEvent(const RandomEvent& event);
	BaseTime GetEventChance(const RandomEvent& randomEvent);
}

template<>
struct std::hash<RandomEvent> {
	size_t operator()(const RandomEvent& event) const {
		return std::hash<std::string>{}(event.Headline);
	}
};