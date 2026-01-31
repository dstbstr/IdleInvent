#pragma once

#include "GhostHunter/Quality/Quality.h"

#include "GameState/GameTime.h"
#include "Mechanics/Accumulator.h"
#include "Mechanics/Upgrade.h"

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

    class Tool {
        Handle m_TickHandle{InvalidHandle};

        std::vector<Accumulator> m_Accumulators;

    public:
        using IdType = ToolName;
        using LevelType = QualityType;

        Tool(ToolName id, QualityType level);

        ToolName Id{ToolName::Unset};
        QualityType Level{QualityType::Unset};

        std::string Describe() const;
        void OnUpgrade();
        void Tick(BaseTime elapsed);

        void Start();
        void Stop();
    };
}