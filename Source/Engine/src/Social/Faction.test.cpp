#include "Social/Faction.h"

namespace Social {
    static_assert(FactionId{}.IsValid() == false);
    static_assert(FactionId{0}.IsValid());
    static_assert(FactionId{0}); // bool conversion
    static_assert(FactionId{0} == FactionId{0});
    static_assert(FactionId{1} != FactionId{2});

    enum struct TestFaction : u8 { GoodGuys, BadGuys, Civilians };
    static_assert(ToFactionId(TestFaction::GoodGuys) == ToFactionId(TestFaction::GoodGuys));
    static_assert(ToFactionId(TestFaction::GoodGuys) != ToFactionId(TestFaction::BadGuys));
} // namespace Social