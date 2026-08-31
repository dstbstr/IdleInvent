#pragma once

#include <GameState/GameTime.h>
#include <Platform/NumTypes.h>

namespace Pets {
    struct Party {
        u32 Attack{0};
        u32 Piercing{0};
        BaseTime AttackInterval{};
    };

}