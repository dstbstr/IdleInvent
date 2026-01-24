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

    struct Tool : IUpgradable {
        ToolName Name{ToolName::Unset};
        QualityType Quality{QualityType::Unset};

        Tool(ToolName Name, QualityType quality) : Name(Name), Quality(quality) {}

        std::string Describe() const override;

        void OnUpgrade() override;
        ResourceCollection GetUpgradeCost() const override;
        bool IsMaxLevel() const override;
    };
}