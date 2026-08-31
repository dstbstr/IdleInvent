#pragma once

#include "Combat/Encounter.h"
#include "Combat/CombatController.h"
#include "DesignPatterns/PubSub.h"
#include "GameState/GameTime.h"

#include <map>
#include <optional>

namespace Combat {
	enum struct RunnerState { Progressed, WaitingForInput, NotReady, Finished, MissingController, Error };
    struct RunnerStatus {
        RunnerState State{};
        CombatantId Actor{};
        std::optional<SubmitError> Error{};

		constexpr auto operator<=>(const RunnerStatus&) const = default;
    };

	struct RunnerConfig {
        size_t MaxStepsPerTick{16};
	};

	template<typename TCombatant, typename TAction, typename TEvent, typename TRules>
	class Runner {
        using TEncounter = Encounter<TCombatant, TAction, TEvent, TRules>;
        using TController = ICombatController<TAction, TEncounter>;

    public:
		Runner(TEncounter encounter, RunnerConfig config = {}) : m_Encounter(std::move(encounter)), m_Config(config) {}

		void Tick(BaseTime elapsed) {
			Publish(m_Encounter.Update(elapsed));

			for(size_t i = 0; i < m_Config.MaxStepsPerTick; i++) {
                auto result = Step();

				if(result.State == RunnerState::Progressed) continue;

				SetStatus(result);
                return;
			}

			SetStatus({.State = RunnerState::Progressed});
		}

		void SetController(CombatantId actor, std::unique_ptr<TController> controller) {
            m_Controllers.insert_or_assign(actor, std::move(controller));
		}

		[[nodiscard]] ScopedHandle SubscribeEvents(const std::function<void(const TEvent&)>& subscriber) {
            return m_Events.Subscribe(subscriber);
		}
		[[nodiscard]] ScopedHandle SubscribeStatus(const std::function<void(const RunnerStatus&)>& subscriber) {
            return m_StatusEvents.Subscribe(subscriber);
		}
        [[nodiscard]] const TEncounter& GetEncounter() const { return m_Encounter; }
		[[nodiscard]] const RunnerStatus& GetStatus() const { return m_Status; }

	private:
		RunnerStatus Step() {
            if(m_Encounter.IsFinished()) return {RunnerState::Finished};
            auto ready = m_Encounter.GetReadyCombatants();
            if(ready.empty()) return {RunnerState::NotReady};

			auto actor = ready.front();
			if(m_Encounter.IsDisabled(actor)) {
                auto events = m_Encounter.SkipTurn(actor);
                Publish(events);
                return {RunnerState::Progressed, actor};
			}

			auto found = m_Controllers.find(actor);
            if(found == m_Controllers.end()) return {RunnerState::MissingController, actor};
            auto* controller = found->second.get();

            auto action = controller->GetNextAction(actor, m_Encounter);
            if(!action) return {RunnerState::WaitingForInput, actor};

			auto result = m_Encounter.Submit(actor, *action);
			if(!result) {
                return {RunnerState::Error, actor, result.error()};
			}

			Publish(*result);
            return {RunnerState::Progressed, actor};
		}

		void Publish(const std::vector<TEvent>& events) {
			for(const auto& event : events) {
                m_Events.Publish(event);
			}
		}

		void SetStatus(RunnerStatus status) {
            if(status == m_Status) return;

			m_Status = status;
            m_StatusEvents.Publish(m_Status);
		}

        TEncounter m_Encounter;
        RunnerConfig m_Config{};
        std::map<CombatantId, std::unique_ptr<ICombatController<TAction, TEncounter>>> m_Controllers;
        PubSub<TEvent> m_Events{};
        PubSub<RunnerStatus> m_StatusEvents{};
        RunnerStatus m_Status{};
	};
	
}