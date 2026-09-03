#include "Pets/Combat/PartyProvider.h"

namespace Pets::PartyProvider {
	HuntCombatant GetParty() {
        return HuntCombatant{
			.ActionInterval = OneSecond, 
			.Stats = PartyStats{
				.Attack = 10, 
				.Piercing = 5
			}
		};
	}
}