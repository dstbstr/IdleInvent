#pragma once

#include "Pets/Pets/Pets.h"
#include <GameState/GameTime.h>
#include <Platform/NumTypes.h>

namespace Pets {
    struct PartyNode {
        PetKind Kind{};
        std::vector<PartyNode> Pets{};
    };

    struct Party {
        PartyNode Hero{{PetKind::Hero}};
        u32 MaxDepth{5};
    };
}