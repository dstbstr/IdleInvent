#include "Ui/Particle/ParticleSystem.h"

#include <imgui.h>

#include <cmath>
#include <random>

namespace {
    // File-scope RNG so the header doesn't pull in <random> and every ParticleSystem instance
    // doesn't carry a ~5KB mt19937 around. Single-threaded; if you ever do off-thread updates,
    // swap for thread_local.
    std::mt19937& Rng() {
        static std::mt19937 rng{std::random_device{}()};
        return rng;
    }

    f32 RandFloat(f32 min, f32 max) {
        if(min >= max) return min;
        std::uniform_real_distribution<f32> dist(min, max);
        return dist(Rng());
    }

    u16 RandU16(u16 min, u16 max) {
        if(min >= max) return min;
        // uniform_int_distribution<u16> isn't a standard-portable choice; widen to int.
        std::uniform_int_distribution<int> dist(min, max);
        return static_cast<u16>(dist(Rng()));
    }
}

namespace Ui {
    ParticleSystem::ParticleSystem(size_t capacity)
        : m_Particles(capacity) {
        m_Emitters.reserve(16);
    }

    Emitter& ParticleSystem::AddEmitter() {
        m_Emitters.emplace_back();
        return m_Emitters.back();
    }

    Emitter& ParticleSystem::AddEmitter(Emitter init) {
        m_Emitters.push_back(init);
        return m_Emitters.back();
    }

    void ParticleSystem::RemoveAllEmitters() {
        // Drop existing particles too -- their EmitterId would dangle otherwise.
        m_ActiveCount = 0;
        m_Emitters.clear();
    }

    void ParticleSystem::SetGravity(ImVec2 g) { m_Gravity = g; }
    ImVec2 ParticleSystem::GetGravity() const { return m_Gravity; }

    void ParticleSystem::Clear() { m_ActiveCount = 0; }

    // this is a hot loop, so we avoid bounds checking.
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    void ParticleSystem::Update(BaseTime elapsed) {
        const auto dtMsWide   = static_cast<u32>(elapsed.count());
        const f32  dtSec      = static_cast<f32>(dtMsWide) * 0.001f;
        const u16  dtMsClamp  = dtMsWide > 0xFFFFu ? u16{0xFFFFu} : static_cast<u16>(dtMsWide);
        const size_t capacity = m_Particles.size();

        // 1) Emit new particles from active emitters.
        for(size_t id = 0; id < m_Emitters.size(); ++id) {
            auto& e = m_Emitters.at(id);
            if(!e.Enabled) continue;

            e.EmissionAccumulator += e.RatePerSecond * dtSec;
            while(e.EmissionAccumulator >= 1.f && m_ActiveCount < capacity) {
                e.EmissionAccumulator -= 1.f;
                SpawnParticle(e, static_cast<u8>(id));
            }
            // If we hit capacity, drop the leftover accumulator so we don't burst-spawn later.
            if(m_ActiveCount >= capacity) {
                e.EmissionAccumulator = 0.f;
            }
        }

        // 2) Tick existing particles + swap-and-pop dead ones.
        for(size_t i = 0; i < m_ActiveCount; /* manual increment */) {
            auto& p = m_Particles.at(i);
            p.Velocity.x += m_Gravity.x * dtSec;
            p.Velocity.y += m_Gravity.y * dtSec;
            p.Position.x += p.Velocity.x * dtSec;
            p.Position.y += p.Velocity.y * dtSec;

            if(p.LifeRemainingMs > dtMsClamp) {
                p.LifeRemainingMs -= dtMsClamp;
                ++i;
            } else {
                // Dead: swap with last active, decrement count, re-check this slot next iteration.
                m_Particles.at(i) = m_Particles.at(--m_ActiveCount);
            }
        }
    }

    void ParticleSystem::Render() const {
        auto* drawList = ImGui::GetWindowDrawList();
        for(size_t i = 0; i < m_ActiveCount; ++i) {
            //const auto& p = m_Particles.at(i);
            //const auto& e = m_Emitters.at(p.EmitterId);
            const auto& p = m_Particles[i];
            const auto& e = m_Emitters[p.EmitterId];
            const u32 alpha = (p.LifeRemainingMs >= e.LifeMaxMs)
                ? 255u
                : (255u * p.LifeRemainingMs) / e.LifeMaxMs;
            const u32 color = (e.Color & 0x00FFFFFFu) | (alpha << 24);
            drawList->AddCircleFilled(p.Position, e.Size, color);
        }
    }

    void ParticleSystem::SpawnParticle(const Emitter& emitter, u8 emitterId) {
        const f32 angle = RandFloat(emitter.AngleMin, emitter.AngleMax);
        const f32 speed = RandFloat(emitter.SpeedMin, emitter.SpeedMax);
        const u16 life  = RandU16(emitter.LifeMinMs, emitter.LifeMaxMs);
        const f32 jx    = emitter.PositionJitter.x > 0.f ? RandFloat(-emitter.PositionJitter.x, emitter.PositionJitter.x) : 0.f;
        const f32 jy    = emitter.PositionJitter.y > 0.f ? RandFloat(-emitter.PositionJitter.y, emitter.PositionJitter.y) : 0.f;

        auto& p = m_Particles.at(m_ActiveCount++);
        p.Position        = ImVec2{emitter.Position.x + jx, emitter.Position.y + jy};
        p.Velocity        = ImVec2{std::cos(angle) * speed, std::sin(angle) * speed};
        p.LifeRemainingMs = life;
        p.EmitterId       = emitterId;
        p._reserved       = 0;
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
}