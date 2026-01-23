#pragma once

#include <Platform/NumTypes.h>
#include <string>

namespace GhostHunter {
    enum struct LocationName : u8 {
        Unset,

        Shed,
        School,
        House,
        Church,
        Hotel,
        Prison,
        Hospital,
        Sanitorium,

        COUNT
    };

    std::string ToString(LocationName location);
    std::string Describe(LocationName location);

    struct Location {
        LocationName Name{LocationName::Unset};

        Location(LocationName name) : Name(name) {}
    };
}