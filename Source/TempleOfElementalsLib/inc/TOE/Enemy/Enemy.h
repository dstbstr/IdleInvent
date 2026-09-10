#pragma once
#include <map>

#include "Platform/NumTypes.h"
#include "TOE/Combat/Effectiveness.h"
#include "TOE/Combat/Elements.h"
#include "TOE/Combat/StatusEffects.h"
#include "Utilities/EnumUtils.h"

namespace TOE {
    class Enemy {
        enum struct Type {
            HUMAN,
            SLIME,
            ELEMENTAL,
            DRAGON,
            GOBLIN,
            GIANT,
            AQUATIC,
            UNDEAD,
            ANIMAL
        };

        u8 m_Lvl{0};
        u8 m_Arm{0};
        u16 m_HP{0};

        u16 m_XP{0};
        u8 m_Eva{0};
        u8 m_MagRes{0};

        std::map<StatusEffects, Effectiveness> m_Status{};
        std::map<Elements, Effectiveness> m_Elements{};
    };
}
