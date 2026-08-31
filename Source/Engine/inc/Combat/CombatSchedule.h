#pragma once

#include "Combat/CombatTypes.h"
#include "GameState/GameTime.h"
#include "Mechanics/Modifier.h"

#include <span>
#include <vector>

namespace Combat {
	struct IScheduler {
        virtual ~IScheduler() = default;

		virtual void Start(std::span<const CombatantId> combatants) = 0;
        virtual void Update(BaseTime elapsed) = 0;
        [[nodiscard]] virtual std::span<const CombatantId> GetReadyCombatants() const = 0;

        virtual bool SkipTurn(CombatantId actor) = 0;
		virtual bool CompleteAction(CombatantId actor) = 0;
        virtual void AddCombatant(CombatantId combatant) = 0;
        virtual void AddCombatant(CombatantId combatant, BaseTime interval) = 0;
        virtual void RemoveCombatant(CombatantId combatant) = 0;

        virtual void ApplySpeedModifier(CombatantId combatant, Modifier modifier) = 0;
	};

    class RoundRobinScheduler final : public IScheduler {
    public:
        void Start(std::span<const CombatantId> combatants) override;
        void Update(BaseTime elapsed) override;
        [[nodiscard]] std::span<const CombatantId> GetReadyCombatants() const override;

        bool SkipTurn(CombatantId actor) override;
        bool CompleteAction(CombatantId actor) override;
        void AddCombatant(CombatantId combatant) override;
        void AddCombatant(CombatantId combatant, BaseTime interval) override;
        void RemoveCombatant(CombatantId combatant) override;

        void ApplySpeedModifier(CombatantId combatant, Modifier modifier) override;
    private:
        std::vector<CombatantId> m_Order{};
        size_t m_Current{0};

        bool Advance(CombatantId actor);
    };
}