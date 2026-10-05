#include "Walker/Crew/WalkerTech.h"

#include <Instrumentation/Logging.h>
#include <Utilities/EnumUtils.h>

namespace Walker {
	std::string ToString(TechKind kind) {
		switch (kind) {
			using enum TechKind;
			case Preparation: return "Preparation";
			case OfflineEfficiency: return "Offline Efficiency";
			case OfflineCapacity: return "Offline Capacity";
		}
		return "Unknown";
	}

	Work GetScienceCost(TechKind kind) {
		switch(kind) {
			using enum TechKind;
			case Preparation: return Work::Pow10(4);
			case OfflineEfficiency: return Work::Pow10(5);
			case OfflineCapacity: return Work::Pow10(6);
		}
		DR_ASSERT_MSG(false, "Unknown tech kind");
		return Zero;
	}

	Work GetEngineeringCost(TechKind kind, u64 currentLevel) {
		switch(kind) {
			using enum TechKind;
			case Preparation: return Work::Pow10(4).Pow(static_cast<u32>(currentLevel));
			case OfflineEfficiency: return Work::Pow10(5).Pow(static_cast<u32>(currentLevel));
			case OfflineCapacity: return Work::Pow10(6).Pow(static_cast<u32>(currentLevel));
		}
		DR_ASSERT_MSG(false, "Unknown tech kind");
		return Zero;
	}

	std::optional<u64> GetMaxLevel(TechKind kind) {
		if (!Enum::IsValid(kind)) return std::nullopt;

		switch(kind) {
			using enum TechKind;
			case OfflineEfficiency: return 5;
			default: return std::nullopt;
		}
	}

	const TechState& TechManager::operator[](TechKind kind) const {
		DR_ASSERT_MSG(Enum::IsValid(kind), "Invalid tech kind");
		return m_State.at(static_cast<size_t>(kind));
	}

	void TechManager::CompleteResearch(TechKind kind) {
		DR_ASSERT_MSG(Enum::IsValid(kind), "Invalid tech kind");
		auto& state = m_State.at(static_cast<size_t>(kind));
		state.Researched = true;
	}

	void TechManager::CompleteUpgrade(TechKind kind) {
		DR_ASSERT_MSG(Enum::IsValid(kind), "Invalid tech kind");
		auto& state = m_State.at(static_cast<size_t>(kind));
		DR_ASSERT_MSG(state.Researched, "Cannot upgrade unresearched tech");
		state.CurrentLevel++;
	}

	bool TechManager::CanUpgrade(TechKind kind) const {
		DR_ASSERT_MSG(Enum::IsValid(kind), "Invalid tech kind");
		const auto& state = (*this)[kind];
		auto maxLevel = GetMaxLevel(kind);
		return state.Researched && (!maxLevel || state.CurrentLevel < *maxLevel);
	}

	void TechManager::Reset() {
		m_State.fill({});
	}
}