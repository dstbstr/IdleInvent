#pragma once

#include "Walker/Crew/CrewRole.h"
#include "Walker/Travel/Vehicle.h"

#include <Platform/NumTypes.h>

#include <array>

namespace Walker {
	enum struct VehicleStat : u8 {
		Unset,
		Accel,
		Speed,
		Capacity,

		COUNT
	};
	enum struct GlobalStat : u8 {
		Unset,
		CargoWork,
		Jobs,
		Accel,
		Speed,
		Capacity,

		COUNT
	};

	class UpgradeKey {
	public:
		static constexpr size_t SlotCount = 
			static_cast<size_t>(GlobalStat::COUNT) +
			static_cast<size_t>(CrewRole::COUNT) +
			(static_cast<size_t>(VehicleKind::COUNT) * static_cast<size_t>(VehicleStat::COUNT));

		static UpgradeKey Global(GlobalStat stat);
		static UpgradeKey Vehicle(VehicleKind kind, VehicleStat stat);
		static UpgradeKey Job(CrewRole role);

		size_t GetIndex() const;

	private:
		explicit UpgradeKey(size_t index) : m_Index(index) {}
		size_t m_Index;
	};

	class WalkerProgression {
	public:
		ProgressionPoints GetAvailablePoints() const;
		UpgradeLevel GetLevel(const UpgradeKey& upgrade) const;

		void AddPoints(ProgressionPoints points);
		bool TryPurchase(const UpgradeKey& upgrade, u64 levels, ProgressionPoints cost);
		void Reset();

	private:
		ProgressionPoints m_Available{0};
		std::array<UpgradeLevel, UpgradeKey::SlotCount> m_Levels{};
	};
}