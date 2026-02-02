#pragma once
#include "GhostHunter/Locations/Room.h"
#include "Mechanics/Accumulator.h"
#include "Utilities/Handle.h"

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

    class Location {
        bool m_IsCooledDown{true};
        Accumulator m_CooldownAccumulator{};
        std::vector<Room> m_Rooms{};
        Handle m_TickHandle{InvalidHandle};
        
    public:
        using IdType = LocationName;
        LocationName Id{LocationName::Unset};

        Location(LocationName id);

        bool IsCooledDown() const { return m_IsCooledDown; }
        const std::vector<Room>& GetRooms() const { return m_Rooms; }

        void StartCooldown();
        void StartInvestigation();

        void Tick(BaseTime elapsed);
    };
}