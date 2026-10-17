#include "Walker/Rebirth/WalkerProgression.h"

#include <Instrumentation/Logging.h>
#include <Utilities/EnumUtils.h>

#include <algorithm>
#include <limits>

namespace Walker {
	UpgradeKey UpgradeKey::Global(GlobalStat stat) {
		DR_ASSERT(Enum::IsValid(stat));
		return UpgradeKey(static_cast<size_t>(stat));
	}

	UpgradeKey UpgradeKey::Vehicle(VehicleKind kind, VehicleStat stat) {
		DR_ASSERT(Enum::IsValid(kind) && Enum::IsValid(stat));
		auto index = static_cast<size_t>(GlobalStat::COUNT) +
			static_cast<size_t>(CrewRole::COUNT) +
			(static_cast<size_t>(kind) * static_cast<size_t>(VehicleStat::COUNT));
		return UpgradeKey(index + static_cast<size_t>(stat));
	}

	UpgradeKey UpgradeKey::Job(CrewRole role) {
		DR_ASSERT(Enum::IsValid(role));
		DR_ASSERT(role != CrewRole::Idle && role != CrewRole::Traveling);
		auto index = static_cast<size_t>(GlobalStat::COUNT) + static_cast<size_t>(role);
		return UpgradeKey(index);
	}

	size_t UpgradeKey::GetIndex() const {
		return m_Index;
	}

	ProgressionPoints WalkerProgression::GetAvailablePoints() const {
		return m_Available;
	}

	UpgradeLevel WalkerProgression::GetLevel(const UpgradeKey& upgrade) const {
		return m_Levels[upgrade.GetIndex()];
	}

	void WalkerProgression::AddPoints(ProgressionPoints points) {
		auto room = static_cast<ProgressionPoints>(std::numeric_limits<ProgressionPoints>::max() - m_Available);
		m_Available += std::min(room, points);
	}

	bool WalkerProgression::TryPurchase(const UpgradeKey& upgrade, u64 levels, ProgressionPoints cost) {
		if(levels == 0 || cost > m_Available) return false;
		auto& level = m_Levels.at(upgrade.GetIndex());
		auto room = static_cast<u64>(std::numeric_limits<UpgradeLevel>::max() - level);
		if(levels > room) return false;

		m_Available -= cost;
		level += static_cast<UpgradeLevel>(levels);
		return true;
	}

	void WalkerProgression::Reset() {
		*this = {};
	}
}