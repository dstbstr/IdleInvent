#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/GameState/GameTime.h"

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
				RegisterEvent(Misinformation, Rarity::Regular);
				RegisterEvent(Censorship, Rarity::Rare);
				RegisterEvent(BookBurnings, Rarity::VeryRare);

				RegisterEvent(BestSeller, Rarity::Common);
				RegisterEvent(ResearchGrant, Rarity::Rare);
				RegisterEvent(Discovery, Rarity::VeryRare);

				RegisterEvent(Conference, Rarity::VeryRare);
				RegisterEvent(MadScientist, Rarity::Legendary);
				RegisterEvent(FringeSect, Rarity::Mythic);
			}
		}
	}
}
