#pragma once

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
        using IdType = LocationName;
        LocationName Id{LocationName::Unset};

        Location(LocationName id) : Id(id) {}
    };
}