#pragma once

#include "Walker/Journey/Endpoints.h"
#include "Walker/Travel/Vehicle.h"

#include <memory>
#include <span>

namespace Walker::WalkerUi {
	bool VehicleSelector(const char* id, std::span<const VehicleKind> choices, VehicleKind& selected);
	bool EndpointSelector(const char* id, std::span<const std::unique_ptr<EndpointInstance>> choices, EndpointInstance*& selected);
	bool ScoutSelector(const char* id, std::span<const EndpointKind> choices, EndpointKind& selected);
}