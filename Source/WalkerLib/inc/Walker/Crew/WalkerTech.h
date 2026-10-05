#pragma once

#include "Walker/WalkerUnits.h"
#include <Platform/NumTypes.h>

#include <array>
#include <optional>
#include <string>

namespace Walker {
	enum struct TechKind : u8 {
		Unset,
		Preparation,
		OfflineEfficiency,
		OfflineCapacity,

		COUNT
	};

	std::string ToString(TechKind kind);

	struct TechState {
		bool Researched{};
		u64 CurrentLevel{1};
	};

	Work GetScienceCost(TechKind kind);
	Work GetEngineeringCost(TechKind kind, u64 currentLevel);
	std::optional<u64> GetMaxLevel(TechKind kind);

	class TechManager {
	public:
		TechManager() = default;
		const TechState& operator[](TechKind kind) const;
		bool CanUpgrade(TechKind kind) const;
		void CompleteResearch(TechKind kind);
		void CompleteUpgrade(TechKind kind);
		void Reset();

	private:
		std::array<TechState, static_cast<size_t>(TechKind::COUNT)> m_State{};
	};
}