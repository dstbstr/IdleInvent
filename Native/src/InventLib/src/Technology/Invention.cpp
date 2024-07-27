#include "InventLib/Technology/Invention.h"
#include "InventLib/Mechanics/Unlockable.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/Resources/Resource.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"
#include "Core/Constexpr/ConstexprStrUtils.h"

#include <array>
#include <unordered_set>

namespace {
    constexpr const std::array LevelSuffixes = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"};

	std::unordered_set<std::string> PurchasedTechs{};
	//auto ResourceSpecialty = Invent::ResourceName::Unset;
}


namespace Invent {
	InventionLevel::InventionLevel(ResourceName resource, size_t age, size_t level, std::string name, std::string description, std::vector<std::string> prereqs, const std::vector<Effect>& effects, std::function<void()> extraOnUnlock, std::function<void()> extraOnPurchase)
		: Name(name + " " + LevelSuffixes[level])
        , Resource(resource)
		, Age(age)
		, Level(level)
		, Description(description)
		, Prerequisites(prereqs)
		//, PurchaseResourceCosts(GetResourceCosts(resource, age))
		, UnlockResourceCosts(PurchaseResourceCosts / 2.0F)
		, Effects(effects) 
		, ExtraOnUnlock(extraOnUnlock)
		, ExtraOnPurchase(extraOnPurchase)
	{
        if(level > 0) {
			Prerequisites.push_back(name + " " + LevelSuffixes[level - 1]);
		}
	}

	/*
	Unlockable Invention::MakeUnlockable(size_t level) const {
		Log::Debug(std::format("Making unlockable for {} level {}", Name, level));

		const auto& currentLevel = Levels[level];

		auto CanUnlock = [currentLevel]() {
			if(currentLevel.Age > 0  && currentLevel.Resource != ResourceSpecialty) {
                return false;
            }

			for(const auto& prereq: currentLevel.Prerequisites) {
				if (!PurchasedTechs.contains(prereq)) {
					return false;
				}
			}

            auto& gameState = ServiceLocator::Get().GetRequired<GameState>();

			return currentLevel.UnlockResourceCosts <= gameState.CurrentLife.Resources;
		};

		auto OnUnlock = [this, currentLevel, level]() -> std::vector<Unlockable> {
            Log::Debug(std::format("Unlocking: {} Level: {}", currentLevel.Name, level));
			auto& purchasables = ServiceLocator::Get().GetRequired<std::unordered_map<std::string, Purchasable>>();
			auto purchasable = MakePurchasable(level);
			purchasables.emplace(purchasable.Name, purchasable);

			if (currentLevel.ExtraOnUnlock) currentLevel.ExtraOnUnlock();

			if (level < Levels.size() - 1) {
				return { MakeUnlockable(level + 1) };
			}
			else {
				return {};
			}
		};

		return Unlockable(currentLevel.Name, CanUnlock, OnUnlock);
	}

	Purchasable Invention::MakePurchasable(size_t level) const {
		Log::Debug(std::format("Making purchasable for {} level {}", Name, level));

		const auto& currentLevel = Levels[level];

		auto OnPurchase = [currentLevel]() {
            Log::Info(std::format("Purchasing {}", currentLevel.Name));
			auto& ps = ServiceLocator::Get().GetRequired<PubSub<InventionLevel>>();
			ps.Publish(currentLevel);
            ResourceSpecialty = currentLevel.Resource;
			PurchasedTechs.insert(currentLevel.Name);

			if (currentLevel.ExtraOnPurchase) currentLevel.ExtraOnPurchase();
		};

		std::stringstream costDesc;
		if (!currentLevel.Prerequisites.empty()) {
            costDesc << "Purchases: ";
            costDesc << Constexpr::Join(", ", currentLevel.Prerequisites);
            costDesc << " ";
		}

		costDesc << currentLevel.PurchaseResourceCosts.Describe();

		return Purchasable(currentLevel.Name, currentLevel.Description, costDesc.str(), currentLevel.PurchaseResourceCosts, OnPurchase);
	}

	namespace Inventions {
        void Reset() {
            PurchasedTechs.clear();
            ResourceSpecialty = ResourceName::Unset;
        }
    } // namespace Inventions
	*/
}