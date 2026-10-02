#pragma once

#include "Walker/WalkerUnits.h"
#include <Platform/NumTypes.h>

#include <optional>

namespace Walker {
	struct JobProgress {
		Work RequiredWork{};
		Work CompletedWork{};

		void Advance(Work progress);

		f32 GetProgress() const;
		bool IsComplete() const;

		std::optional<Time> GetEta(WorkRate rate) const;
	};
}