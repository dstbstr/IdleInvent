#pragma once

#include "GameState/GameTime.h"
#include "Mechanics/Progression.h"
#include "Mechanics/ScaleFunc.h"

#include <Platform/NumTypes.h>

#include <string>
#include <functional>
#include <memory>
#include <chrono>

struct Advancement {
	std::string Name{};
    u64 InitialCost{0};
	u64 CurrentLevel{ 1 };
    u64 MaxLevel{100};

	u64 CurrentExp{ 0 };
	std::function<u64(u64)> NextLevelCost = Scale::Logarithmic<u64, 2>;
	u64 ExpToNextLevel{ 0 };
	Progression Progress{};

	Advancement(const std::string& name, u64 maxLevel, std::function<u64(u64)> levelCosts, u64 initialLevelCost, Progression progress = {}) 
		: Name(name)
		, InitialCost(initialLevelCost)
		, MaxLevel(maxLevel)
		, NextLevelCost(levelCosts)
		, ExpToNextLevel(initialLevelCost + levelCosts(1))
		, Progress(progress)
	{
	}

	void Tick(BaseTime elapsed);
};
