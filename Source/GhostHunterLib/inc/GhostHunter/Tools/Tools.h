#pragma once

#include "GhostHunter/Quality/Quality.h"

#include "GameState/GameTime.h"
#include "Mechanics/Upgrade.h"

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
        using TypeEnum = ToolName;
        using LevelEnum = QualityType;

        ToolName Id{ToolName::Unset};
        QualityType Level{QualityType::Unset};

        std::string Describe() const;
        void OnUpgrade();
    };

    static_assert(UpgradableType<Tool>);
}