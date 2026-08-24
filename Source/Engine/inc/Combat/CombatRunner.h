#pragma once

#include "Combat/Encounter.h"
#include "Combat/CombatController.h"
#include "DesignPatterns/PubSub.h"
#include "GameState/GameTime.h"

#include <map>
#include <optional>

namespace Combat {
	template<typename TCombatant, typename TAction, typename TEvent, typename TRules>
	class Runner {
        using TEncounter = Encounter<TCombatant, TAction, TEvent, TRules>;
        using TController = ICombatController<TAction, TEncounter>;

        static constexpr size_t MaxSteps = 16;

		enum struct StepState { Progressed, WaitingForInput, NotReady, Finished, MissingController, Error };
        struct StepResult {
            StepState State{};
            CombatantId Actor{};
            std::optional<SubmitError> Error{};
        };

    public:
		Runner(TEncounter encounter) : m_Encounter(std::move(encounter)) {}

		void Update(BaseTime elapsed) {
			Publish(m_Encounter.Update(elapsed));

			for(size_t i = 0; i < MaxSteps; i++) {
                auto result = Step();

				if(result.State != StepState::Progressed) break;
			}
		}

		void SetController(CombatantId actor, std::unique_ptr<TController> controller) {
            m_Controllers.insert_or_assign(actor, std::move(controller));
		}

	private:

		StepResult Step() {
            if(m_Encounter.IsFinished()) return {StepState::Finished};
            auto ready = m_Encounter.GetReadyCombatants();
            if(ready.empty()) return {StepState::NotReady};

			auto actor = ready.front();
			if(!m_Encounter.CanAct(actor)) {
                auto events = m_Encounter.SkipTurn(actor);
                Publish(events);
                return {StepState::Progressed, actor};
			}

			auto found = m_Controllers.find(actor);
            if(found == m_Controllers.end()) return {StepState::MissingController, actor};
            auto* controller = found->second.get();

            auto action = controller->GetNextAction(actor, m_Encounter);
            if(!action) return {StepState::WaitingForInput, actor};

			auto result = m_Encounter.Submit(actor, *action);
			if(!result) {
                return {StepState::Error, actor, result.error()};
			}

			Publish(*result);
            return {StepState::Progressed, actor};
		}

		void Publish(const std::vector<TEvent>& events) {
			for(const auto& event : events) {
                m_Events.Publish(event);
			}
		}

        TEncounter m_Encounter;
        std::map<CombatantId, std::unique_ptr<ICombatController<TAction, TEncounter>>> m_Controllers;
        PubSub<TEvent> m_Events{};
	};
	
}