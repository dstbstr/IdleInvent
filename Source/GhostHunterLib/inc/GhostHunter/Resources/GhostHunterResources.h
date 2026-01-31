#pragma once
#include "Instrumentation/Logging.h"
#include <Resources/Resource.h>

#include <map>

namespace GhostHunter {
    enum struct ResourceName : u8 {
        Unset,
        Cash,
        Images,
        Audio,
        Video,
        Physical,

        COUNT
    };

    std::string ToString(ResourceName name);
}