#include "GhostHunter/Locations/Ghost.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Locations/Room.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

#include <vector>
#include <unordered_map>

namespace {
    std::unordered_map<GhostHunter::LocationName, std::vector<GhostHunter::Room>>& GetRooms() {
        static std::unordered_map<GhostHunter::LocationName, std::vector<GhostHunter::Room>> roomsByLocation{};
        return roomsByLocation;
    }
    std::unordered_map<GhostHunter::LocationName, BaseTime>& GetCooldownTimes() {
        static std::unordered_map<GhostHunter::LocationName, BaseTime> cooldownTimesByLocation{};
        return cooldownTimesByLocation;
    }
    std::unordered_map<GhostHunter::LocationName, std::string>& Descriptions() {
        static std::unordered_map<GhostHunter::LocationName, std::string> descriptions{};
        return descriptions;
    }

    ResourceCollection MakeRc(s64 images, s64 audio, s64 video, s64 physical) {
        using enum GhostHunter::ResourceName;
        return CreateRc<GhostHunter::ResourceName>(
            std::pair{Images, images}, std::pair{Audio, audio}, std::pair{Video, video}, std::pair{Physical, physical}
        );
    }
}

#define LOCATION(_Location, _Cooldown, _Description) \
    namespace _Location##Registry { \
        auto Type = GhostHunter::LocationName::_Location; \
        void Register(); \
        struct Registrar { \
            Registrar() { \
                GetCooldownTimes()[Type] = _Cooldown; \
                Descriptions()[Type] = _Description; \
                Register(); \
            } \
        } registrar; \
    }                                                    \
    void _Location##Registry::Register()
#define ROOM(_Name, _Images, _Audio, _Video, _Physical) \
    GetRooms()[Type].push_back({_Name, MakeRc(_Images, _Audio, _Video, _Physical), MakeRc(_Images, _Audio, _Video, _Physical), std::nullopt})
#define GHOST(_Name, _Description) \
    GetRooms()[Type].back().SpecialGuest = GhostHunter::Ghost { _Name, _Description }

LOCATION(Shed, OneSecond * 10, "Your mom's gardening shed.  Haunted?") {
    ROOM("Entry Way", 5, 8, 2, 10);
}
LOCATION(School, OneMinute * 5, "Your younger brother's middle school.  Spooky!") {
    ROOM("Gym", 10, 20, 3, 12);
    ROOM("Cafeteria", 3, 11, 15, 3);
}
LOCATION(House, OneMinute * 10, "The house that belonged to your neighbor before he died. Maybe he's still around") {
    ROOM("Kitchen", 1, 1, 1, 1);
    ROOM("Boiler Room", 2, 2, 2, 2);
    ROOM("Attic", 3, 3, 3, 3);
}
LOCATION(Church, OneMinute * 20, "An abandoned church with boards on the windows. Now we're getting somewhere.") {
    ROOM("Foyer", 1, 1, 1, 1);
    ROOM("Basement", 2, 2, 2, 2);
    ROOM("Confessional", 3, 3, 3, 3);
    ROOM("Cellar", 4, 4, 4, 4);
}
LOCATION(Hotel, OneMinute * 30, "Opulence and ambience are nowhere to be seen anymore. This place is falling apart!") {
    ROOM("Lobby", 1, 1, 1, 1);
    ROOM("Room 102", 2, 2, 2, 2);
    ROOM("Room 212", 3, 3, 3, 3);
    ROOM("Laundry", 4, 4, 4, 4);
}
LOCATION(Prison, OneHour, "Once home to the worst criminals, now home to restless spirits.") {
    ROOM("Cell Block B", 1, 1, 1, 1);
    ROOM("Med Bay", 2, 2, 2, 2);
    ROOM("Showers", 3, 3, 3, 3);
    ROOM("Old Sparky", 4, 4, 4, 4);
    ROOM("Guard House", 5, 5, 5, 5);
}
LOCATION(Hospital, OneHour * 2, "A decrepit hospital in eastern Europe. This place gives me the creeps during the day.") {
    ROOM("Nurses Quarters", 1, 1, 1, 1);
    ROOM("Surgery", 2, 2, 2, 2);
    ROOM("East Wing", 3, 3, 3, 3);
    ROOM("Break Room", 4, 4, 4, 4);
    ROOM("Maternity", 5, 5, 5, 5);
    ROOM("Dispatch", 6, 6, 6, 6);
}
LOCATION(Sanitorium, OneHour * 4, "Who are we kidding, this place is certainly haunted, and wants you to reside here") {
    ROOM("Gathering Space", 1, 1, 1, 1);
    ROOM("Cafeteria", 2, 2, 2, 2);
    ROOM("Pharmacy", 3, 3, 3, 3);
    ROOM("Lobotomy", 4, 4, 4, 4);
    ROOM("Morgue", 5, 5, 5, 5);
    ROOM("West Wing", 6, 6, 6, 6);
    ROOM("Office", 7, 7, 7, 7);
}
#undef LOCATION
#undef ROOM
#undef GHOST

namespace GhostHunter::_LocationDetails {
    std::vector<Room> RoomsByLocation(LocationName location) { return GetRooms().at(location); }

    BaseTime GetCooldownTime(LocationName location) { return GetCooldownTimes().at(location); }

    std::string Describe(LocationName location) { return Descriptions().at(location); }
} // namespace GhostHunter::_LocationDetails