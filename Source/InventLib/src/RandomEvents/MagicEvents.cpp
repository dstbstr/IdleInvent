#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/Effects/Effect.h"

#include <GameState/GameTime.h>

namespace Invent {
	namespace RandomEvents {
		namespace MagicEvents {
			// disasters
			const RandomEvent Backfire;
			const RandomEvent Curse;
			const RandomEvent Silence;

			// positive events
			const RandomEvent ComponentCache;
			const RandomEvent Metamagic;
			const RandomEvent Divination;

			// mixed events
			const RandomEvent Rift;
			const RandomEvent Necromancer;
			const RandomEvent TooStrong;

			void RegisterAll() {
				//TODO: Yuck
				::RandomEvents::RegisterEvent(Backfire, Rarity::Uncommon);
				::RandomEvents::RegisterEvent(Curse, Rarity::Rare);
				::RandomEvents::RegisterEvent(Silence, Rarity::VeryRare);

				::RandomEvents::RegisterEvent(ComponentCache, Rarity::Common);
				::RandomEvents::RegisterEvent(Metamagic, Rarity::Uncommon);
				::RandomEvents::RegisterEvent(Divination, Rarity::Rare);

				::RandomEvents::RegisterEvent(Rift, Rarity::VeryRare);
				::RandomEvents::RegisterEvent(Necromancer, Rarity::Legendary);
				::RandomEvents::RegisterEvent(TooStrong, Rarity::Mythic);
			}
		}
	}
}
