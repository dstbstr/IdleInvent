#pragma once

#include "Iduel/Economy/EconomyState.h"
#include "Iduel/Simulation/ParticipantState.h"

namespace Iduel {
    struct ParticipantBinding {
        size_t ParticipantIndex{};
        size_t EconomyIndex{};
    };

    class Simulation {
    public:
        Simulation(std::vector<EconomyState> economies, std::vector<ParticipantState> participants, std::vector<ParticipantBinding> bindings, f64 target);

        void Tick(BaseTime elapsed);

		const std::vector<ParticipantState>& GetParticipants() const { return m_Participants; }
		f64 GetTarget() const { return m_Target; }
		std::vector<size_t> GetFinished() const { return m_FinishedParticipants; }

    private:
        std::vector<EconomyState> m_Economies{};
		std::vector<ParticipantState> m_Participants{};
		std::vector<ParticipantBinding> m_Bindings{};
        std::vector<size_t> m_FinishedParticipants{};
        f64 m_Target{0.0};
    };
}