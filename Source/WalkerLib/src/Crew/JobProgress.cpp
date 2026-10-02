#include "Walker/Crew/JobProgress.h"

#include <algorithm>

namespace Walker {
	void JobProgress::Advance(Work progress) {
		CompletedWork += progress;
	}

	f32 JobProgress::GetProgress() const {
		if (RequiredWork <= Zero) return 1.f;
		return std::clamp(static_cast<f32>(Work::Ratio(CompletedWork, RequiredWork)), 0.f, 1.f);
	}

	bool JobProgress::IsComplete() const {
		return CompletedWork >= RequiredWork;
	}

	std::optional<Time> JobProgress::GetEta(WorkRate rate) const {
		using namespace Walker::Literals;

		if(IsComplete()) return Zero;
		if(rate <= Zero) return std::nullopt;

		auto remainingWork = RequiredWork - CompletedWork;
		return remainingWork * MsPerSec / rate;
	}
}