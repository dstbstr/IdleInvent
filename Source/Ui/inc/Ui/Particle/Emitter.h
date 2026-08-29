#pragma once
#include <Constexpr/ConstexprMath.h>
#include <Platform/NumTypes.h>
#include <Ui/Direction.h>

#include <imgui.h>

namespace Ui {
    struct EmitterSettings {
        ImVec2 PositionJitter{};
        ImVec2 Gravity{};
        f32 RatePerSecond{50.f};
        f32 AngleMin{0.f};
        f32 AngleMax{Constexpr::DegToRad(360.f)};
        f32 SpeedMin{50.f};
        f32 SpeedMax{150.f};
        u16 LifeMinMs{500};
        u16 LifeMaxMs{1'500};
        f32 Size{4.f};
        ImU32 Color{IM_COL32_WHITE};
    };

    struct Emitter {
        bool Enabled{false};
        ImVec2 Position{};
        EmitterSettings Settings{};

        // ParticleSystem internal -- leave alone.
        f32 EmissionAccumulator{0.f};
    };

    enum struct Spread : u8 {
        Laser, Thin, Narrow, Wide, Half, Full
    };

    [[nodiscard]] constexpr f32 ToRadians(Spread s) {
        switch(s) {
            using enum Spread;
            case Laser:  return Constexpr::DegToRad(5.f);
            case Thin:   return Constexpr::DegToRad(20.f);
            case Narrow: return Constexpr::DegToRad(45.f);
            case Wide:   return Constexpr::DegToRad(90.f);
            case Half:   return Constexpr::DegToRad(180.f);
            case Full:   return Constexpr::DegToRad(360.f);
        }
        return 0.f;
    }

    // Writes AngleMin/AngleMax on an emitter using named direction + spread. For arbitrary
    // angles, set AngleMin/AngleMax directly. Spread::Full ignores the direction and emits
    // omni-directionally.
    inline void SetArc(EmitterSettings& e, Direction dir, Spread spread) {
        if(spread == Spread::Full) {
            e.AngleMin = 0.f;
            e.AngleMax = Constexpr::DegToRad(360.f);
            return;
        }
        const f32 center = ToAngleRadians(dir);
        const f32 halfWidth = ToRadians(spread) * 0.5f;
        e.AngleMin = center - halfWidth;
        e.AngleMax = center + halfWidth;
    }

    // 20-byte particle, 3 per 64-byte cache line.
    struct Particle {
        ImVec2 Position{};
        ImVec2 Velocity{};
        u16 LifeRemainingMs{0};
        u8 EmitterId{0}; //index into ParticleSystem's emitter table
        u8 _reserved{0}; // spare for future per-particle flag
    };
}