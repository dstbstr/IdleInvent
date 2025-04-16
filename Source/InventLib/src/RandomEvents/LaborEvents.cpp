#include "InventLib/RandomEvents/RandomEvents.h"

#include <GameState/GameTime.h>
#include <Mechanics/RandomEvent.h>

namespace Invent {
	namespace RandomEvents {
		namespace LaborEvents {
			// disasters
			RandomEvent Breakdown;
			RandomEvent Thieves;
			RandomEvent Strike;

			// positive events
			RandomEvent TwoForOne;
			RandomEvent MoraleBoost;
			RandomEvent Overtime;

			// mixed events
			RandomEvent NewHire;
			RandomEvent LatestTrend;
			RandomEvent Merger;

			void RegisterAll() {
				::RandomEvents::RegisterEvent(Breakdown, Rarity::Common);
				::RandomEvents::RegisterEvent(Thieves, Rarity::Uncommon);
				::RandomEvents::RegisterEvent(Strike, Rarity::Rare);

				::RandomEvents::RegisterEvent(TwoForOne, Rarity::Regular);
				::RandomEvents::RegisterEvent(MoraleBoost, Rarity::Uncommon);
				::RandomEvents::RegisterEvent(Overtime, Rarity::Rare);

				::RandomEvents::RegisterEvent(NewHire, Rarity::VeryRare);
				::RandomEvents::RegisterEvent(LatestTrend, Rarity::Legendary);
				::RandomEvents::RegisterEvent(Merger, Rarity::Mythic);
			}
		}
	}
}
