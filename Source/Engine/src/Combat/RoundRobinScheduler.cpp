#include "Combat/CombatSchedule.h"

#include <ranges>
#include <span>

namespace Combat {
    void RoundRobinScheduler::Start(std::span<const CombatantId> combatants) {
        m_Order.assign(combatants.begin(), combatants.end());
        m_Current = 0;
    }

    void RoundRobinScheduler::Update(BaseTime) {}
    std::span<const CombatantId> RoundRobinScheduler::GetReadyCombatants() const { 
        if(m_Order.empty()) return {};
        return {m_Order.data() + m_Current, 1};
    }

    bool RoundRobinScheduler::Advance(CombatantId actor) {
        if(m_Order.empty() || m_Order.at(m_Current) != actor) return false;

        m_Current = (m_Current + 1) % m_Order.size();
        return true;    
    }

    bool RoundRobinScheduler::CompleteAction(CombatantId actor) {
        return Advance(actor);
    }

    bool RoundRobinScheduler::SkipTurn(CombatantId actor) {
        return Advance(actor);
    }

    void RoundRobinScheduler::AddCombatant(CombatantId combatant) {
        if(m_Order.empty()) {
            m_Order.push_back(combatant);
            m_Current = 0;
            return;
        }

        m_Order.insert(m_Order.begin() + m_Current, combatant); 
        m_Current++;
    }

    void RoundRobinScheduler::AddCombatant(CombatantId combatant, BaseTime) { AddCombatant(combatant); }

    void RoundRobinScheduler::ApplySpeedModifier(CombatantId combatant, Modifier modifier) {
        // RoundRobinScheduler does not currently support speed modifiers
    }

    void RoundRobinScheduler::RemoveCombatant(CombatantId combatant) {
        auto it = std::ranges::find(m_Order, combatant);
        if(it == m_Order.end()) return;

        auto index = static_cast<size_t>(std::distance(m_Order.begin(), it));
        m_Order.erase(it);
        if(m_Order.empty()) {
            m_Current = 0; // probably redundant
            return;
        }
        if(index < m_Current) {
            m_Current--;
        } else if(m_Current >= m_Order.size()) {
            m_Current = 0;
        }
    }
}