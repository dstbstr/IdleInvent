#pragma once
#include "Instrumentation/Logging.h"
#include <Resources/Resource.h>
#include <Platform/NumTypes.h>
#include <map>

namespace GhostHunter {
    enum struct ResourceName : u8 {
        Unset,
        Cash,
        Belief,
        Evidence,
        Essence,

        COUNT
    };

    std::vector<ResourceName> GetAllResourceNames();
    std::string ToString(ResourceName name);
}