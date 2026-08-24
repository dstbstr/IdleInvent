#pragma once

#include "Combat/CombatTypes.h"

#include <optional>


namespace Combat {
	template<typename TAction, typename TEncounter>
	struct ICombatController {
        virtual ~ICombatController() = default;

		virtual std::optional<TAction> GetNextAction(CombatantId actor, const TEncounter& encounter) = 0;
	};
}