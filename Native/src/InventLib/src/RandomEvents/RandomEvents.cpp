#include "InventLib/RandomEvents/RandomEvents.h"

#include "InventLib/GameState/GameTime.h"
#include "InventLib/Mechanics/RandomEvent.h"

namespace {
    using namespace Invent::RandomEvents;

    std::chrono::milliseconds MoreCommon(std::chrono::milliseconds chance) {
        return chance == Rarity::Common      ? Rarity::Regular
               : chance == Rarity::Uncommon  ? Rarity::Common
               : chance == Rarity::Rare      ? Rarity::Uncommon
               : chance == Rarity::VeryRare  ? Rarity::Rare
               : chance == Rarity::Legendary ? Rarity::VeryRare
               : chance == Rarity::Mythic    ? Rarity::Legendary
                                             : chance / 2;
    }

    std::chrono::milliseconds LessCommon(std::chrono::milliseconds chance) {
        return chance == Rarity::Regular     ? Rarity::Common
               : chance == Rarity::Common    ? Rarity::Uncommon
               : chance == Rarity::Uncommon  ? Rarity::Rare
               : chance == Rarity::Rare      ? Rarity::VeryRare
               : chance == Rarity::VeryRare  ? Rarity::Legendary
               : chance == Rarity::Legendary ? Rarity::Mythic
                                             : chance * 2;
        }
} // namespace

namespace Invent {
    namespace RandomEvents {
        void Initialize() {
            NaturalEvents::RegisterAll();
        }

        void MakeMoreCommon(RandomEvent event) {
            auto rarity = GetEventChance(event);
            UnregisterEvent(event);
            RegisterEvent(event, MoreCommon(rarity));
        }

        void MakeLessCommon(RandomEvent event) {
            auto rarity = GetEventChance(event);
            UnregisterEvent(event);
            RegisterEvent(event, LessCommon(rarity));
        }
    } // namespace RandomEvents
} // namespace Invent