#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/GameState/GameTime.h"

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
				RegisterEvent(Breakdown, Rarity::Common);
				RegisterEvent(Thieves, Rarity::Uncommon);
				RegisterEvent(Strike, Rarity::Rare);

				RegisterEvent(TwoForOne, Rarity::Regular);
				RegisterEvent(MoraleBoost, Rarity::Uncommon);
				RegisterEvent(Overtime, Rarity::Rare);

				RegisterEvent(NewHire, Rarity::VeryRare);
				RegisterEvent(LatestTrend, Rarity::Legendary);
				RegisterEvent(Merger, Rarity::Mythic);
			}
		}
	}
}
