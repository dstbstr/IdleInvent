#pragma once
#include <Constexpr/ConstexprMath.h>
#include <Platform/NumTypes.h>
#include <Ui/Direction.h>

#include <imgui.h>

namespace Ui {
    // Emitter owns the shared appearance + spawn distributions. Consumers usually mutate
    // fields directly (Enabled, Position, etc.) -- e.g. enable while the mouse is held.
    struct Emitter {
        bool Enabled{false};
        ImVec2 Position{};
        ImVec2 PositionJitter{}; // each spawn perturbs Position by uniform(-jitter, +jitter)
        ImVec2 Gravity{}; // applied per-frame to every particle from this emitter (px/sec^2)
        f32 RatePerSecond{50.f};
        f32 AngleMin{0.f}, AngleMax{Constexpr::DegToRad(360.f)}; // spawn direction range, radians
        f32 SpeedMin{50.f}, SpeedMax{150.f}; // px/sec range
        u16 LifeMinMs{500}, LifeMaxMs{1'500}; // life range; LifeMaxMs also drives alpha fade
        f32 Size{4.f}; // shared across all particles from this emitter
        ImU32 Color{0xFFFFFFFF}; // shared; alpha is overridden per-particle by fade

        // ParticleSystem internal -- leave alone.
        f32 EmissionAccumulator{0.f};
    };

    // Named spread tiers for emitter arcs. Values are the FULL spread; SetArc uses
    // half-width around the direction's center angle.
    //  Laser   ~5deg   pinpoint beam
    //  Thin    ~20deg  tight cone
    //  Narrow  ~45deg  classic fountain arc
    //  Wide    ~90deg  quarter circle
    //  Half    180deg  hemisphere
    //  Full    360deg  omni-directional; ignores Direction
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
    inline void SetArc(Emitter& e, Direction dir, Spread spread) {
        if(spread == Spread::Full) {
            e.AngleMin = 0.f;
            e.AngleMax = Constexpr::DegToRad(360.f);
            return;
        }
        const f32 center    = ToAngleRadians(dir);
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