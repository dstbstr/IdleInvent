#pragma once

#include "Walker/WalkerUnits.h"

#include <array>
#include <string>

namespace Walker {
	enum struct TechKind : u8 {
		Unset,
		Preparation,

		COUNT
	};

	std::string ToString(TechKind kind);

	struct TechState {
		bool Researched{};
		u64 CurrentLevel{};
	};

	Work GetScienceCost(TechKind kind);
	Work GetEngineeringCost(TechKind kind, u64 currentLevel);

	class TechManager {
	public:
		TechManager() = default;
		const TechState& operator[](TechKind kind) const;
		void CompleteResearch(TechKind kind);
		void CompleteUpgrade(TechKind kind);
		void Reset();

	private:
		std::array<TechState, static_cast<size_t>(TechKind::COUNT)> m_State{};
	};
}