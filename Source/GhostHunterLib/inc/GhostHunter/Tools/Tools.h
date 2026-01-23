#pragma once

#include "GhostHunter/Quality/Quality.h"
#include "GameState/GameTime.h"

#include <Platform/NumTypes.h>

#include <vector>
#include <string>

namespace GhostHunter {
    enum struct ToolName : u8 {
        Unset,
        
        Camera,
        Flashlight,
        EmfDetector,
        ThermalCamera,
        EvpRecorder,
        LaserGrid,
        SpiritBox,

        COUNT
    };

    std::string ToString(ToolName tool);
    std::string Describe(ToolName tool);

    struct Tool {
        ToolName Name{ToolName::Unset};
        QualityType Quality{0};

        std::string Describe() const;
    };
}