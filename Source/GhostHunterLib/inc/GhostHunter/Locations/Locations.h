#pragma once

#include <Platform/NumTypes.h>
#include <vector>

namespace GhostHunter {
    enum struct LocationName {
        Unset,

        Shed,
        School,
        House,
        Church,
        Hotel,
        Prison,
        Hospital,
        Sanitorium,
    };

    std::vector<LocationName> GetAllLocationNames();
}