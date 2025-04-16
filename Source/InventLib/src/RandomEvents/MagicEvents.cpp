#include "InventLib/RandomEvents/RandomEvents.h"

#include <GameState/GameTime.h>
#include <Mechanics/RandomEvent.h>

namespace Invent {
	namespace RandomEvents {
		namespace MagicEvents {
			// disasters
			RandomEvent Backfire;
			RandomEvent Curse;
			RandomEvent Silence;

			// positive events
			RandomEvent ComponentCache;
			RandomEvent Metamagic;
			RandomEvent Divination;

			// mixed events
			RandomEvent Rift;
			RandomEvent Necromancer;
			RandomEvent TooStrong;

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
