#include "Walker/Home/Garage.h"

#include <Utilities/EnumUtils.h>

namespace Walker {
	std::vector<VehicleKind> Garage::GetAvailable() const {
		std::vector<VehicleKind> result;
		for(auto v : Enum::GetAllValues<VehicleKind>()) {
			if(m_Vehicles[static_cast<size_t>(v)]) result.push_back(v);
		}
		return result;
	}

	bool Garage::Select(VehicleKind kind) {
		auto index = static_cast<size_t>(kind);
		if (kind == VehicleKind::Unset || index >= m_Vehicles.size()) return false;
		if(!m_Vehicles[index]) return false;

		m_Selected = kind;
		return true;
	}

	OwnedVehicle* Garage::GetSelected() {
		return m_Vehicles[static_cast<size_t>(m_Selected)].get();
	}
	const OwnedVehicle* Garage::GetSelected() const {
		return m_Vehicles[static_cast<size_t>(m_Selected)].get();
	}

	bool Garage::Add(VehicleKind kind) {
		auto index = static_cast<size_t>(kind);
		if(kind == VehicleKind::Unset || index >= m_Vehicles.size()) return false;
		if(m_Vehicles[index]) return false;

		m_Vehicles.at(index) = std::make_unique<OwnedVehicle>(kind);
		return true;
	}

	bool Garage::Contains(VehicleKind kind) const {
		return m_Vehicles[static_cast<size_t>(kind)] != nullptr;
	}
}