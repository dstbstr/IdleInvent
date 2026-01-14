#pragma once

#include <Platform/NumTypes.h>
#include "GhostHunter/Quality/Quality.h"

#include <vector>
#include <string>

namespace GhostHunter {
    enum struct ToolName : u8 {
        Unset,

        Flashlight,
        EmfDetector,
        ThermalCamera,
        EvpRecorder,
        LaserGrid,
        SpiritBox,

        FogMachine,
        Costumes,
        Makeup,
        EffectCrew,
        CameraCrew,
        TourBus,

        TarotDeck,
        AutomaticWritingKit,
        CrystalBall,
        MagicMirror,
        SeanceTable,
        RitualCircle,

        COUNT
    };

    std::string ToString(ToolName tool);
    std::string Describe(ToolName tool);

    struct Tool {
        ToolName Name{ToolName::Unset};
        QualityType Quality{0};
        Tool() = default;
        Tool(ToolName name, QualityType quality) : Name(name), Quality(quality) {}

        void Upgrade();
    };

    namespace Tools {
        void Initialize();
        void ShutDown();
    }
}