#include "InventLib/RandomEvents/RandomEvents.h"

#include <GameState/GameTime.h>
#include <Mechanics/RandomEvent.h>

namespace Invent {
	namespace RandomEvents {
		namespace LaborEvents {
			// disasters
			const RandomEvent Breakdown;
			const RandomEvent Thieves;
			const RandomEvent Strike;

			// positive events
			const RandomEvent TwoForOne;
			const RandomEvent MoraleBoost;
			const RandomEvent Overtime;

			// mixed events
			const RandomEvent NewHire;
			const RandomEvent LatestTrend;
			const RandomEvent Merger;

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
