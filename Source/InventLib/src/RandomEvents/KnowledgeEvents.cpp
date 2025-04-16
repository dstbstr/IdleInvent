#include "InventLib/RandomEvents/RandomEvents.h"

#include <Mechanics/RandomEvent.h>
#include <GameState/GameTime.h>

namespace Invent {
	namespace RandomEvents {
		namespace KnowledgeEvents {
			// disasters
			RandomEvent Misinformation;
			RandomEvent Censorship;
			RandomEvent BookBurnings;

			// positive events
			RandomEvent BestSeller;
			RandomEvent ResearchGrant;
			RandomEvent Discovery;

			// mixed events
			RandomEvent Conference;
			RandomEvent MadScientist;
			RandomEvent FringeSect;

			void RegisterAll() {
				::RandomEvents::RegisterEvent(Misinformation, Rarity::Regular);
				::RandomEvents::RegisterEvent(Censorship, Rarity::Rare);
				::RandomEvents::RegisterEvent(BookBurnings, Rarity::VeryRare);

				::RandomEvents::RegisterEvent(BestSeller, Rarity::Common);
				::RandomEvents::RegisterEvent(ResearchGrant, Rarity::Rare);
				::RandomEvents::RegisterEvent(Discovery, Rarity::VeryRare);

				::RandomEvents::RegisterEvent(Conference, Rarity::VeryRare);
				::RandomEvents::RegisterEvent(MadScientist, Rarity::Legendary);
				::RandomEvents::RegisterEvent(FringeSect, Rarity::Mythic);
			}
		}
	}
}
