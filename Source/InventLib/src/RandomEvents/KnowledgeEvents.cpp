#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/RandomEvents/RandomEvent.h"
#include <GameState/GameTime.h>

namespace Invent {
	namespace RandomEvents {
		namespace KnowledgeEvents {
			// disasters
			const RandomEvent Misinformation;
			const RandomEvent Censorship;
			const RandomEvent BookBurnings;

			// positive events
			const RandomEvent BestSeller;
			const RandomEvent ResearchGrant;
			const RandomEvent Discovery;

			// mixed events
			const RandomEvent Conference;
			const RandomEvent MadScientist;
			const RandomEvent FringeSect;

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
