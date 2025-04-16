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
    size_t InitialCost{0};
	size_t CurrentLevel{ 1 };
    size_t MaxLevel{100};

	size_t CurrentExp{ 0 };
	std::function<size_t(size_t)> NextLevelCost = Scale::Logarithmic<size_t, 2>;
	size_t ExpToNextLevel{ 0 };
	Progression Progress{};

	Advancement(const std::string& name, size_t maxLevel, std::function<size_t(size_t)> levelCosts, size_t initialLevelCost, Progression progress = {}) 
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
