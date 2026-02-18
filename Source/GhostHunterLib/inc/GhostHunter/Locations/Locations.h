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

    class Location {
        Accumulator m_CooldownAccumulator;
        std::vector<Room> m_Rooms{};
        std::vector<ScopedHandle> m_Handles{};
        std::optional<ScopedHandle> m_TickHandle{};
    public:
        using IdType = LocationName;
        LocationName Id{LocationName::Unset};

        Location(LocationName id);

        const std::vector<Room>& GetRooms() const { return m_Rooms; }
        std::vector<Room>& GetRooms() { return m_Rooms; }

        void Tick(BaseTime elapsed);

    private:
        void OnCooldown();
        void StartInvestigation();
        void EndInvestigation();
    };

    namespace _LocationDetails {
        std::vector<Room> RoomsByLocation(LocationName location);
        BaseTime GetCooldownTime(LocationName location);
        std::string Describe(LocationName location);
    } // namespace _Details

    std::string ToString(LocationName location);
    std::string Describe(LocationName location);

}