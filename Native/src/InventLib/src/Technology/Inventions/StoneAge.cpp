#include "InventLib/Technology/Technology.h"

#include "InventLib/Technology/Invention.h"
#include "InventLib/Mechanics/Purchasable.h"
#include "InventLib/Mechanics/Unlockable.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/RandomEvents/RandomEvents.h"

#include <ranges>

namespace {
	using namespace Invent;

	Invention CreateInvention(ResourceName resource, const std::string& name, const std::vector<std::string>& descriptions);
	void ClearStoneAge();

	std::vector<Invention> AllInventions{{
		CreateInvention(ResourceName::Influence, "Tribes", {
			"I found that not killing people has benefits too.",
			"I think I just invented 'friendship', let's see where it goes.",
			"If we all wear the same thing, we can see friends from far away!"
		}),
		CreateInvention(ResourceName::Knowledge, "Oral Tradition", {
			"What do you mean 'what happened yesterday?'",
			"I saw a bear over there a couple days ago.",
			"Did I ever tell you about what my grandpa saw?"
		}),
		CreateInvention(ResourceName::Labor, "StoneTools", {
			"These things are heavy, I bet they squish things good!",
			"When I smash these things together, I get pointy things!",
			"Sticks and stones CAN break bones!"
		}),
		CreateInvention(ResourceName::Magic, "Totems", {
			"That's my lucky rock, you can't have it.",
			"This stick reminds me of mother, I'll keep it.",
			"I've found that the bugs don't eat me as much when I wear this.",
		}),
		CreateInvention(ResourceName::Wealth, "Hunting and Gathering", {
			"I found a dead deer, turns out it's edible.",
			"Ya know, berries don't run nearly as fast as deer do.",
			"I think I'm getting the hang of this 'not starving' thing."
		})
	}};

	std::vector<Effect> GetEffects(size_t level, ResourceName resource) {
		using namespace Effects;

		auto r = GetRelativeResources(resource);
		switch (level) {
		case 0:
			return {
				{CreatePermanent(r[0], EffectTarget::Resources, Buff::Add::Small)}
			};
		case 1:
			return {
				{CreatePermanent(r[0], EffectTarget::Resources, Buff::Add::Medium)},
				{CreatePermanent(r[1], EffectTarget::Resources, Buff::Add::Small)},
				{CreatePermanent(r[2], EffectTarget::Resources, Buff::Add::Small)}
			};
		case 2:
			return {
				{CreatePermanent(r[0], EffectTarget::Resources, Buff::Mul::Large)},
				{CreatePermanent(r[1], EffectTarget::Resources, Buff::Mul::Medium)},
				{CreatePermanent(r[2], EffectTarget::Resources, Buff::Mul::Small)}
			};
		default:
			return {};
		}
	}

	std::vector<InventionLevel> MakeLevels(ResourceName resource, std::string name, std::vector<std::string> descriptions) {
		std::vector<InventionLevel> result{};
		auto OnPurchase = [resource]() {
			ClearStoneAge();
			switch (resource) {
			case ResourceName::Labor:
				RandomEvents::LaborEvents::RegisterAll();
				break;
			case ResourceName::Wealth:
				RandomEvents::WealthEvents::RegisterAll();
				break;
			case ResourceName::Influence:
				RandomEvents::InfluenceEvents::RegisterAll();
				break;
			case ResourceName::Knowledge:
				RandomEvents::KnowledgeEvents::RegisterAll();
				break;
			case ResourceName::Magic:
				RandomEvents::MagicEvents::RegisterAll();
				break;
			case ResourceName::Unset:
				throw "Wat?";
			}
		};

		result.push_back(InventionLevel{ resource, 0, 0, name, descriptions[0], GetEffects(0, resource), {}, OnPurchase });

		for (const auto& description : descriptions | std::views::drop(1)) {
			result.push_back(InventionLevel{ resource, 0ull, result.size(), name, description, GetEffects(result.size(), resource) });
		}
		return result;
	}

	Invention CreateInvention(ResourceName resource, const std::string& name, const std::vector<std::string>& descriptions) {
		return Invention{
			.Name = name,
			.Levels = MakeLevels(resource, name, descriptions)
		};
	}

	void ClearStoneAge() {
		auto& services = ServiceLocator::Get();
		auto& purchasables = services.GetRequired<std::unordered_map<std::string, Purchasable>>();
		auto& unlockables = services.GetRequired<std::unordered_map<std::string, Unlockable>>();
		for (const auto& invention : AllInventions) {
			purchasables.erase(invention.Levels[0].Name);
			unlockables.erase(invention.Levels[0].Name);
		}
	}
}

namespace Invent {
	namespace Technologies {
		std::vector<Unlockable> StoneAgeTechs = AllInventions |
			std::ranges::views::transform([](const Invention& i) { return i.MakeUnlockable(0); }) |
			std::ranges::to<std::vector<Unlockable>>();
	}
}
