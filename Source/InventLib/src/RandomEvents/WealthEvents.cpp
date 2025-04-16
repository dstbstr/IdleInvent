#include "InventLib/RandomEvents/RandomEvents.h"

#include <GameState/GameTime.h>
#include <Mechanics/RandomEvent.h>

namespace Invent {
	namespace RandomEvents {
		namespace WealthEvents {
			// disasters
			RandomEvent Robbery;
			RandomEvent Bankruptcy;
			RandomEvent HyperInflation;

			// positive events
			RandomEvent NewInvestor;
			RandomEvent Inheritance;
			RandomEvent Lottery;

			// mixed events
			RandomEvent Sponsorship;
			RandomEvent LongShot;
			RandomEvent PoliticalUnrest;

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
