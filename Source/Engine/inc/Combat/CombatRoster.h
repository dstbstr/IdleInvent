#pragma once

#include "Combat/CombatTypes.h"
#include "Social/Faction.h"

#include <map>
#include <utility>

namespace Combat {
	template<typename TCombatant>
	class Roster {
        struct Entry {
            Social::FactionId Faction{};
            TCombatant Combatant;
        };
    public:
        CombatantId Add(Social::FactionId faction, TCombatant combatant) {
            auto id = CombatantId{m_NextId++};
            m_Roster.emplace(id, Entry{faction, std::move(combatant)});
            return id;
        }

        [[nodiscard]] bool Contains(CombatantId id) const { return m_Roster.contains(id); }
        [[nodiscard]] size_t Size() const { return m_Roster.size(); }
        TCombatant& Get(CombatantId id) { return m_Roster.at(id).Combatant; }
        const TCombatant& Get(CombatantId id) const { return m_Roster.at(id).Combatant; }
        Social::FactionId GetFaction(CombatantId id) const { return m_Roster.at(id).Faction; }

        constexpr void Visit(const auto& visitor) {
            for(auto& [id, entry] : m_Roster) {
                visitor(id, entry.Faction, entry.Combatant);
            }
        }

        constexpr void Visit(const auto& visitor) const {
            for(const auto& [id, entry]: m_Roster) {
                visitor(id, entry.Faction, entry.Combatant);
            }
        }

    private:
        std::map<CombatantId, Entry> m_Roster{};
        u32 m_NextId{0};
	};
}