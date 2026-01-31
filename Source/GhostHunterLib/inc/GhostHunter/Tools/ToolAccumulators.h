#pragma once

#include "GhostHunter/Tools/Tools.h"
#include "GhostHunter/Quality/Quality.h"
#include "Mechanics/Accumulator.h"

#include <vector>

namespace GhostHunter {
    std::vector<Accumulator> GetAccumulators(ToolName tool, QualityType level);
}