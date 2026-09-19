#pragma once

#include "Walker/WalkerUnits.h"

#include <optional>

namespace Walker {
	struct CargoTransfer {
        Mass Target{};
        Mass Transferred{};
        Work AccumulatedWork{};

        Mass Advance(Work addedWork, Work workPerKg, Mass available);

        f32 GetProgress() const;

        std::optional<Time> GetEta(WorkRate rate, Work workPerKg) const;
	};
}