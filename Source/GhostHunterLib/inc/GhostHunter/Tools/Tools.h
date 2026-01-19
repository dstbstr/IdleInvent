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

    class Tool {
        ToolName m_Name{ToolName::Unset};
        QualityType m_Quality{0};
        BaseTime m_Duration{0};
        BaseTime m_Elapsed{0};
        bool m_Active{false};

    public:
        Tool(ToolName name, QualityType quality) : m_Name(name), m_Quality(quality) {}

        void Update(BaseTime elapsed);
        void Upgrade();
        void Use();
        f32 GetProgress() const;

        std::string Describe() const;
    };

    namespace Tools {
        void Initialize();
        void ShutDown();

        void Update(BaseTime elapsed);
    }
}