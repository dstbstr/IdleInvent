#include "Combat/CombatSchedule.h"

namespace Combat {
    void RealTimeScheduler::Update(BaseTime elapsed) {
        for (auto& [id, entry] : m_Combatants) {
            if(entry.TimeTillAction <= ZeroTime) continue;

            auto effectiveElapsed = BaseTime{entry.SpeedModifier.Apply(elapsed.count())};
            effectiveElapsed = std::max(effectiveElapsed, ZeroTime);
            entry.TimeTillAction -= effectiveElapsed;
            if (entry.TimeTillAction <= ZeroTime) {
                m_Ready.push_back(id);
            }
        }
    }
    
    std::span<const CombatantId> RealTimeScheduler::GetReadyCombatants() const {
        return m_Ready;
    }
    
    bool RealTimeScheduler::ResetCombatant(CombatantId actor) {
        auto it = std::ranges::find(m_Ready, actor);
        if(it != m_Ready.end()) {
            auto& entry = m_Combatants.at(*it);
            entry.TimeTillAction += entry.ActionInterval;
            if (entry.TimeTillAction > ZeroTime) {
                // if intervals are really large,
                // they could get 2 turns in a row
                m_Ready.erase(it);
            }
            return true;
        } else {
            return false;
        }
    }

    bool RealTimeScheduler::SkipTurn(CombatantId actor) {
        return ResetCombatant(actor);
    }
    
    bool RealTimeScheduler::CompleteAction(CombatantId actor) {
        return ResetCombatant(actor);
    }
    
    void RealTimeScheduler::AddCombatant(CombatantId id, BaseTime interval) {
        DR_ASSERT_MSG(interval > ZeroTime, "Combatant action interval must be greater than zero");
        if(interval <= ZeroTime) return;

        if (!m_Combatants.contains(id)) {
            m_Combatants[id] = Entry{
                .ActionInterval = interval, 
                .TimeTillAction = interval, .SpeedModifier = {}
            };
        }
    }
    
    void RealTimeScheduler::RemoveCombatant(CombatantId id) {
        if (auto it = std::ranges::find(m_Ready, id); it != m_Ready.end()) {
            m_Ready.erase(it);
        }
        m_Combatants.erase(id);
    }
    
    void RealTimeScheduler::ApplySpeedModifier(CombatantId id, Modifier modifier) {
        if (auto it = m_Combatants.find(id); it != m_Combatants.end()) {
            it->second.SpeedModifier += modifier;
        }
    }
}