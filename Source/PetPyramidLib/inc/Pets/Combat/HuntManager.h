#pragma once

#include "Pets/Character/Party.h"
#include "Pets/Character/PartyResolver.h"
#include "Pets/Combat/HuntCombatant.h"
#include "Pets/Combat/HuntControllers.h"
#include "Pets/Combat/HuntTypes.h"
#include "Pets/Combat/HuntEvents.h"
#include "Pets/Combat/HuntRules.h"
#include "Pets/Inventory/Inventory.h"
#include "Pets/Pets/Leveling.h"

#include <Combat/CombatRunner.h>

#include <memory>
#include <optional>

namespace Pets {

    class HuntManager {
    public:
        HuntManager(Inventory& inventory, Party& party, PetRoster& roster, BaseTime searchTime = OneSecond * 30)
            : m_SearchTime(searchTime)
            , m_RemainingSearchTime(searchTime)
            , m_Runner(nullptr)
            , m_Inventory(inventory)
            , m_Party(party)
            , m_Roster(roster) {}

        void Tick(BaseTime elapsed);
        bool IsHunting() const { return m_Runner != nullptr; }
        void SetPartyController(bool manual);
        bool IsPartyManual() const { return m_PartyManualControl; }
        bool IsAwaitingPartyInput() const;
        void SetPartyAction(ActionRequest request);
        const Combat::RunnerStatus* GetStatus() const;
        Combat::CombatantId GetPreyId() const { return m_PreyId; }
        std::optional<PreyStats> GetPreyStats() const;

        ScopedHandle SubscribeActionResults(const std::function<void(const ActionResult&)>& subscriber);
        void SubscribeActionResults(std::vector<ScopedHandle>& outHandles, const std::function<void(const ActionResult&)>& subscriber);

        ScopedHandle SubscribeLevelingEvents(const std::function<void(const Leveling::Event&)>& subscriber);
        void SubscribeLevelingEvents(std::vector<ScopedHandle>& outHandles, const std::function<void(const Leveling::Event&)>& subscriber);
    private:
        BaseTime m_SearchTime{};
        BaseTime m_RemainingSearchTime{};
        std::unique_ptr<HuntCombatRunner> m_Runner{};
        PubSub<ActionResult> m_ActionResults{};
        PubSub<Leveling::Event> m_LevelingEvents{};
        std::optional<ScopedHandle> m_EventHandle{};

        Inventory& m_Inventory;
        Party& m_Party;
        PetRoster& m_Roster;
        std::optional<PartyResolution> m_CurrentResolution{};
        Combat::CombatantId m_PartyId{};
        Combat::CombatantId m_PreyId{};
        
        bool m_PartyManualControl{true};
        PartyManualController* m_PartyController{nullptr};

        void CreateHunt();
        void EndHunt();
        
        void HandleActionResult(const ActionResult& result);
        void OnPreyCaptured();
        void OnPreyKilled();
        void OnPreyFled();
    };
}