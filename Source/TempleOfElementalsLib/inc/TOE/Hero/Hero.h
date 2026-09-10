#pragma once
#include "Platform/NumTypes.h"

namespace TOE {
    class Hero {
        u8 m_Lvl{0};
        u8 m_Strength{0};
        u8 m_Agility{0};
        u8 m_Intelligence{0};
        u8 m_Vitality{0};

        u8 m_Eva{0};
        u8 m_MagRes{0};
        u8 m_Arm{0};

        u16 m_HP{0};
        u16 m_XP{0};

        std::string m_Name;

    public:
        void SetName(const std::string& name);
        std::string GetName() const;
        void SetHp(u16 hp);
        u16 GetHp() const;
    };
}