#pragma once

#include "Walker/WalkerUnits.h"
#include <Platform/NumTypes.h>

namespace Walker::WalkerUi {
	void EtaProgressBar(f32 progress, std::optional<Time> eta, ImVec2 size = ImVec2{-1.f, 0.f});
}