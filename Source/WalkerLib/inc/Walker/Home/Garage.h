#pragma once

#include "Walker/Travel/Vehicle.h"

#include <array>
#include <memory>
#include <vector>

namespace Walker {
	class Garage {
	public:
		std::vector<VehicleKind> GetAvailable() const;
		bool Select(VehicleKind kind);
		OwnedVehicle* GetSelected();
		const OwnedVehicle* GetSelected() const;

		bool Add(VehicleKind kind);
		bool Contains(VehicleKind kind) const;

	private:
		std::array<std::unique_ptr<OwnedVehicle>, static_cast<size_t>(VehicleKind::COUNT)> m_Vehicles{};
		VehicleKind m_Selected{VehicleKind::Unset};
	};
}