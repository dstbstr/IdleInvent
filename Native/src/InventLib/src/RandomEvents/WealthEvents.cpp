#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/GameState/GameTime.h"

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
				RegisterEvent(Robbery, Rarity::Common);
				RegisterEvent(Bankruptcy, Rarity::Rare);
				RegisterEvent(HyperInflation, Rarity::Legendary);

				RegisterEvent(NewInvestor, Rarity::Uncommon);
				RegisterEvent(Inheritance, Rarity::Rare);
				RegisterEvent(Lottery, Rarity::VeryRare);

				RegisterEvent(Sponsorship, Rarity::VeryRare);
				RegisterEvent(LongShot, Rarity::Legendary);
				RegisterEvent(PoliticalUnrest, Rarity::Mythic);
			}
		}
	}
}
