#pragma once

#include "GhostHunter/Quality/Quality.h"
#include "GameState/GameTime.h"

#include <Platform/NumTypes.h>

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
        Tool(ToolName name, QualityType quality) : Name(name), Quality(quality) {}

        ToolName Name{ToolName::Unset};
        QualityType Quality{0};

        void Upgrade();

        std::string Describe() const;
    };

    namespace Tools {
        void Initialize();
        void ShutDown();
    }
}