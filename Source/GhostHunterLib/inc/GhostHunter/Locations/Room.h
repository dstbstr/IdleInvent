#pragma once
#include "GhostHunter/Locations/Ghost.h"
#include "Resources/Resource.h"

#include <string>
#include <optional>

namespace GhostHunter {
    struct Room {
        std::string Name;
        ResourceCollection AvailableResources;
        std::optional<Ghost> SpecialGuest;
    };
}