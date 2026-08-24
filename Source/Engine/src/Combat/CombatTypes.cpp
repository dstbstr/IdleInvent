#include "Combat/CombatTypes.h"

namespace Combat {
    static_assert(CombatantId{}.IsValid() == false);
    static_assert(CombatantId{0}.IsValid());
    static_assert(CombatantId{0}); // bool conversion
    static_assert(CombatantId{0} == CombatantId{0});
    static_assert(CombatantId{1} != CombatantId{2});
}