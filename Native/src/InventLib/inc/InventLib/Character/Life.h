#pragma once

#include "InventLib/GameState/GameTime.h"
#include "InventLib/Mechanics/Progression.h"
#include "InventLib/Resources/Resource.h"

namespace Invent {
    struct Life {
        Progression WorkProgress{};
        ResourceName Specialty{ResourceName::Unset};
        ResourceCollection Resources{};

        void Start(ResourceName specialty);
        void Retire();

        void Tick(BaseTime elapsed);
    };
} // namespace Invent