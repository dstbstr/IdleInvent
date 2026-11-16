#pragma once

#include "Iduel/IduelTypes.h"
#include "Iduel/Simulation/IStrategy.h"

namespace Iduel {
	struct ParticipantState {
		IStrategy* Strategy{};
		f64 Wallet{};
	};
}