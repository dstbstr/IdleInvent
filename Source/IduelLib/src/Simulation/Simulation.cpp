#include "Iduel/Simulation/Simulation.h"
#include "Iduel/Simulation/IStrategy.h"
#include "Iduel/Economy/Generator.h"

namespace Iduel {
    Simulation::Simulation(std::vector<EconomyState> economies, std::vector<ParticipantState> participants, std::vector<ParticipantBinding> bindings, f64 target)
        : m_Economies(economies)
        , m_Participants(participants)
        , m_Bindings(bindings)
        , m_Target(target) {
        if(target <= 0.0) throw std::invalid_argument("Target must be positive");
        if(m_Bindings.size() != m_Participants.size()) {
            throw std::invalid_argument("Each participant needs a binding");
        }

        std::vector<bool> seen(m_Participants.size(), false);
        for(const auto& binding : m_Bindings) {
			if (binding.EconomyIndex >= m_Economies.size()) throw std::invalid_argument("Binding economy index out of range");
			auto idx = binding.ParticipantIndex;
			if (idx >= m_Participants.size()) throw std::invalid_argument("Binding participant index out of range");
            if(seen[idx]) throw std::invalid_argument("Participant already bound");
            seen[idx] = true;
        }
    }

    void Simulation::Tick(BaseTime elapsed) {
        if (elapsed <= ZeroTime || !m_FinishedParticipants.empty()) return;

        std::vector<f64> payouts(m_Economies.size(), 0.0);
        for(size_t eIndex = 0; eIndex < m_Economies.size(); ++eIndex) {
			auto& econ = m_Economies[eIndex];
            for(size_t i = 0; i < econ.GenCounts.size(); i++) {
                auto owned = econ.GenCounts[i];
                if(owned == 0) continue;

                auto& gen = econ.Generators[i];
				auto requiredTime = gen.GetTimePerPayout(owned);
                auto payout = gen.GetIncomePerPayout(owned);
				econ.PayoutElapsed[i] += elapsed;

				while (econ.PayoutElapsed[i] >= requiredTime) {
					econ.PayoutElapsed[i] -= requiredTime;
					payouts[eIndex] += payout;
				}
            }
        }

        for(auto [pIndex, eIndex] : m_Bindings) {
			auto& wallet = m_Participants[pIndex].Wallet;
			wallet += payouts[eIndex];
            if(wallet >= m_Target) {
                m_FinishedParticipants.push_back(pIndex);
            }
        }

        if(!m_FinishedParticipants.empty()) {
            return;
        }

        for(auto [pIndex, eIndex] : m_Bindings) {
			auto& participant = m_Participants[pIndex];
			auto& econ = m_Economies[eIndex];

			if(auto result = participant.Strategy->ChoosePurchase(econ, participant)) {
				auto genIdx = result->GeneratorIndex;
                auto& gen = econ.Generators[genIdx];
                f64 cost = 0.0;
				for (size_t i = 0; i < result->Count; ++i) {
					cost += gen.GetCost(econ.GenCounts[genIdx] + i);
				}
				if (participant.Wallet >= cost) {
					participant.Wallet -= cost;

					auto& genCount = econ.GenCounts[genIdx];
					auto oldTime = gen.GetTimePerPayout(genCount);
					econ.GenCounts[genIdx] += result->Count;
					auto newTime = gen.GetTimePerPayout(genCount);

                    if(newTime < oldTime) {
                        econ.PayoutElapsed[genIdx] = std::min(econ.PayoutElapsed[genIdx], newTime);
                    }
				}
            }
		}
    }
}