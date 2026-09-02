#include "Pets/Combat/HuntManager.h"
#include "Pets/Combat/HuntControllers.h"

namespace Pets {
    void HuntManager::Tick(BaseTime elapsed) {
        if(m_Runner) {
            m_Runner->Tick(elapsed);
            if(m_Runner->GetStatus().State == Combat::RunnerState::Finished) {
                EndHunt();
            }
        } else {
            m_RemainingSearchTime -= elapsed;
            if(m_RemainingSearchTime <= ZeroTime) {
                CreateHunt();
                m_RemainingSearchTime += m_SearchTime;
                m_RemainingSearchTime = std::max(m_RemainingSearchTime, ZeroTime);
            }
        }
    }

    void HuntManager::EndHunt() {
        m_PartyController = nullptr;
        m_EventHandle.reset();
        m_Runner.reset();
        m_PartyId = {};
        m_PreyId = {};
    }

    void HuntManager::CreateHunt() { 
        auto rules = HuntRules{m_Inventory};
        auto schedule = std::make_unique<::Combat::RealTimeScheduler>();
        auto encounter = HuntEncounter{rules, std::move(schedule)};

        auto party = GetParty();
        m_PartyId = encounter.AddCombatant(
            Social::ToFactionId(HuntFaction::Party),
            party,
            party.ActionInterval
        );

        auto prey = GetPrey();
        m_PreyId = encounter.AddCombatant(
            Social::ToFactionId(HuntFaction::Prey), 
            prey, prey.ActionInterval);
        
        m_Runner = std::make_unique<HuntCombatRunner>(std::move(encounter));
        m_Runner->SetController(m_PreyId, std::make_unique<PreyController>());
        m_EventHandle = m_Runner->SubscribeEvents([this](const ActionResult& result) { 
            m_ActionResults.Publish(result); 
        });
        SetPartyController(m_PartyManualControl);
    }

    void HuntManager::SetPartyController(bool manual) {
        m_PartyManualControl = manual;
        if(m_Runner && m_PartyId.IsValid()) {
            if (manual) {
                auto partyController = std::make_unique<PartyManualController>();
                m_PartyController = partyController.get();
                m_Runner->SetController(m_PartyId, std::move(partyController));
            } else {
                m_PartyController = nullptr;
                m_Runner->SetController(m_PartyId, std::make_unique<PartyAutoController>());    
            }
        }
    }

    bool HuntManager::IsAwaitingPartyInput() const {
        auto* status = GetStatus();
        if(!status || !m_PartyController) return false;
        return status->State == Combat::RunnerState::WaitingForInput;
    }

    void HuntManager::SetPartyAction(ActionRequest request) {
        if(m_Runner && m_PartyId.IsValid() && m_PartyController) {
            m_PartyController->CurrentAction = request;
        }
    }

    const Combat::RunnerStatus* HuntManager::GetStatus() const {
        if(!m_Runner) return nullptr;
        return &m_Runner->GetStatus();
    }

    std::optional<PreyStats> HuntManager::GetPreyStats() const {
        if(!m_Runner || !m_PreyId.IsValid()) return std::nullopt;
        auto& combatant = m_Runner->GetEncounter().GetRoster().Get(m_PreyId);
        if(auto* stats = std::get_if<PreyStats>(&combatant.Stats)) {
            return *stats;
        }
        return std::nullopt;
    }

    ScopedHandle HuntManager::SubscribeActionResults(const std::function<void(const ActionResult&)>& subscriber) {
        return m_ActionResults.Subscribe(subscriber);
    }
    void HuntManager::SubscribeActionResults(std::vector<ScopedHandle>& outHandles, const std::function<void(const ActionResult&)>& subscriber) {
        m_ActionResults.Subscribe(outHandles, subscriber);
    }

    HuntCombatant HuntManager::GetParty() {
        return HuntCombatant{
            .ActionInterval = OneSecond, 
            .Stats = PartyStats{
                .Attack = 10, 
                .Piercing = 5
            }
        };
    }

    HuntCombatant HuntManager::GetPrey() {
        return HuntCombatant{
            .ActionInterval = OneSecond,
            .Stats = PreyStats{
                .Name = "Bat",
                .CurrentHp = 25,
                .MaxHp = 25,
                .Armor = 5,
                .Dodge = 0.1f,
                .CaptureHpLevel = 0.2f,
                .CaptureChance = 0.5f,
                .Gold = 100,
                .Xp = 50,
                .FleeTime = OneMinute,
                .MaxFleeTime = OneMinute
            }
        };
    }
}