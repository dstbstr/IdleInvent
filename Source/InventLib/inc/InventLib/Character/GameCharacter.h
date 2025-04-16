#pragma once

#include "InventLib/Character/Death.h"
#include "InventLib/Resources/InventResources.h"

#include <Resources/Resource.h>

namespace Invent {
    struct GameCharacterSave {
        u8 CurrentAge {0};
        DeathSave DeathData;
        ResourceSave ResourceData;
    };

    struct GameCharacter {
        GameCharacter(size_t baseLifeSpan);
        void Load(const GameCharacterSave& saveData);
        void Save(GameCharacterSave& saveData) const;

        BaseTime CurrentAge{16 * OneGameYear};
        Death CharacterDeath{};

        InventResourceCollection CurrentResources{};

        void Tick(BaseTime elapsed);
    };
}