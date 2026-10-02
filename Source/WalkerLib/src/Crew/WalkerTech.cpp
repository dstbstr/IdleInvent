#include "Walker/Crew/WalkerTech.h"

#include <Instrumentation/Logging.h>

namespace Walker {
	std::string ToString(TechKind kind) {
		switch (kind) {
			using enum TechKind;
			case Preparation: return "Preparation";
		}
		return "Unknown";
	}

	Work GetScienceCost(TechKind kind) {
		switch(kind) {
			using enum TechKind;
			case Preparation: return Work::Pow10(4);
		}
		DR_ASSERT_MSG(false, "Unknown tech kind");
		return Zero;
	}

	Work GetEngineeringCost(TechKind kind, u64 currentLevel) {
		switch(kind) {
			using enum TechKind;
			case Preparation: return Work::Pow10(4).Pow(static_cast<u32>(currentLevel + 1));
		}
		DR_ASSERT_MSG(false, "Unknown tech kind");
		return Zero;
	}

	const TechState& TechManager::operator[](TechKind kind) const {
		DR_ASSERT_MSG(kind > TechKind::Unset && kind < TechKind::COUNT, "Invalid tech kind");
		return m_State.at(static_cast<size_t>(kind));
	}

	void TechManager::CompleteResearch(TechKind kind) {
		DR_ASSERT_MSG(kind > TechKind::Unset && kind < TechKind::COUNT, "Invalid tech kind");
		auto& state = m_State.at(static_cast<size_t>(kind));
		state.Researched = true;
	}

	void TechManager::CompleteUpgrade(TechKind kind) {
		DR_ASSERT_MSG(kind > TechKind::Unset && kind < TechKind::COUNT, "Invalid tech kind");
		auto& state = m_State.at(static_cast<size_t>(kind));
		DR_ASSERT_MSG(state.Researched, "Cannot upgrade unresearched tech");
		state.CurrentLevel++;
	}

	void TechManager::Reset() {
		m_State.fill({});
	}
}