#pragma once

#include "Core/NumTypes.h"
#include "InventLib/Mechanics/Progression.h"

#include <string>
#include <functional>
#include <memory>
#include <chrono>

namespace Invent {
	namespace AdvancementCosts {
		template<size_t Log>
		size_t Logarithmic(size_t currentLevel) {
			return static_cast<size_t>(std::pow(Log, currentLevel));
		}

		template<size_t Value>
		size_t Constant(size_t) {
			return Value;
		}

		template<size_t Mult>
		size_t Linear(size_t currentLevel) {
			return currentLevel * Mult;
		}

		template<size_t Step, typename StepFn>
		auto MakePunctuated(StepFn stepFn) {
			return [stepFn](size_t currentLevel) {
				return stepFn((currentLevel / Step) + 1);
			};
		}	
	}
	struct Advancement {
		std::string Name{};
		size_t CurrentLevel{ 1 };
		size_t CurrentExp{ 0 };
		std::function<size_t(size_t)> NextLevelCost = AdvancementCosts::Logarithmic<2>;
		size_t ExpToNextLevel{ 0 };
		Progression Progress{};

		Advancement(const std::string& name, std::function<size_t(size_t)> levelCosts, Progression progress = {}) 
			: Name(name)
			, NextLevelCost(levelCosts)
			, ExpToNextLevel(levelCosts(1))
			, Progress(progress)
		{
			ExpToNextLevel = NextLevelCost(CurrentLevel);
		}

		void Tick(BaseTime elapsed);
	};
}