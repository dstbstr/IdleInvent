#pragma once

#include <GameState/GameTime.h>
#include <Platform/NumTypes.h>

namespace Invent {
    enum struct DeathType {
        Sudden,
        Illness,
        Slow
    };

    struct DeathSave {
        u8 DeathYear{0};
        u8 NearDeathYear{0};
        u8 TypeOfDeath : 2;
        u8 HasWarned : 1;
        u8 Padding : 5;
    };

    struct NearDeath {
        BaseTime WarningTime{OneGameYear};
        std::string WarningMessage{};
        bool HasWarned{false};
    };

    struct Death {
        BaseTime TimeOfDeath{OneGameYear * 18};
        DeathType Type{DeathType::Illness};
        NearDeath NearDeathData{};

        void Save(DeathSave& save) const;
    };

    Death GenerateDeath(size_t baseLifeSpan);
    Death LoadDeath(const DeathSave& save);

    std::string ToString(const Death& death);
}