#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/Effects/Effect.h"

#include <GameState/GameTime.h>

namespace Invent {
	namespace RandomEvents {
		namespace WealthEvents {
			// disasters
			const RandomEvent Robbery;
			const RandomEvent Bankruptcy;
			const RandomEvent HyperInflation;

			// positive events
			const RandomEvent NewInvestor;
			const RandomEvent Inheritance;
			const RandomEvent Lottery;

			// mixed events
			const RandomEvent Sponsorship;
			const RandomEvent LongShot;
			const RandomEvent PoliticalUnrest;

			void RegisterAll() {
				::RandomEvents::RegisterEvent(Robbery, Rarity::Common);
				::RandomEvents::RegisterEvent(Bankruptcy, Rarity::Rare);
				::RandomEvents::RegisterEvent(HyperInflation, Rarity::Legendary);

				::RandomEvents::RegisterEvent(NewInvestor, Rarity::Uncommon);
				::RandomEvents::RegisterEvent(Inheritance, Rarity::Rare);
				::RandomEvents::RegisterEvent(Lottery, Rarity::VeryRare);

				::RandomEvents::RegisterEvent(Sponsorship, Rarity::VeryRare);
				::RandomEvents::RegisterEvent(LongShot, Rarity::Legendary);
				::RandomEvents::RegisterEvent(PoliticalUnrest, Rarity::Mythic);
			}
		}
	}
}
