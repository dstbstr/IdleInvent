#pragma once

#include <Platform/NumTypes.h>
#include <Ui/Particle/Emitter.h>
#include <GameState/GameTime.h>

#include <imgui.h>

#include <vector>

namespace Ui {

    class ParticleSystem {
    public:
        explicit ParticleSystem(size_t capacity = 4096);

        // Emitters are owned by the system. References stay stable as long as you don't add
        // beyond the initial reserve (16) -- adjust the reserve in the .cpp if you need more.
        Emitter& AddEmitter();
        Emitter& AddEmitter(Emitter init);
        void     RemoveAllEmitters();

        void     Update(BaseTime elapsed);

        // Draws filled circles to ImGui::GetWindowDrawList() at each particle's screen
        // position. Caller is responsible for being inside whatever window context they want.
        void     Render() const;

        // O(1) clear -- just resets the active count, doesn't shrink the pool.
        void     Clear();

        size_t   ParticleCount() const { return m_ActiveCount; }
        size_t   ParticleCapacity() const { return m_Particles.size(); }

    private:
        void SpawnParticle(const Emitter& emitter, u8 emitterId);

        std::vector<Particle> m_Particles;     // size == capacity, never resized after ctor
        size_t                m_ActiveCount{0};
        std::vector<Emitter>  m_Emitters;
    };
}

