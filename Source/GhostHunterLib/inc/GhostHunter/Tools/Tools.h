#pragma once

#include <Platform/NumTypes.h>
#include "Quality/Quality.h"

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
        RitualCircle
    };

    struct Tool {
        ToolName Name{ToolName::Unset};
        QualityType Quality{0};
        Tool() = default;
        Tool(ToolName name, QualityType quality) : Name(name), Quality(quality) {}
    };
}