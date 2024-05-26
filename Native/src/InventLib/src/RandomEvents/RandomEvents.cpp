#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/Mechanics/RandomEvent.h"
#include "InventLib/GameState/GameTime.h"

namespace {
	using namespace Invent::RandomEvents;

	size_t MoreCommon(size_t chance) {
		switch (chance) {
		case Rarity::Common: return Rarity::Regular;
		case Rarity::Uncommon: return Rarity::Common;
		case Rarity::Rare: return Rarity::Uncommon;
		case Rarity::VeryRare: return Rarity::Rare;
		case Rarity::Legendary: return Rarity::VeryRare;
		case Rarity::Mythic: return Rarity::Legendary;
		default: return chance / 2;
		}
	}

	size_t LessCommon(size_t chance) {
		switch (chance) {
		case Rarity::Regular: return Rarity::Common;
		case Rarity::Common: return Rarity::Uncommon;
		case Rarity::Uncommon: return Rarity::Rare;
		case Rarity::Rare: return Rarity::VeryRare;
		case Rarity::VeryRare: return Rarity::Legendary;
		case Rarity::Legendary: return Rarity::Mythic;
		default: return chance * 2;
		}
	}
}

namespace Invent {
	namespace RandomEvents {
		void Initialize() {
			NaturalEvents::RegisterAll();
		}

		void MakeMoreCommon(RandomEvent event) {
			auto rarity = GetEventChance(event);
			UnregisterEvent(event);
			RegisterEvent(event, MoreCommon(rarity));
		}

		void MakeLessCommon(RandomEvent event) {
			auto rarity = GetEventChance(event);
			UnregisterEvent(event);
			RegisterEvent(event, LessCommon(rarity));
		}
	}
}