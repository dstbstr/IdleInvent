#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/GameState/GameTime.h"

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
				RegisterEvent(Backfire, Rarity::Uncommon);
				RegisterEvent(Curse, Rarity::Rare);
				RegisterEvent(Silence, Rarity::VeryRare);

				RegisterEvent(ComponentCache, Rarity::Common);
				RegisterEvent(Metamagic, Rarity::Uncommon);
				RegisterEvent(Divination, Rarity::Rare);

				RegisterEvent(Rift, Rarity::VeryRare);
				RegisterEvent(Necromancer, Rarity::Legendary);
				RegisterEvent(TooStrong, Rarity::Mythic);
			}
		}
	}
}
