#pragma once

#include "Walker/Travel/Vehicle.h"

#include <imgui.h>

#include <span>

namespace Walker::WalkerUi {
	bool VehicleSelector(const char* id, std::span<const VehicleKind> choices, VehicleKind& selected);
}