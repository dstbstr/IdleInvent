#pragma once

#include "Combat/CombatRoster.h"
#include "Combat/CombatSchedule.h"
#include "Social/Faction.h"
#include "Mechanics/Modifier.h"

#include <expected>
#include <memory>
#include <vector>

namespace Combat {
    enum struct SubmitError : u8 { EncounterFinished, ActorNotReady, InvalidAction };
    enum struct TurnEnd : u8 { Acted, Skipped };

	struct EncounterProgress {
        BaseTime Elapsed{};
        u64 CompletedTurns{};
	};

    struct SchedulerChange {
        CombatantId Combatant{};
        // may need to support timeout on these
        Modifier SpeedModifier{};
    };

	template<typename TEvent>
	struct ActionResolution {
        std::vector<TEvent> Events{};
        std::vector<CombatantId> RemovedCombatants{};
        std::vector<SchedulerChange> ScheduleChanges{};
        bool EncounterFinished{false};
	};

	template<typename TCombatant, typename TAction, typename TEvent, typename TRules>
	class Encounter {
    public:
		Encounter(TRules rules, std::unique_ptr<IScheduler> schedule) 
			: m_Rules(std::move(rules))
			, m_Schedule(std::move(schedule))
		{}

		std::vector<TEvent> Update(BaseTime elapsed) {
            m_Progress.Elapsed += elapsed;
            auto resolution = m_Rules.Update(m_Roster, elapsed);
            ApplyResolution(resolution);
			m_Schedule->Update(elapsed);

			return std::move(resolution.Events);
		}

		CombatantId AddCombatant(Social::FactionId faction, TCombatant combatant) {
            auto id = m_Roster.Add(faction, std::move(combatant));
            m_Schedule->AddCombatant(id);
            return id;
        }

		[[nodiscard]] std::span<const CombatantId> GetReadyCombatants() const {
            if(m_Finished) return {};
            return m_Schedule->GetReadyCombatants();
		}

		std::vector<TEvent> SkipTurn(CombatantId actor) {
            if(m_Finished) return {};

			auto ready = m_Schedule->GetReadyCombatants();
            if(std::ranges::find(ready, actor) == ready.end()) return {};

            m_Progress.CompletedTurns++;
			auto resolution = m_Rules.EndTurn(m_Roster, actor, TurnEnd::Skipped, m_Progress);
            m_Schedule->SkipTurn(actor);
            ApplyResolution(resolution);

            return std::move(resolution.Events);
		}

		[[nodiscard]] const Roster<TCombatant>& GetRoster() const { return m_Roster; }
        [[nodiscard]] const EncounterProgress& GetProgress() const { return m_Progress; }
        [[nodiscard]] bool IsFinished() const { return m_Finished; }
		[[nodiscard]] bool IsDisabled(CombatantId actor) const { return m_Rules.IsDisabled(m_Roster, actor); }
		[[nodiscard]] bool CanSubmit(CombatantId actor, const TAction& action) const {
            return m_Rules.CanSubmit(m_Roster, actor, action);
		}

		std::expected<std::vector<TEvent>, SubmitError> Submit(CombatantId actor, const TAction& action) {
			if(m_Finished) return std::unexpected(SubmitError::EncounterFinished);

			auto ready = m_Schedule->GetReadyCombatants();

			if(std::ranges::find(ready, actor) == ready.end()) return std::unexpected(SubmitError::ActorNotReady);
			if(!m_Rules.CanSubmit(m_Roster, actor, action)) return std::unexpected(SubmitError::InvalidAction);

			auto actionResolution = m_Rules.Resolve(m_Roster, actor, action);
            m_Progress.CompletedTurns++;
            auto turnResolution = m_Rules.EndTurn(m_Roster, actor, TurnEnd::Acted, m_Progress);

            m_Schedule->CompleteAction(actor);

			ApplyResolution(actionResolution);
            ApplyResolution(turnResolution);

            auto events = actionResolution.Events;
            std::copy(turnResolution.Events.begin(), turnResolution.Events.end(), std::back_inserter(events));
			return std::move(events);
		}

	private:
        void ApplyResolution(const ActionResolution<TEvent>& resolution) {
            for(auto combatant: resolution.RemovedCombatants) {
                m_Schedule->RemoveCombatant(combatant);
            }
            for(auto change: resolution.ScheduleChanges) {
                m_Schedule->ApplySpeedModifier(change.Combatant, change.SpeedModifier);
            }
            m_Finished |= resolution.EncounterFinished;
        }

        TRules m_Rules;
        std::unique_ptr<IScheduler> m_Schedule;
        Roster<TCombatant> m_Roster{};
        bool m_Finished{false};
        EncounterProgress m_Progress{};
	};
}